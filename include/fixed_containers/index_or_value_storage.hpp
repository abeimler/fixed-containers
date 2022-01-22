#pragma once

#include "fixed_containers/concepts.hpp"

#include <type_traits>
#include <utility>

namespace fixed_containers::index_or_value_storage_detail
{
namespace index_or_value_storage_trivially_copyable
{
template <class T>
union IndexOrValueStorage
{
    std::size_t index;
    T value;
    // clang-format off
    constexpr IndexOrValueStorage() noexcept : index{} { }
    constexpr IndexOrValueStorage(const T& v) : value{v} { }
    constexpr IndexOrValueStorage(T&& v) : value{std::move(v)} { }
    template <class... Args>
    constexpr IndexOrValueStorage(Args&&... args) : value(std::forward<Args>(args)...) { }
    // clang-format on
    constexpr IndexOrValueStorage(const IndexOrValueStorage&) = default;
    constexpr IndexOrValueStorage(IndexOrValueStorage&&) noexcept = default;
    constexpr IndexOrValueStorage& operator=(const IndexOrValueStorage&) = default;
    constexpr IndexOrValueStorage& operator=(IndexOrValueStorage&&) noexcept = default;
    constexpr ~IndexOrValueStorage() noexcept = default;
};

}  // namespace index_or_value_storage_trivially_copyable

namespace index_or_value_storage_non_trivially_copyable
{
template <class T>
union IndexOrValueStorage
{
    std::size_t index;
    T value;
    // clang-format off
    constexpr IndexOrValueStorage() noexcept : index{} { }
    constexpr IndexOrValueStorage(const T& v) : value{v} { }
    constexpr IndexOrValueStorage(T&& v) : value{std::move(v)} { }
    template <class... Args>
    constexpr IndexOrValueStorage(Args&&... args) : value(std::forward<Args>(args)...) { }

    constexpr IndexOrValueStorage(const IndexOrValueStorage&) requires TriviallyCopyConstructible<T> = default;
    constexpr IndexOrValueStorage(IndexOrValueStorage&&) noexcept requires TriviallyMoveConstructible<T> = default;
    constexpr IndexOrValueStorage& operator=(const IndexOrValueStorage&) requires TriviallyCopyAssignable<T> = default;
    constexpr IndexOrValueStorage& operator=(IndexOrValueStorage&&) noexcept requires TriviallyMoveAssignable<T> = default;
    // clang-format on

    constexpr IndexOrValueStorage(const IndexOrValueStorage& other)
      : value{other.value}
    {
    }
    constexpr IndexOrValueStorage(IndexOrValueStorage&& other) noexcept
      : value{std::move(other.value)}
    {
    }

    // CAUTION: we can't assign as we don't know whether value is the active union member.
    // Users are responsible for knowing that and calling the destructor appropriately.
    constexpr IndexOrValueStorage& operator=(const IndexOrValueStorage&) = delete;
    constexpr IndexOrValueStorage& operator=(IndexOrValueStorage&&) noexcept = delete;
    // CAUTION: Users must manually call the destructor of T as they are the ones that keep
    // track of which member is active.
    constexpr ~IndexOrValueStorage() noexcept {}
};

}  // namespace index_or_value_storage_non_trivially_copyable

// IndexOrValueStorage<T> should carry the properties of T. For example, if T fulfils
// std::is_trivially_copy_assignable<T>, then so should IndexOrValueStorage<T>.
// This is done with concepts. However, at the time of writing there is a compiler bug
// that is preventing usage of concepts for destructors: https://bugs.llvm.org/show_bug.cgi?id=46269
// WORKAROUND due to destructors: manually do the split with std::conditional_t.
// NOTE: we branch on TriviallyCopyable instead of TriviallyDestructible because it needs all
// special functions to be trivial. The NonTriviallyCopyable flavor handles triviality separately
// for each special function (except the destructor).
template <class T>
using IndexOrValueStorage =
    std::conditional_t<TriviallyCopyable<T>,
                       index_or_value_storage_trivially_copyable::IndexOrValueStorage<T>,
                       index_or_value_storage_non_trivially_copyable::IndexOrValueStorage<T>>;

}  // namespace fixed_containers::index_or_value_storage_detail
