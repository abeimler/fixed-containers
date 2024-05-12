#pragma once

#include <concepts>
#include <memory>

namespace fixed_containers
{
/**
 * Class template that wraps a non-const reference.
 *
 * This class is meant to replace non-const reference function parameters that are *only* written
 * to. This class is not meant to be used as general reference wrapper. In particular, `out` objects
 * are neither copyable nor assignable.
 *
 * To call a function that has an `out` parameter, pass the argument by value using the explicit
 * constructor.
 * Example use in a function:
 * ```c++
 * void set_value(out<int> value)
 * {
 *     *value = 5;
 * }
 *
 * int main()
 * {
 *     int value = 3;
 *     set_value(out{value});
 * }
 * ```
 *
 * In order to pass an `out` parameter as argument to another function that also expects an
 * `out` parameter, create a new `out` object. Example:
 * ```c++
 * void set_value_wrapper(out<int> value)
 * {
 *     set_another_value(out{*value});
 * }
 * ```
 *
 * @tparam T type of the reference
 */
template <typename T>
class out
{
public:
    constexpr explicit out(T& t) noexcept
      : t_{t}
    {
    }

    constexpr out(const out& original) noexcept = delete;
    constexpr out(out&& original) noexcept = delete;
    constexpr out& operator=(const out& original) = delete;
    constexpr out& operator=(out&& original) = delete;

    // non-cost overloads only, to prevent passing by `const`/`const&`
    constexpr T* operator->() noexcept { return std::addressof(t_); }
    constexpr T* operator&() noexcept { return std::addressof(t_); }
    constexpr T& operator*() noexcept { return t_; }

private:
    T& t_;
};

}  // namespace fixed_containers
