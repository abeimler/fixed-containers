#define MAGIC_ENUM_RANGE_MIN (-1)
#define MAGIC_ENUM_RANGE_MAX 254

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
struct Range {
    int32_t min{0};
    int32_t max{0};
};

using SmallIntEnumMap = fixed_containers::EnumMap<Keys, int32_t>;
using IntEnumMap = fixed_containers::EnumMap<MoreKeys, int32_t>;
using RangeEnumMap = fixed_containers::EnumMap<MoreKeys, Range>;

struct MyComponent {
    SmallIntEnumMap stats{};
    IntEnumMap values{};
    RangeEnumMap ranges{};
};

constexpr std::size_t MAX_RUNS = 8 << 13;

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
    state.counters["size"] = v.size();
}
BENCHMARK(BM_std_map_construct)->RangeMultiplier(2)->Range(8U, MAX_RUNS);

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
    std::unordered_map<Keys, int> v = {
        {Keys::Foo, state.range(0)},
        {Keys::Bar, state.range(0) * 2},
        {Keys::Baz, 3 * state.range(0) / 4},
    };
    state.counters["sizeof_approx"] = sizeof(v) + v.size() * (sizeof(decltype(v)::key_type) + sizeof(decltype(v)::mapped_type));
    state.counters["size"] = v.size();
}
BENCHMARK(BM_std_unordered_map_construct)->RangeMultiplier(2)->Range(8U, MAX_RUNS);

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
    state.counters["size"] = v.size();
}
BENCHMARK(BM_fixed_enum_map_construct)->RangeMultiplier(2)->Range(8U, MAX_RUNS);


static void BM_fixed_enum_map_construct_SmallIntEnumMap(benchmark::State& state) {
    for (auto _ : state) {
        for (int i = 0; i < state.range(0); ++i)
        {
            SmallIntEnumMap v;
            benchmark::DoNotOptimize(v);
            benchmark::ClobberMemory();
        }
    }
    SmallIntEnumMap v;
    state.counters["sizeof"] = sizeof(v);
    state.counters["size"] = v.size();
}
BENCHMARK(BM_fixed_enum_map_construct_SmallIntEnumMap)->RangeMultiplier(2)->Range(8U, MAX_RUNS);

static void BM_fixed_enum_map_construct_IntEnumMap(benchmark::State& state) {
    for (auto _ : state) {
        for (int i = 0; i < state.range(0); ++i)
        {
            IntEnumMap v;
            benchmark::DoNotOptimize(v);
            benchmark::ClobberMemory();
        }
    }
    IntEnumMap v;
    state.counters["sizeof"] = sizeof(v);
}
BENCHMARK(BM_fixed_enum_map_construct_IntEnumMap)->RangeMultiplier(2)->Range(8U, 8U<<12U);

static void BM_fixed_enum_map_construct_RangeEnumMap(benchmark::State& state) {
    for (auto _ : state) {
        for (int i = 0; i < state.range(0); ++i)
        {
            RangeEnumMap v;
            benchmark::DoNotOptimize(v);
            benchmark::ClobberMemory();
        }
    }
    RangeEnumMap v;
    state.counters["sizeof"] = sizeof(v);
}
BENCHMARK(BM_fixed_enum_map_construct_RangeEnumMap)->RangeMultiplier(2)->Range(8U, MAX_RUNS);


static void BM_fixed_enum_map_construct_MyComponent(benchmark::State& state) {
    for (auto _ : state) {
        for (int i = 0; i < state.range(0); ++i)
        {
            MyComponent v;
            benchmark::DoNotOptimize(v);
            benchmark::ClobberMemory();
        }
    }
    MyComponent v;
    state.counters["sizeof"] = sizeof(v);
}
BENCHMARK(BM_fixed_enum_map_construct_MyComponent)->RangeMultiplier(2)->Range(8U, MAX_RUNS);