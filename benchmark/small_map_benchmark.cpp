#include "RandomLookUpBenchmark.h"

#include "fixed_containers/fixed_map.hpp"
#include "fixed_containers/fixed_unordered_map.hpp"

#include <benchmark/benchmark.h>

#include <map>
#include <unordered_map>

static inline constexpr size_t MAX_SIZE = 8U<<4U;

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

template<class MapType, bool is_allocated_map>
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
        state.counters["MaxSize"] = MAX_SIZE;
        state.counters["sizeof"] = sizeof(v);
    }
    state.counters["size"] = v.size();
}


BENCHMARK(BM_access_single_miss<std::map<int, int>>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);
BENCHMARK(BM_access_single<std::map<int, int>>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);
BENCHMARK(BM_random_access<std::map<int, int>>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);
BENCHMARK(BM_lookup<std::map<int, int>, true>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);

BENCHMARK(BM_access_single_miss<std::unordered_map<int, int>>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);
BENCHMARK(BM_access_single<std::unordered_map<int, int>>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);
BENCHMARK(BM_random_access<std::unordered_map<int, int>>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);
BENCHMARK(BM_lookup<std::unordered_map<int, int>, true>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);

BENCHMARK(BM_access_single_miss<fixed_containers::FixedMap<int, int, MAX_SIZE>>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);
BENCHMARK(BM_access_single<fixed_containers::FixedMap<int, int, MAX_SIZE>>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);
BENCHMARK(BM_random_access<fixed_containers::FixedMap<int, int, MAX_SIZE>>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);
BENCHMARK(BM_lookup<fixed_containers::FixedMap<int, int, MAX_SIZE>, false>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);

BENCHMARK(BM_access_single_miss<fixed_containers::FixedUnorderedMap<int, int, MAX_SIZE>>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);
BENCHMARK(BM_access_single<fixed_containers::FixedUnorderedMap<int, int, MAX_SIZE>>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);
BENCHMARK(BM_random_access<fixed_containers::FixedUnorderedMap<int, int, MAX_SIZE>>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);
BENCHMARK(BM_lookup<fixed_containers::FixedUnorderedMap<int, int, MAX_SIZE>, false>)->RangeMultiplier(2)->Range(8U, MAX_SIZE);
