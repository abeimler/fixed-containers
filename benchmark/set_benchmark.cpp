#include "RandomLookUpBenchmark.h"

#include "fixed_containers/fixed_set.hpp"
#include "fixed_containers/fixed_unordered_set.hpp"

#include <benchmark/benchmark.h>

#include <set>
#include <unordered_set>

static inline constexpr size_t MAX_SIZE = 8U<<13U;

BENCHMARK(BM_set_random_access_insert<std::set<int>, true>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);
BENCHMARK(BM_set_random_access_insert<fixed_containers::FixedSet<int, MAX_SIZE>, false>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);
BENCHMARK(BM_set_random_access_insert<std::unordered_set<int>, true>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);
BENCHMARK(BM_set_random_access_insert<fixed_containers::FixedUnorderedSet<int, MAX_SIZE>, false>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);

BENCHMARK(BM_set_insert<std::set<int>>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);
BENCHMARK(BM_set_insert<fixed_containers::FixedSet<int, MAX_SIZE>>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);
BENCHMARK(BM_set_insert<std::unordered_set<int>>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);
BENCHMARK(BM_set_insert<fixed_containers::FixedUnorderedSet<int, MAX_SIZE>>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);
