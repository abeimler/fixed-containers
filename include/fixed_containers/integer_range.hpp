#pragma once

#include <cassert>
#include <concepts>
#include <cstddef>

namespace fixed_containers
{
template <typename T>
concept IsIntegerRange = requires(const T t, const T t2, std::size_t i) {
    {
        t.start_inclusive()
    } -> std::same_as<std::size_t>;

    {
        t.end_exclusive()
    } -> std::same_as<std::size_t>;

    {
        t.contains(i)
    } -> std::same_as<bool>;

    {
        t.distance()
    } -> std::same_as<std::size_t>;

    {
        t == t2
    } -> std::same_as<bool>;
};

template <std::size_t START_INCLUSIVE, std::size_t END_EXCLUSIVE>
    requires(START_INCLUSIVE <= END_EXCLUSIVE)
class CompileTimeIntegerRange
{
public:
    [[nodiscard]] constexpr std::size_t start_inclusive() const { return START_INCLUSIVE; }
    [[nodiscard]] constexpr std::size_t end_exclusive() const { return END_EXCLUSIVE; }
    [[nodiscard]] constexpr bool contains(const std::size_t i) const
    {
        return START_INCLUSIVE <= i && i < END_EXCLUSIVE;
    }
    [[nodiscard]] constexpr std::size_t distance() const { return END_EXCLUSIVE - START_INCLUSIVE; }

    template <std::size_t START_INCLUSIVE2, std::size_t END_EXCLUSIVE2>
    constexpr bool operator==(
        const CompileTimeIntegerRange<START_INCLUSIVE2, END_EXCLUSIVE2>&) const
    {
        return START_INCLUSIVE == START_INCLUSIVE2 && END_EXCLUSIVE == END_EXCLUSIVE2;
    }
};

class IntegerRange
{
public:
    static constexpr IntegerRange closed_open(const std::size_t start, const std::size_t end)
    {
        return {start, end};
    }

    template <std::size_t START_INCLUSIVE, std::size_t END_EXCLUSIVE>
    static constexpr CompileTimeIntegerRange<START_INCLUSIVE, END_EXCLUSIVE> closed_open()
    {
        return {};
    }

private:
    std::size_t start_inclusive_;
    std::size_t end_exclusive_;

public:
    constexpr IntegerRange()
      : IntegerRange(0, 0)
    {
    }

private:
    constexpr IntegerRange(const std::size_t start_inclusive_inclusive,
                           const std::size_t end_exclusive)
      : start_inclusive_{start_inclusive_inclusive}
      , end_exclusive_{end_exclusive}
    {
        assert(start_inclusive_ <= end_exclusive_);
    }

public:
    [[nodiscard]] constexpr std::size_t start_inclusive() const { return start_inclusive_; }
    [[nodiscard]] constexpr std::size_t end_exclusive() const { return end_exclusive_; }
    [[nodiscard]] constexpr bool contains(const std::size_t i) const
    {
        return start_inclusive_ <= i && i < end_exclusive_;
    }
    [[nodiscard]] constexpr std::size_t distance() const
    {
        return end_exclusive_ - start_inclusive_;
    }

    constexpr bool operator==(const IntegerRange& other) const = default;
};

struct StartingIntegerAndDistance
{
    std::size_t start{};
    std::size_t distance{};

    [[nodiscard]] constexpr IntegerRange to_range() const
    {
        return IntegerRange::closed_open(start, start + distance);
    }
};

}  // namespace fixed_containers
