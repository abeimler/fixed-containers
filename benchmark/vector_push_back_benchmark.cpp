#include "RandomLookUpBenchmark.h"

#include "fixed_containers/fixed_vector.hpp"

#include <benchmark/benchmark.h>

#include <vector>

static inline constexpr size_t MAX_SIZE = 8U<<13U;

static void BM_std_vector_push_back_with_reserve(benchmark::State& state) {
    RandomLookUpBenchmarkSuit suit (state.range(0));
    std::vector<int> v;
    v.reserve(state.range(0));
    for (auto _ : state) {
        auto data = v.data();
        benchmark::DoNotOptimize(data);
        for (int i = 0; i < state.range(0); ++i) {
            state.PauseTiming();
            const auto value = suit.values[i];
            state.ResumeTiming();
            v.push_back(value);
            benchmark::ClobberMemory();
        }
        state.PauseTiming();
        v.clear();
        state.ResumeTiming();
    }
}
BENCHMARK(BM_std_vector_push_back_with_reserve)->RangeMultiplier(2)->Range(8U, MAX_SIZE);


static void BM_std_vector_push_back_single(benchmark::State& state) {
    for (auto _ : state) {
        std::vector<int> v;
        v.reserve(1);
        auto data = v.data();
        benchmark::DoNotOptimize(data);
        v.push_back(42);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_std_vector_push_back_single);
static void BM_std_vector_push_back(benchmark::State& state) {
    RandomLookUpBenchmarkSuit suit (state.range(0));
    std::vector<int> v;
    for (auto _ : state) {
        auto data = v.data();
        benchmark::DoNotOptimize(data);
        for (int i = 0; i < state.range(0); ++i) {
            state.PauseTiming();
            const auto value = suit.values[i];
            state.ResumeTiming();
            v.push_back(value);
            benchmark::ClobberMemory();
        }
        state.PauseTiming();
        v.clear();
        state.ResumeTiming();
    }
}
BENCHMARK(BM_std_vector_push_back)->RangeMultiplier(2)->Range(8U, 8U<<12U);

static void BM_std_vector_emplace_back(benchmark::State& state) {
    RandomLookUpBenchmarkSuit suit (state.range(0));
    std::vector<int> v;
    for (auto _ : state) {
        auto data = v.data();
        benchmark::DoNotOptimize(data);
        for (int i = 0; i < state.range(0); ++i) {
            state.PauseTiming();
            const auto value = suit.values[i];
            state.ResumeTiming();
            v.emplace_back(value);
            benchmark::ClobberMemory();
        }
        state.PauseTiming();
        v.clear();
        state.ResumeTiming();
    }
}
BENCHMARK(BM_std_vector_emplace_back)->RangeMultiplier(2)->Range(8U, MAX_SIZE);


static void BM_fixed_vector_push_back_single(benchmark::State& state) {
    for (auto _ : state) {
        fixed_containers::FixedVector<int, 1> v;
        auto data = v.data();
        benchmark::DoNotOptimize(data);
        v.push_back(42);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_fixed_vector_push_back_single);
static void BM_fixed_vector_push_back(benchmark::State& state) {
    RandomLookUpBenchmarkSuit suit (state.range(0));
    fixed_containers::FixedVector<int, MAX_SIZE> v;
    for (auto _ : state) {
        auto data = v.data();
        benchmark::DoNotOptimize(data);
        for (int i = 0; i < state.range(0); ++i) {
            state.PauseTiming();
            const auto value = suit.values[i];
            state.ResumeTiming();
            v.push_back(value);
            benchmark::ClobberMemory();
        }
        state.PauseTiming();
        v.clear();
        state.ResumeTiming();
    }
}
BENCHMARK(BM_fixed_vector_push_back)->RangeMultiplier(2)->Range(8U, MAX_SIZE);
