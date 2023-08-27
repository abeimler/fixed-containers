#include "fixed_containers/enum_map.hpp"
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

static void BM_std_map_construct(benchmark::State& state) {
    for (auto _ : state) {
        for (int i = 0; i < state.range(0); ++i)
        {
            std::map<Keys, int> v = {
                {Keys::Foo, state.range(0)},
                {Keys::Bar, state.range(0) * 2},
                {Keys::Baz, 3 * state.range(0) / 4},
            };
            benchmark::DoNotOptimize(v);
            benchmark::ClobberMemory();
        }
    }
    std::map<Keys, int> v = {
        {Keys::Foo, state.range(0)},
        {Keys::Bar, state.range(0) * 2},
        {Keys::Baz, 3 * state.range(0) / 4},
    };
    state.counters["sizeof_approx"] = sizeof(v) + v.size() * (sizeof(decltype(v)::key_type) + sizeof(decltype(v)::mapped_type));
}
BENCHMARK(BM_std_map_construct)->RangeMultiplier(2)->Range(8U, 8U<<12U);

static void BM_std_unordered_map_construct(benchmark::State& state) {
    for (auto _ : state) {
        for (int i = 0; i < state.range(0); ++i)
        {
            std::unordered_map<Keys, int> v = {
                {Keys::Foo, state.range(0)},
                {Keys::Bar, state.range(0) * 2},
                {Keys::Baz, 3 * state.range(0) / 4},
            };
            benchmark::DoNotOptimize(v);
            benchmark::ClobberMemory();
        }
    }
}
BENCHMARK(BM_std_unordered_map_construct)->RangeMultiplier(2)->Range(8U, 8U<<12U);

static void BM_fixed_enum_map_construct(benchmark::State& state) {
    for (auto _ : state) {
        for (int i = 0; i < state.range(0); ++i)
        {
            fixed_containers::EnumMap<Keys, int> v = {
                {Keys::Foo, state.range(0)},
                {Keys::Bar, state.range(0) * 2},
                {Keys::Baz, 3 * state.range(0) / 4},
            };
            benchmark::DoNotOptimize(v);
            benchmark::ClobberMemory();
        }
    }
    fixed_containers::EnumMap<Keys, int> v = {
        {Keys::Foo, state.range(0)},
        {Keys::Bar, state.range(0) * 2},
        {Keys::Baz, 3 * state.range(0) / 4},
    };
    state.counters["sizeof"] = sizeof(v);
}
BENCHMARK(BM_fixed_enum_map_construct)->RangeMultiplier(2)->Range(8U, 8U<<12U);
