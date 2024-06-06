#include "fixed_containers/fixed_map.hpp"
#include "LookUpBenchmark.h"
#include <benchmark/benchmark.h>
#include <map>
#include <unordered_map>

static inline constexpr size_t MaxSize = 8U<<12U;

static void BM_std_map_access_single_miss(benchmark::State& state) {
    LookUpBenchmarkSuit suit (10);
    std::map<size_t, int> v;
    const auto key = suit.indexes[0];
    const auto miss_key = suit.indexes[0] + suit.indexes[1] / 3;
    v[key] = suit.values[0];
    for (auto _ : state) {
        benchmark::DoNotOptimize(v[miss_key]);
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_std_map_access_single_miss);
static void BM_std_map_access_single(benchmark::State& state) {
    LookUpBenchmarkSuit suit (1);
    std::map<size_t, int> v;
    const auto key = suit.indexes[0];
    v[key] = suit.values[0];
    for (auto _ : state) {
        benchmark::DoNotOptimize(v[key]);
    }
}
BENCHMARK(BM_std_map_access_single);
static void BM_std_map_random_access(benchmark::State& state) {
    LookUpBenchmarkSuit suit (state.range(0));
    std::map<size_t, int> v;
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
BENCHMARK(BM_std_map_random_access)->RangeMultiplier(2)->Range(8U, MaxSize);

static void BM_std_map_lookup(benchmark::State& state) {
    LookUpBenchmarkSuit suit (state.range(0));
    std::map<size_t, int> v;
    for (int i = 0; i < state.range(0); ++i) {
        v[i] = suit.values[i];
    }
    for (auto _ : state) {
        for (int i = 0; i < state.range(0); ++i) {
            auto& entry = v.at(7);
            benchmark::DoNotOptimize(entry);
        }
    }
    state.counters["sizeof_approx"] = sizeof(v) + v.size() * (sizeof(decltype(v)::key_type) + sizeof(decltype(v)::mapped_type));
}
BENCHMARK(BM_std_map_lookup)->RangeMultiplier(2)->Range(8U, MaxSize);

static void BM_std_unordered_map_access_single_miss(benchmark::State& state) {
    LookUpBenchmarkSuit suit (10);
    std::unordered_map<size_t, int> v;
    const auto key = suit.indexes[0];
    const auto miss_key = suit.indexes[0] + suit.indexes[1] / 3;
    v[key] = suit.values[0];
    for (auto _ : state) {
        benchmark::DoNotOptimize(v[miss_key]);
        benchmark::ClobberMemory();
    }
    state.counters["sizeof_approx"] = sizeof(v) + v.size() * (sizeof(decltype(v)::key_type) + sizeof(decltype(v)::mapped_type));
}
BENCHMARK(BM_std_unordered_map_access_single_miss);
static void BM_std_unordered_map_access_single(benchmark::State& state) {
    LookUpBenchmarkSuit suit (1);
    std::unordered_map<size_t, int> v;
    const auto key = suit.indexes[0];
    v[key] = suit.values[0];
    for (auto _ : state) {
        benchmark::DoNotOptimize(v[key]);
    }
    state.counters["sizeof_approx"] = sizeof(v) + v.size() * (sizeof(decltype(v)::key_type) + sizeof(decltype(v)::mapped_type));
}
BENCHMARK(BM_std_unordered_map_access_single);
static void BM_std_unordered_map_random_access(benchmark::State& state) {
    LookUpBenchmarkSuit suit (state.range(0));
    std::unordered_map<size_t, int> v;
    for (int i = 0; i < suit.size(); ++i) {
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
    state.counters["sizeof_approx"] = sizeof(v) + v.size() * (sizeof(decltype(v)::key_type) + sizeof(decltype(v)::mapped_type));
}
BENCHMARK(BM_std_unordered_map_random_access)->RangeMultiplier(2)->Range(8U, MaxSize);

static void BM_std_unordered_map_lookup(benchmark::State& state) {
    LookUpBenchmarkSuit suit (state.range(0));
    std::unordered_map<size_t, int> v;
    for (int i = 0; i < suit.size(); ++i) {
        v[i] = suit.values[i];
    }
    for (auto _ : state) {
        for (int i = 0; i < state.range(0); ++i) {
            auto& entry = v.at(7);
            benchmark::DoNotOptimize(entry);
        }
    }
    state.counters["sizeof_approx"] = sizeof(v) + v.size() * (sizeof(decltype(v)::key_type) + sizeof(decltype(v)::mapped_type));
}
BENCHMARK(BM_std_unordered_map_lookup)->RangeMultiplier(2)->Range(8U, MaxSize);


static void BM_fixed_map_access_single_miss(benchmark::State& state) {
    LookUpBenchmarkSuit suit (10);
    fixed_containers::FixedMap<size_t, int, MaxSize> v;
    const auto key = suit.indexes[0];
    const auto miss_key = suit.indexes[0] + suit.indexes[1] / 3;
    v[key] = suit.values[0];
    for (auto _ : state) {
        benchmark::DoNotOptimize(v[miss_key]);
        benchmark::ClobberMemory();
    }
    state.counters["sizeof"] = sizeof(v);
}
BENCHMARK(BM_fixed_map_access_single_miss);
static void BM_fixed_map_access_single(benchmark::State& state) {
    LookUpBenchmarkSuit suit (1);
    fixed_containers::FixedMap<size_t, int, MaxSize> v;
    const auto key = suit.indexes[0];
    v[key] = suit.values[0];
    for (auto _ : state) {
        benchmark::DoNotOptimize(v[key]);
    }
    state.counters["sizeof"] = sizeof(v);
}
BENCHMARK(BM_fixed_map_access_single);
static void BM_fixed_map_random_access(benchmark::State& state) {
    LookUpBenchmarkSuit suit (state.range(0));
    fixed_containers::FixedMap<size_t, int, MaxSize> v;
    for (int i = 0; i < suit.size(); ++i) {
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
    state.counters["sizeof"] = sizeof(v);
}
BENCHMARK(BM_fixed_map_random_access)->RangeMultiplier(2)->Range(8U, MaxSize);

static void BM_fixed_map_lookup(benchmark::State& state) {
    LookUpBenchmarkSuit suit (state.range(0));
    fixed_containers::FixedMap<size_t, int, MaxSize> v;
    for (int i = 0; i < suit.size(); ++i) {
        v[i] = suit.values[i];
    }
    for (auto _ : state) {
        for (int i = 0; i < state.range(0); ++i) {
            auto& entry = v.at(7);
            benchmark::DoNotOptimize(entry);
        }
    }
    state.counters["sizeof"] = sizeof(v);
}
BENCHMARK(BM_fixed_map_lookup)->RangeMultiplier(2)->Range(8U, MaxSize);
