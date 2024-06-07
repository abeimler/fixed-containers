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
enum class MoreKeys : uint8_t {
    None,
    Foo,
    Bar,
    Baz,
    Key01,
    Key02,
    Key03,
    Key04,
    Key05,
    Key06,
    Key07,
    Key09,
    Key10,
    Key11,
    Key12,
};
using SmallIntEnumSet = fixed_containers::EnumSet<Keys>;
using IntEnumSet = fixed_containers::EnumSet<MoreKeys>;

static void BM_std_map_bool_construct(benchmark::State& state) {
    for (auto _ : state) {
        for (int i = 0; i < state.range(0); ++i)
        {
            std::map<Keys, bool> v = {
                {Keys::None, false},
                {Keys::Foo, true},
                {Keys::Bar, true},
                {Keys::Baz, true},
            };
            benchmark::DoNotOptimize(v);
            benchmark::ClobberMemory();
        }
    }
    std::map<Keys, bool> v = {
        {Keys::None, false},
        {Keys::Foo, true},
        {Keys::Bar, true},
        {Keys::Baz, true},
    };
    state.counters["sizeof_approx"] = sizeof(v) + v.size() * (sizeof(decltype(v)::key_type) + sizeof(decltype(v)::mapped_type));
}
BENCHMARK(BM_std_map_bool_construct)->RangeMultiplier(2)->Range(8U, 8U<<12U);

static void BM_std_unordered_map_bool_construct(benchmark::State& state) {
    for (auto _ : state) {
        for (int i = 0; i < state.range(0); ++i)
        {
            std::unordered_map<Keys, bool> v = {
                {Keys::None, false},
                {Keys::Foo, true},
                {Keys::Bar, true},
                {Keys::Baz, true},
            };
            benchmark::DoNotOptimize(v);
            benchmark::ClobberMemory();
        }
    }
    std::unordered_map<Keys, bool> v = {
        {Keys::None, false},
        {Keys::Foo, true},
        {Keys::Bar, true},
        {Keys::Baz, true},
    };
    state.counters["sizeof_approx"] = sizeof(v) + v.size() * (sizeof(decltype(v)::key_type) + sizeof(decltype(v)::mapped_type));
}
BENCHMARK(BM_std_unordered_map_bool_construct)->RangeMultiplier(2)->Range(8U, 8U<<12U);

static void BM_std_array_bool_construct(benchmark::State& state) {
    for (auto _ : state) {
        for (int i = 0; i < state.range(0); ++i)
        {
            std::array<bool, 4> v = {
                false,
                true,
                true,
                true,
            };
            benchmark::DoNotOptimize(v);
            benchmark::ClobberMemory();
        }
    }
    std::array<bool, 4> v = {
        false,
        true,
        true,
        true,
    };
    state.counters["sizeof"] = sizeof(v);
}
BENCHMARK(BM_std_array_bool_construct)->RangeMultiplier(2)->Range(8U, 8U<<12U);

static void BM_std_vector_bool_construct(benchmark::State& state) {
    for (auto _ : state) {
        for (int i = 0; i < state.range(0); ++i)
        {
            std::vector<bool> v = {
                false,
                true,
                true,
                true,
            };
            benchmark::DoNotOptimize(v);
            benchmark::ClobberMemory();
        }
    }
    std::vector<bool> v = {
        false,
        true,
        true,
        true,
    };
    state.counters["sizeof"] = sizeof(v) + (v.size() * sizeof(decltype(v)::value_type));
}
BENCHMARK(BM_std_vector_bool_construct)->RangeMultiplier(2)->Range(8U, 8U<<12U);


static void BM_fixed_enum_set_construct(benchmark::State& state) {
    for (auto _ : state) {
        for (int i = 0; i < state.range(0); ++i)
        {
            fixed_containers::EnumSet<Keys> v = {
                Keys::Foo,
                Keys::Bar,
                Keys::Baz,
            };
            benchmark::DoNotOptimize(v);
            benchmark::ClobberMemory();
        }
    }
    fixed_containers::EnumSet<Keys> v = {
        Keys::Foo,
        Keys::Bar,
        Keys::Baz,
    };
    state.counters["sizeof"] = sizeof(v);
}
BENCHMARK(BM_fixed_enum_set_construct)->RangeMultiplier(2)->Range(8U, 8U<<12U);


static void BM_fixed_enum_set_construct_SmallEnumSet(benchmark::State& state) {
    for (auto _ : state) {
        for (int i = 0; i < state.range(0); ++i)
        {
            SmallIntEnumSet v;
            benchmark::DoNotOptimize(v);
            benchmark::ClobberMemory();
        }
    }
    SmallIntEnumSet v;
    state.counters["sizeof"] = sizeof(v);
}
BENCHMARK(BM_fixed_enum_set_construct_SmallEnumSet)->RangeMultiplier(2)->Range(8U, 8U<<12U);

static void BM_fixed_enum_set_construct_IntEnumSet(benchmark::State& state) {
    for (auto _ : state) {
        for (int i = 0; i < state.range(0); ++i)
        {
            IntEnumSet v;
            benchmark::DoNotOptimize(v);
            benchmark::ClobberMemory();
        }
    }
    IntEnumSet v;
    state.counters["sizeof"] = sizeof(v);
}
BENCHMARK(BM_fixed_enum_set_construct_IntEnumSet)->RangeMultiplier(2)->Range(8U, 8U<<12U);