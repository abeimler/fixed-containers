#include "fixed_containers/fixed_vector.hpp"
#include "LookUpBenchmark.h"
#include <array>
#include <benchmark/benchmark.h>
#include <vector>

static inline constexpr size_t MaxSize = 8U<<12U;

static void BM_std_vector_random_access_with_reserve(benchmark::State& state) {
    LookUpBenchmarkSuit suit (state.range(0));
    std::vector<int> v (state.range(0), 0);
    for (int i = 0; i < state.range(0); ++i) {
        v[i] = suit.values[i];
    }
    for (auto _ : state) {
        auto data = v.data();
        benchmark::DoNotOptimize(data);
        for (int i = 0; i < state.range(0); ++i) {
            state.PauseTiming();
            const auto j = suit.indexes[i];
            state.ResumeTiming();
            benchmark::DoNotOptimize(v[j]);
        }
    }
}
BENCHMARK(BM_std_vector_random_access_with_reserve)->RangeMultiplier(2)->Range(8U, MaxSize);

static void BM_std_vector_random_access(benchmark::State& state) {
    LookUpBenchmarkSuit suit (state.range(0));
    std::vector<int> v;
    for (int i = 0; i < state.range(0); ++i) {
        v.push_back(suit.values[i]);
    }
    for (auto _ : state) {
        auto data = v.data();
        benchmark::DoNotOptimize(data);
        for (int i = 0; i < state.range(0); ++i) {
            state.PauseTiming();
            const auto j = suit.indexes[i];
            state.ResumeTiming();
            benchmark::DoNotOptimize(v[j]);
        }
    }
}
BENCHMARK(BM_std_vector_random_access)->RangeMultiplier(2)->Range(8U, MaxSize);


static void BM_fixed_vector_random_access(benchmark::State& state) {
    LookUpBenchmarkSuit suit (state.range(0));
    fixed_containers::FixedVector<int, MaxSize> v;
    for (int i = 0; i < state.range(0); ++i) {
        v.push_back(suit.values[i]);
    }
    for (auto _ : state) {
        auto data = v.data();
        benchmark::DoNotOptimize(data);
        for (int i = 0; i < state.range(0); ++i) {
            state.PauseTiming();
            const auto j = suit.indexes[i];
            state.ResumeTiming();
            benchmark::DoNotOptimize(v[j]);
        }
    }
}
BENCHMARK(BM_fixed_vector_random_access)->RangeMultiplier(2)->Range(8U, MaxSize);


static void BM_std_array_random_access(benchmark::State& state) {
    LookUpBenchmarkSuit suit (state.range(0));
    constexpr size_t MaxSize = 8U<<10U;
    std::array<int, MaxSize> v;
    for (int i = 0; i < state.range(0); ++i) {
        v[i] = suit.values[i];
    }
    for (auto _ : state) {
        auto data = v.data();
        benchmark::DoNotOptimize(data);
        for (int i = 0; i < state.range(0); ++i) {
            state.PauseTiming();
            const auto j = suit.indexes[i];
            state.ResumeTiming();
            benchmark::DoNotOptimize(v[j]);
        }
    }
}
BENCHMARK(BM_std_array_random_access)->RangeMultiplier(2)->Range(8U, MaxSize);