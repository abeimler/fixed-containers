#ifndef FIXED_CONTAINERS_RANDOMLOOKUPBENCHMARK_H
#define FIXED_CONTAINERS_RANDOMLOOKUPBENCHMARK_H

#include <algorithm>
#include <benchmark/benchmark.h>
#include <cassert>
#include <random>
#include <vector>

class RandomLookUpBenchmarkSuit
{
public:
    std::vector<size_t> indexes;
    std::vector<int> values;
    size_t random_index1{0};
    size_t random_index2{0};

    [[nodiscard]] auto size() const {
        return m_size;
    }

    explicit RandomLookUpBenchmarkSuit(size_t input_size)
        : indexes(input_size, 0)
        , values(input_size, 0)
        , m_size(input_size)
    {
        assert(input_size > 0);
        std::random_device m_r;
        std::default_random_engine e(m_r());

        std::uniform_int_distribution<size_t> uniform_dist_indexes (0U, input_size-1U);
        std::generate(indexes.begin(), indexes.end(), [&](){ return uniform_dist_indexes(e); });

        std::uniform_int_distribution<int> uniform_dist_values (0, 100);
        std::generate(values.begin(), values.end(), [&](){ return uniform_dist_values(e); });

        if (input_size >= 2)
        {
            std::uniform_int_distribution<size_t> uniform_dist_inner_indexes(input_size > 2 ? input_size/2 : 1, input_size - 2);
            random_index1 = uniform_dist_values(e);
            assert(random_index1 >= 1);
            random_index2 = random_index1-1;
        }
    }

private:
    //std::mt19937_64 m_rng;
    size_t m_size;
};

template <typename MapType>
[[maybe_unused]] MapType make_shuffled_map(size_t size)
{
    using KeyType = typename MapType::key_type;
    MapType instance{};
    RandomLookUpBenchmarkSuit suit (size);

    // fill the map completely
    for (size_t i = 0; i < suit.size(); i++)
    {
        instance[static_cast<KeyType>(suit.indexes[i])] = suit.values[i];
    }

    if(suit.size() > 0) {
        for(size_t i = 0;i < suit.indexes.size()/2;i+=3)
        {
            instance.erase(suit.indexes[(997 + i) % (suit.indexes.size() - 1)]);
            instance.erase(suit.indexes[(333 + i)  % (suit.indexes.size() - 1)]);
            instance.erase(suit.indexes[(1023 + i) % (suit.indexes.size() - 1)]);
            instance.erase(suit.indexes[(15 + i) % (suit.indexes.size() - 1)]);
            instance.erase(suit.indexes[(1 + i) % (suit.indexes.size() - 1)]);
        }
    }

    return instance;
}

#endif  // FIXED_CONTAINERS_RANDOMLOOKUPBENCHMARK_H
