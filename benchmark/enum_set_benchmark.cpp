#include "fixed_containers/enum_set.hpp"
#include "LookUpBenchmark.h"
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


static void BM_std_map_bool_access_single_at(benchmark::State& state) {
    LookUpBenchmarkSuit suit (magic_enum::enum_count<Keys>());
    std::map<Keys, bool> v;
    const auto key = magic_enum::enum_cast<Keys>(suit.indexes[0]).value_or(Keys::None);
    v[key] = suit.values[0] >= 50;
    for (auto _ : state) {
        benchmark::DoNotOptimize(v.at(key));
    }
}
BENCHMARK(BM_std_map_bool_access_single_at);
static void BM_std_map_bool_access_single(benchmark::State& state) {
    LookUpBenchmarkSuit suit (magic_enum::enum_count<Keys>());
    std::map<Keys, bool> v;
    const auto key = magic_enum::enum_cast<Keys>(suit.indexes[0]).value_or(Keys::None);
    v[key] = suit.values[0] >= 50;
    for (auto _ : state) {
        benchmark::DoNotOptimize(v[key]);
    }
}
BENCHMARK(BM_std_map_bool_access_single);
static void BM_std_map_bool_random_access(benchmark::State& state) {
    LookUpBenchmarkSuit suit (magic_enum::enum_count<Keys>());
    std::map<Keys, bool> v;
    for (size_t i = 0; i < suit.size(); ++i) {
        const auto key = magic_enum::enum_cast<Keys>(i).value_or(Keys::None);
        v[key] = suit.values[i] >= 50;
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
BENCHMARK(BM_std_map_bool_random_access)->RangeMultiplier(2)->Range(8U, 8U<<12U);


static void BM_std_unordered_map_bool_access_single_at(benchmark::State& state) {
    LookUpBenchmarkSuit suit (magic_enum::enum_count<Keys>());
    std::unordered_map<Keys, bool> v;
    const auto key = magic_enum::enum_cast<Keys>(suit.indexes[0]).value_or(Keys::None);
    v[key] = suit.values[0] >= 50;
    for (auto _ : state) {
        benchmark::DoNotOptimize(v.at(key));
    }
}
BENCHMARK(BM_std_unordered_map_bool_access_single_at);
static void BM_std_unordered_map_bool_access_single(benchmark::State& state) {
    LookUpBenchmarkSuit suit (magic_enum::enum_count<Keys>());
    std::unordered_map<Keys, int> v;
    const auto key = magic_enum::enum_cast<Keys>(suit.indexes[0]).value_or(Keys::None);
    v[key] = suit.values[0] >= 50;
    for (auto _ : state) {
        benchmark::DoNotOptimize(v[key]);
    }
}
BENCHMARK(BM_std_unordered_map_bool_access_single);
static void BM_std_unordered_map_bool_random_access(benchmark::State& state) {
    LookUpBenchmarkSuit suit (magic_enum::enum_count<Keys>());
    std::unordered_map<Keys, bool> v;
    for (int i = 0; i < suit.size(); ++i) {
        const auto key = magic_enum::enum_cast<Keys>(i).value_or(Keys::None);
        v[key] = suit.values[i] >= 50;
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
BENCHMARK(BM_std_unordered_map_bool_random_access)->RangeMultiplier(2)->Range(8U, 8U<<12U);



static void BM_fixed_enum_set_access_single(benchmark::State& state) {
    LookUpBenchmarkSuit suit (magic_enum::enum_count<Keys>());
    fixed_containers::EnumSet<Keys> v;
    const auto key = magic_enum::enum_cast<Keys>(suit.indexes[0]).value_or(Keys::None);
    if(suit.values[0] >= 50) {
        v.insert(key);
    } else {
        v.erase(key);
    }
    for (auto _ : state) {
        benchmark::DoNotOptimize(v.contains(key));
    }
}
BENCHMARK(BM_fixed_enum_set_access_single);
static void BM_fixed_enum_set_random_access(benchmark::State& state) {
    LookUpBenchmarkSuit suit (magic_enum::enum_count<Keys>());
    fixed_containers::EnumSet<Keys> v;
    for (int i = 0; i < suit.size(); ++i) {
        const auto key = magic_enum::enum_cast<Keys>(i).value_or(Keys::None);
        if(suit.values[i] >= 50) {
            v.insert(key);
        } else {
            v.erase(key);
        }
    }
    for (auto _ : state) {
        for (int i = 0; i < state.range(0); ++i) {
            state.PauseTiming();
            const auto key = magic_enum::enum_cast<Keys>(suit.indexes[i % suit.size()]).value_or(Keys::None);
            state.ResumeTiming();
            benchmark::DoNotOptimize(v.contains(key));
        }
    }
}
BENCHMARK(BM_fixed_enum_set_random_access)->RangeMultiplier(2)->Range(8U, 8U<<12U);


static void BM_std_vector_bool_random_access(benchmark::State& state) {
    LookUpBenchmarkSuit suit (magic_enum::enum_count<Keys>());
    std::vector<bool> v (magic_enum::enum_count<Keys>(), 0);
    for (int i = 0; i < suit.size(); ++i) {
        const auto key = magic_enum::enum_cast<Keys>(i).value_or(Keys::None);
        v[magic_enum::enum_integer(key)] = suit.values[i] >= 50;
    }
    for (auto _ : state) {
        for (int i = 0; i < state.range(0); ++i) {
            state.PauseTiming();
            const auto key = magic_enum::enum_cast<Keys>(suit.indexes[i % suit.size()]).value_or(Keys::None);
            const auto j = magic_enum::enum_integer(key);
            state.ResumeTiming();
            benchmark::DoNotOptimize(v[j]);
        }
    }
}
BENCHMARK(BM_std_vector_bool_random_access)->RangeMultiplier(2)->Range(8U, 8U<<12U);



static void BM_std_array_bool_random_access(benchmark::State& state) {
    LookUpBenchmarkSuit suit (magic_enum::enum_count<Keys>());
    std::array<bool, magic_enum::enum_count<Keys>()> v;
    for (int i = 0; i < suit.size(); ++i) {
        const auto key = magic_enum::enum_cast<Keys>(i).value_or(Keys::None);
        v[magic_enum::enum_integer(key)] = suit.values[i] >= 50;
    }
    for (auto _ : state) {
        for (int i = 0; i < state.range(0); ++i) {
            state.PauseTiming();
            const auto key = magic_enum::enum_cast<Keys>(suit.indexes[i % suit.size()]).value_or(Keys::None);
            const auto j = magic_enum::enum_integer(key);
            state.ResumeTiming();
            benchmark::DoNotOptimize(v[j]);
        }
    }
}
BENCHMARK(BM_std_array_bool_random_access)->RangeMultiplier(2)->Range(8U, 8U<<12U);