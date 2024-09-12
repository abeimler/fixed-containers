#include "RandomLookUpBenchmark.h"

#include "fixed_containers/fixed_map.hpp"
#include "fixed_containers/fixed_unordered_map.hpp"

#include <benchmark/benchmark.h>

#include <map>
#include <unordered_map>

static inline constexpr size_t MAX_SIZE = 8U<<6U;

BENCHMARK(BM_map_access_single_miss<std::map<int, int>>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);
BENCHMARK(BM_map_access_single<std::map<int, int>>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);
BENCHMARK(BM_map_random_access<std::map<int, int>>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);
BENCHMARK(BM_map_lookup<std::map<int, int>, true>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);

BENCHMARK(BM_map_access_single_miss<std::unordered_map<int, int>>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);
BENCHMARK(BM_map_access_single<std::unordered_map<int, int>>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);
BENCHMARK(BM_map_random_access<std::unordered_map<int, int>>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);
BENCHMARK(BM_map_lookup<std::unordered_map<int, int>, true>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);

BENCHMARK(BM_map_access_single_miss<fixed_containers::FixedMap<int, int, MAX_SIZE>>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);
BENCHMARK(BM_map_access_single<fixed_containers::FixedMap<int, int, MAX_SIZE>>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);
BENCHMARK(BM_map_random_access<fixed_containers::FixedMap<int, int, MAX_SIZE>>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);
BENCHMARK(BM_map_lookup<fixed_containers::FixedMap<int, int, MAX_SIZE>, false>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);

BENCHMARK(BM_map_access_single_miss<fixed_containers::FixedUnorderedMap<int, int, MAX_SIZE>>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);
BENCHMARK(BM_map_access_single<fixed_containers::FixedUnorderedMap<int, int, MAX_SIZE>>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);
BENCHMARK(BM_map_random_access<fixed_containers::FixedUnorderedMap<int, int, MAX_SIZE>>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);
BENCHMARK(BM_map_lookup<fixed_containers::FixedUnorderedMap<int, int, MAX_SIZE>, false>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);
