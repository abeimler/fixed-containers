#include "fixed_containers/fixed_map.hpp"
#include "LookUpBenchmark.h"
#include <benchmark/benchmark.h>
#include <map>
#include <unordered_map>

static inline constexpr size_t MaxSize = 8U<<12U;

static void BM_std_map_insert_single(benchmark::State& state) {
    for (auto _ : state) {
        std::map<size_t, int> v;
        benchmark::DoNotOptimize(v);
        v[8] = 42;
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_std_map_insert_single);
static void BM_std_map_insert(benchmark::State& state) {
    LookUpBenchmarkSuit suit (state.range(0));
    std::map<size_t, int> v;
    for (auto _ : state) {
        for (int i = 0; i < state.range(0); ++i) {
            state.PauseTiming();
            const auto key = suit.indexes[i];
            const auto value = suit.values[i];
            state.ResumeTiming();
            v[key] = value;
            benchmark::ClobberMemory();
        }
        state.PauseTiming();
        v.clear();
        state.ResumeTiming();
    }
}
BENCHMARK(BM_std_map_insert)->RangeMultiplier(2)->Range(8U, MaxSize);


static void BM_std_unordered_map_insert_single(benchmark::State& state) {
    for (auto _ : state) {
        std::map<size_t, int> v;
        benchmark::DoNotOptimize(v);
        v[8] = 42;
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_std_unordered_map_insert_single);
static void BM_std_unordered_map_insert(benchmark::State& state) {
    LookUpBenchmarkSuit suit (state.range(0));
    std::unordered_map<size_t, int> v;
    for (auto _ : state) {
        for (int i = 0; i < state.range(0); ++i) {
            state.PauseTiming();
            const auto key = suit.indexes[i];
            const auto value = suit.values[i];
            state.ResumeTiming();
            v[key] = value;
            benchmark::ClobberMemory();
        }
        state.PauseTiming();
        v.clear();
        state.ResumeTiming();
    }
}
BENCHMARK(BM_std_unordered_map_insert)->RangeMultiplier(2)->Range(8U, MaxSize);


static void BM_fixed_map_insert_single(benchmark::State& state) {
    for (auto _ : state) {
        fixed_containers::FixedMap<size_t, int, 1> v;
        benchmark::DoNotOptimize(v);
        v[8] = 42;
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_fixed_map_insert_single);
static void BM_fixed_map_insert(benchmark::State& state) {
    LookUpBenchmarkSuit suit (state.range(0));
    fixed_containers::FixedMap<size_t, int, MaxSize> v;
    for (auto _ : state) {
        for (int i = 0; i < state.range(0); ++i) {
            state.PauseTiming();
            const auto key = suit.indexes[i];
            const auto value = suit.values[i];
            state.ResumeTiming();
            v[key] = value;
            benchmark::ClobberMemory();
        }
        state.PauseTiming();
        v.clear();
        state.ResumeTiming();
    }
}
BENCHMARK(BM_fixed_map_insert)->RangeMultiplier(2)->Range(8U, MaxSize);
