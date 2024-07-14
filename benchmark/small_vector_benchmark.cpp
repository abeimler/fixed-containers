#include "RandomLookUpBenchmark.h"

#include "fixed_containers/fixed_vector.hpp"
#include "fixed_containers/fixed_deque.hpp"
#include "fixed_containers/fixed_queue.hpp"

#include <benchmark/benchmark.h>

#include <array>
#include <vector>
#include <deque>
#include <queue>

static inline constexpr size_t MAX_SIZE = 8U<<6U;

BENCHMARK(BM_array_random_access<std::vector<int>, true>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);
BENCHMARK(BM_array_random_access_no_push_back<std::array<int, MAX_SIZE>, false>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);
BENCHMARK(BM_array_random_access<fixed_containers::FixedVector<int, MAX_SIZE>, false>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);

BENCHMARK(BM_array_random_access<std::deque<int>, true>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);
BENCHMARK(BM_array_random_access<fixed_containers::FixedDeque<int, MAX_SIZE>, false>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);

BENCHMARK(BM_queue_pop<std::queue<int>>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);
BENCHMARK(BM_queue_pop<fixed_containers::FixedQueue<int, MAX_SIZE>>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);

BENCHMARK(BM_array_push_back<std::vector<int>>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);
BENCHMARK(BM_array_push_back<fixed_containers::FixedVector<int, MAX_SIZE>>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);
