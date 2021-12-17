#pragma once

#include <cassert>
#include <compare>
#include <cstddef>
#include <string_view>

namespace fixed_containers
{
/**
 * Fully constexpr implementation of a compile-time literal null-terminated string.
 * StringLiteral is trivially_copyable and standard_layout. Furthermore, all functions are
 * constexpr.
 *
 * Compare:
 * <ul>
 * <li> static constexpr const char* s = "blah";
 * <li> static constexpr const char s[5] = "blah";  // size 5 =  4 chars + null terminator
 * <li> static constexpr StringLiteral s = "blah";  // size 4 (null-terminator is not counted)
 * </ul>
 *
 * StringLiteral is cleaner to use, no confusion about size (null terminator), constant time
 * size(), safe to use as a return type, size not hardcoded in the type (which would make it
 * hard to change the string) and is implicitly convertible to std::string_view and c_str for
 * convenient use in existing APIs.
 */
class StringLiteral
{
public:
    template <std::size_t N>
    /*implicit*/ consteval StringLiteral(const char (&str)[N]) noexcept
      : storage_(str, N - 1)
    {
        assert(c_str()[N - 1] == '\0');
    }

    constexpr StringLiteral() noexcept
      : storage_("", 0)
    {
    }

    [[nodiscard]] constexpr std::size_t size() const { return storage_.size(); }

    [[nodiscard]] constexpr const char* c_str() const { return storage_.data(); }
    /*implicit*/ constexpr operator const char*() const { return c_str(); }

    [[nodiscard]] constexpr std::string_view as_view() const { return storage_; }
    /*implicit*/ constexpr operator std::string_view() const { return as_view(); }

    constexpr bool operator==(const StringLiteral& other) const
    {
        return storage_ == other.storage_;
    }
    constexpr std::strong_ordering operator<=>(const StringLiteral& other) const
    {
        return storage_ <=> other.storage_;
    }

    constexpr bool operator==(const std::string_view& other) const { return storage_ == other; }
    constexpr std::strong_ordering operator<=>(const std::string_view& other) const
    {
        return storage_ <=> other;
    }

    constexpr bool operator==(const char* const other) const { return storage_ == other; }
    constexpr std::strong_ordering operator<=>(const char* const other) const
    {
        return storage_ <=> other;
    }

private:
    std::string_view storage_;
};

}  // namespace fixed_containers
