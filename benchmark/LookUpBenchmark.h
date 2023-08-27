#ifndef FIXED_CONTAINERS_LOOKUPBENCHMARK_H
#define FIXED_CONTAINERS_LOOKUPBENCHMARK_H

#include <algorithm>
#include <benchmark/benchmark.h>
#include <cassert>
#include <random>
#include <vector>

class LookUpBenchmarkSuit {
public:
    std::vector<size_t> indexes;
    std::vector<int> values;

    [[nodiscard]] auto size() const {
        return m_size;
    }

    explicit LookUpBenchmarkSuit(size_t input_size)
        : indexes(input_size, 0)
        , values(input_size, 0)
        , m_size(input_size)
    {
        assert(input_size > 0);
        std::default_random_engine e(m_r());

        std::uniform_int_distribution<size_t> uniform_dist_indexes (0U, input_size-1U);
        std::generate(indexes.begin(), indexes.end(), [&](){ return uniform_dist_indexes(e); });

        std::uniform_int_distribution<int> uniform_dist_values (0, 100);
        std::generate(values.begin(), values.end(), [&](){ return uniform_dist_values(e); });
    }

private:
    //std::mt19937_64 m_rng;
    std::random_device m_r;
    size_t m_size;
};

#endif  // FIXED_CONTAINERS_LOOKUPBENCHMARK_H
