#include "RandomLookUpBenchmark.h"

#include "fixed_containers/enum_map.hpp"

#include <benchmark/benchmark.h>
#include <magic_enum.hpp>

#include <map>
#include <unordered_map>

enum class Keys : uint8_t {
    None,
    Foo,
    Bar,
    Baz,
};

constexpr std::size_t MAX_RUNS = 8 << 13;

static void BM_std_map_access_single_at(benchmark::State& state) {
    RandomLookUpBenchmarkSuit suit (magic_enum::enum_count<Keys>());
    std::map<Keys, int> v;
    const auto key = magic_enum::enum_cast<Keys>(suit.indexes[0]).value_or(Keys::None);
    v[key] = suit.values[0];
    for (auto _ : state) {
        benchmark::DoNotOptimize(v.at(key));
    }
}
BENCHMARK(BM_std_map_access_single_at);
static void BM_std_map_access_single(benchmark::State& state) {
    RandomLookUpBenchmarkSuit suit (magic_enum::enum_count<Keys>());
    std::map<Keys, int> v;
    const auto key = magic_enum::enum_cast<Keys>(suit.indexes[0]).value_or(Keys::None);
    v[key] = suit.values[0];
    for (auto _ : state) {
        benchmark::DoNotOptimize(v[key]);
    }
}
BENCHMARK(BM_std_map_access_single);
static void BM_std_map_random_access(benchmark::State& state) {
    RandomLookUpBenchmarkSuit suit (magic_enum::enum_count<Keys>());
    std::map<Keys, int> v;
    for (size_t i = 0; i < suit.size(); ++i) {
        const auto key = magic_enum::enum_cast<Keys>(i).value_or(Keys::None);
        v[key] = suit.values[i];
    }
    for (auto _ : state) {
        for (int i = 0; i < state.range(0); ++i) {
            state.PauseTiming();
            const auto key = magic_enum::enum_cast<Keys>(suit.indexes[i % suit.size()]).value_or(Keys::None);
            state.ResumeTiming();
            benchmark::DoNotOptimize(v[key]);
        }
    }
}
BENCHMARK(BM_std_map_random_access)->RangeMultiplier(2)->Range(8U, MAX_RUNS);


static void BM_std_unordered_map_access_single(benchmark::State& state) {
    RandomLookUpBenchmarkSuit suit (magic_enum::enum_count<Keys>());
    std::unordered_map<Keys, int> v;
    const auto key = magic_enum::enum_cast<Keys>(suit.indexes[0]).value_or(Keys::None);
    v[key] = suit.values[0];
    for (auto _ : state) {
        benchmark::DoNotOptimize(v[key]);
    }
}
BENCHMARK(BM_std_unordered_map_access_single);
static void BM_std_unordered_map_random_access(benchmark::State& state) {
    RandomLookUpBenchmarkSuit suit (magic_enum::enum_count<Keys>());
    std::unordered_map<Keys, int> v;
    for (int i = 0; i < suit.size(); ++i) {
        const auto key = magic_enum::enum_cast<Keys>(i).value_or(Keys::None);
        v[key] = suit.values[i];
    }
    for (auto _ : state) {
        for (int i = 0; i < state.range(0); ++i) {
            state.PauseTiming();
            const auto key = magic_enum::enum_cast<Keys>(suit.indexes[i % suit.size()]).value_or(Keys::None);
            state.ResumeTiming();
            benchmark::DoNotOptimize(v[key]);
        }
    }
}
BENCHMARK(BM_std_unordered_map_random_access)->RangeMultiplier(2)->Range(8U, MAX_RUNS);



static void BM_fixed_enum_map_access_single(benchmark::State& state) {
    RandomLookUpBenchmarkSuit suit (magic_enum::enum_count<Keys>());
    fixed_containers::EnumMap<Keys, int> v;
    const auto key = magic_enum::enum_cast<Keys>(suit.indexes[0]).value_or(Keys::None);
    v[key] = suit.values[0];
    for (auto _ : state) {
        benchmark::DoNotOptimize(v[key]);
    }
}
BENCHMARK(BM_fixed_enum_map_access_single);
static void BM_fixed_enum_map_random_access(benchmark::State& state) {
    RandomLookUpBenchmarkSuit suit (magic_enum::enum_count<Keys>());
    fixed_containers::EnumMap<Keys, int> v;
    for (int i = 0; i < suit.size(); ++i) {
        const auto key = magic_enum::enum_cast<Keys>(i).value_or(Keys::None);
        v[key] = suit.values[i];
    }
    for (auto _ : state) {
        for (int i = 0; i < state.range(0); ++i) {
            state.PauseTiming();
            const auto key = magic_enum::enum_cast<Keys>(suit.indexes[i % suit.size()]).value_or(Keys::None);
            state.ResumeTiming();
            benchmark::DoNotOptimize(v[key]);
        }
    }
}
BENCHMARK(BM_fixed_enum_map_random_access)->RangeMultiplier(2)->Range(8U, MAX_RUNS);
