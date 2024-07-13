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


template<class MapType>
static void BM_access_single_miss(benchmark::State& state) {
    RandomLookUpBenchmarkSuit suit (10);
    MapType v{};
    const auto key = suit.random_index1;
    const auto miss_key = suit.random_index2;
    v[key] = suit.values[0];
    for (auto _ : state) {
        benchmark::DoNotOptimize(v[miss_key]);
        benchmark::ClobberMemory();
    }
}

template<class MapType>
static void BM_access_single(benchmark::State& state) {
    RandomLookUpBenchmarkSuit suit (1);
    MapType v{};
    const auto key = suit.random_index1;
    v[key] = suit.values[0];
    for (auto _ : state) {
        benchmark::DoNotOptimize(v[key]);
    }
}

template<class MapType>
static void BM_random_access(benchmark::State& state) {
    RandomLookUpBenchmarkSuit suit (state.range(0));
    MapType v{};
    for (int i = 0; i < state.range(0); ++i) {
        v[i] = suit.values[i];
    }
    for (auto _ : state) {
        for (int i = 0; i < state.range(0); ++i) {
            state.PauseTiming();
            const auto key = suit.indexes[i];
            state.ResumeTiming();
            benchmark::DoNotOptimize(v[key]);
        }
    }
}

template<class MapType, bool is_allocated_map, size_t MAX_SIZE = 0>
static void BM_lookup(benchmark::State& state) {
    RandomLookUpBenchmarkSuit suit (state.range(0));
    MapType v{};
    for (int i = 0; i < state.range(0); ++i) {
        v[i] = suit.values[i];
    }
    for (auto _ : state) {
        for (int i = 0; i < state.range(0); ++i) {
            auto& entry = v.at(7);
            benchmark::DoNotOptimize(entry);
        }
    }

    if constexpr (is_allocated_map)
    {
        state.counters["sizeof_approx"] = sizeof(v) + v.size() * (sizeof(typename decltype(v)::key_type) +
                                                                  sizeof(typename decltype(v)::mapped_type));
    } else {
        state.counters["MaxSize"] = v.max_size();
        state.counters["sizeof"] = sizeof(v);
    }
    state.counters["size"] = v.size();
}


#endif  // FIXED_CONTAINERS_RANDOMLOOKUPBENCHMARK_H
