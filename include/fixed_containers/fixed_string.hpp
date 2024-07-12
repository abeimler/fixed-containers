#pragma once

#include "fixed_containers/assert_or_abort.hpp"
#include "fixed_containers/concepts.hpp"
#include "fixed_containers/fixed_vector.hpp"
#include "fixed_containers/preconditions.hpp"
#include "fixed_containers/sequence_container_checking.hpp"
#include "fixed_containers/source_location.hpp"

#include <array>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <string_view>

namespace fixed_containers
{

namespace customize {

enum class StringTruncationIsError : bool { NoError = false, Error = true };

}

template <std::size_t MAXIMUM_LENGTH,
          customize::SequenceContainerChecking CheckingType =
              customize::SequenceContainerAbortChecking<char, MAXIMUM_LENGTH>,
          customize::StringTruncationIsError STRING_TRUNCATION_IS_ERROR = customize::StringTruncationIsError::Error>
class FixedString
{
    using Checking = CheckingType;
    using CharT = char;
    using Self = FixedString<MAXIMUM_LENGTH, Checking>;
    using FixedVecStorage = FixedVector<CharT, MAXIMUM_LENGTH + 1, CheckingType>;

    struct ScopedNullTermination
    {
        Self* self_;
        std_transition::source_location loc_;

        constexpr ScopedNullTermination(Self* self,
                                        const std_transition::source_location& loc) noexcept
          : self_(self)
          , loc_(loc)
        {
        }

        constexpr ~ScopedNullTermination() noexcept { self_->null_terminate(loc_); }
    };

public:
    using value_type = typename FixedVecStorage::value_type;
    using size_type = typename FixedVecStorage::size_type;
    using difference_type = typename FixedVecStorage::difference_type;
    using pointer = typename FixedVecStorage::pointer;
    using const_pointer = typename FixedVecStorage::const_pointer;
    using reference = typename FixedVecStorage::reference;
    using const_reference = typename FixedVecStorage::const_reference;
    using const_iterator = typename FixedVecStorage::const_iterator;
    using iterator = typename FixedVecStorage::iterator;
    using reverse_iterator = typename FixedVecStorage::reverse_iterator;
    using const_reverse_iterator = typename FixedVecStorage::const_reverse_iterator;

public:
    [[nodiscard]] static constexpr std::size_t static_max_size() noexcept { return MAXIMUM_LENGTH; }

public:  // Public so this type is a structural type and can thus be used in template parameters
    FixedVecStorage IMPLEMENTATION_DETAIL_DO_NOT_USE_data_;
    bool IMPLEMENTATION_DETAIL_DO_NOT_USE_truncated_{false};

public:
    inline static constexpr std::size_t MaxLength = MAXIMUM_LENGTH;

    constexpr FixedString(const std_transition::source_location& loc =
                              std_transition::source_location::current()) noexcept
      : IMPLEMENTATION_DETAIL_DO_NOT_USE_data_{}
      , IMPLEMENTATION_DETAIL_DO_NOT_USE_truncated_{false}
    {
        null_terminate_at_max_length();
        null_terminate(loc);
    }

    constexpr FixedString(
        size_type count,
        CharT character,
        const std_transition::source_location& loc = std_transition::source_location::current())
      : IMPLEMENTATION_DETAIL_DO_NOT_USE_data_{count, character, loc}
      , IMPLEMENTATION_DETAIL_DO_NOT_USE_truncated_{false}
    {
        null_terminate_at_max_length();
        null_terminate(loc);
    }

    template <std::size_t N>
    explicit(false) constexpr FixedString(const CharT(&s)[N],
                                          const std_transition::source_location& loc = std_transition::source_location::current())
      : FixedString(std::string_view{s}, loc)
    {
        static_assert(N-1 <= MaxLength, "string didn't fit");
    }

    template <std::size_t MAXIMUM_LENGTH_2,
              customize::SequenceContainerChecking CheckingType2 =
                  customize::SequenceContainerAbortChecking<char, MAXIMUM_LENGTH_2>,
              customize::StringTruncationIsError STRING_TRUNCATION_IS_ERROR2 = customize::StringTruncationIsError::Error>
    explicit(false) constexpr FixedString(const FixedString<MAXIMUM_LENGTH_2, CheckingType2, STRING_TRUNCATION_IS_ERROR2>& s,
                                          const std_transition::source_location& loc = std_transition::source_location::current())
      : FixedString(std::string_view{s}, loc)
    {
        static_assert(MAXIMUM_LENGTH_2-1 <= MaxLength, "string didn't fit");
    }

    explicit(false) constexpr FixedString(
        const CharT* char_ptr,
        const std_transition::source_location& loc = std_transition::source_location::current())
      : FixedString(std::string_view{char_ptr}, loc)
    {
    }

    explicit(false) constexpr FixedString(
        const fixed_containers::StringLiteral& s,
        const std_transition::source_location& loc = std_transition::source_location::current())
      : FixedString(std::string_view{s}, loc)
    {
    }

    constexpr FixedString(
        std::initializer_list<CharT> ilist,
        const std_transition::source_location& loc = std_transition::source_location::current())
      : IMPLEMENTATION_DETAIL_DO_NOT_USE_data_{ilist, loc}
      , IMPLEMENTATION_DETAIL_DO_NOT_USE_truncated_{false}
    {
        null_terminate_at_max_length();
        null_terminate(loc);
    }

    explicit(false) constexpr FixedString(const std::string_view& view,
                                          const std_transition::source_location& loc =
                                              std_transition::source_location::current()) noexcept
      : IMPLEMENTATION_DETAIL_DO_NOT_USE_data_{view.begin(),
                                               (STRING_TRUNCATION_IS_ERROR == customize::StringTruncationIsError::NoError)
                                                   ? std::next(view.begin(), std::min(MAXIMUM_LENGTH, view.size()))
                                                   : view.end(),
                                               loc}
      , IMPLEMENTATION_DETAIL_DO_NOT_USE_truncated_{view.length() > IMPLEMENTATION_DETAIL_DO_NOT_USE_data_.max_size()-1}
    {
        null_terminate_at_max_length();
        null_terminate(loc);
    }

    constexpr FixedString& assign(
        size_type count,
        CharT character,
        const std_transition::source_location& loc = std_transition::source_location::current())
    {
        vec().assign(count, character, loc);
        null_terminate(loc);
        return *this;
    }
    template <class InputIt>
    constexpr FixedString& assign(
        InputIt first,
        InputIt last,
        const std_transition::source_location& loc = std_transition::source_location::current())
    {
        vec().assign(first, last, loc);
        null_terminate(loc);
        return *this;
    }
    constexpr FixedString& assign(
        std::initializer_list<CharT> ilist,
        const std_transition::source_location& loc = std_transition::source_location::current())
    {
        vec().assign(ilist, loc);
        null_terminate(loc);
        return *this;
    }
    constexpr FixedString& assign(
        const std::string_view& view,
        const std_transition::source_location& loc = std_transition::source_location::current())
    {
        if constexpr (STRING_TRUNCATION_IS_ERROR == customize::StringTruncationIsError::NoError)
        {
            vec().assign(view.begin(), std::next(view.begin(), std::min(vec().max_size()-1, view.size())), loc);
            IMPLEMENTATION_DETAIL_DO_NOT_USE_truncated_ = view.length() > vec().max_size()-1;
            null_terminate_at_max_length();
        }
        else
        {
            vec().assign(view.begin(), view.end(), loc);
            null_terminate(loc);
        }
        return *this;
    }

    [[nodiscard]] constexpr reference operator[](size_type index) noexcept
    {
        // Cannot capture real source_location for operator[]
        // This operator should not range-check according to the spec, but we want the extra safety.
        return vec().at(index, std_transition::source_location::current());
    }
    [[nodiscard]] constexpr const_reference operator[](size_type index) const noexcept
    {
        // Cannot capture real source_location for operator[]
        // This operator should not range-check according to the spec, but we want the extra safety.
        return vec().at(index, std_transition::source_location::current());
    }

    [[nodiscard]] constexpr reference at(size_type index,
                                         const std_transition::source_location& loc =
                                             std_transition::source_location::current()) noexcept
    {
        return vec().at(index, loc);
    }
    [[nodiscard]] constexpr const_reference at(
        size_type index,
        const std_transition::source_location& loc =
            std_transition::source_location::current()) const noexcept
    {
        return vec().at(index, loc);
    }

    constexpr reference front(
        const std_transition::source_location& loc = std_transition::source_location::current())
    {
        return vec().front(loc);
    }
    [[nodiscard]] constexpr const_reference front(
        const std_transition::source_location& loc =
            std_transition::source_location::current()) const
    {
        return vec().front(loc);
    }
    constexpr reference back(
        const std_transition::source_location& loc = std_transition::source_location::current())
    {
        return vec().back(loc);
    }
    [[nodiscard]] constexpr const_reference back(
        const std_transition::source_location& loc =
            std_transition::source_location::current()) const
    {
        return vec().back(loc);
    }

    [[nodiscard]] constexpr const char* data() const noexcept { return vec().data(); }
    [[nodiscard]] constexpr char* data() noexcept { return vec().data(); }
    [[nodiscard]] constexpr const CharT* c_str() const noexcept { return data(); }

    explicit(false) constexpr operator std::string_view() const
    {
        return std::string_view(data(), length());
    }

    constexpr iterator begin() noexcept { return vec().begin(); }
    [[nodiscard]] constexpr const_iterator begin() const noexcept { return cbegin(); }
    [[nodiscard]] constexpr const_iterator cbegin() const noexcept { return vec().cbegin(); }
    constexpr iterator end() noexcept { return vec().end(); }
    [[nodiscard]] constexpr const_iterator end() const noexcept { return cend(); }
    [[nodiscard]] constexpr const_iterator cend() const noexcept { return vec().cend(); }

    constexpr reverse_iterator rbegin() noexcept { return vec().rbegin(); }
    [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept { return crbegin(); }
    [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept
    {
        return vec().crbegin();
    }
    constexpr reverse_iterator rend() noexcept { return vec().rend(); }
    [[nodiscard]] constexpr const_reverse_iterator rend() const noexcept { return crend(); }
    [[nodiscard]] constexpr const_reverse_iterator crend() const noexcept { return vec().crend(); }

    [[nodiscard]] constexpr bool empty() const noexcept { return length() == 0; }
    [[nodiscard]] constexpr std::size_t length() const noexcept { return vec().size(); }
    [[nodiscard]] constexpr std::size_t size() const noexcept { return length(); }
    [[nodiscard]] constexpr std::size_t max_size() const noexcept { return static_max_size(); }
    constexpr void reserve(const std::size_t new_capacity,
                           const std_transition::source_location& loc =
                               std_transition::source_location::current()) noexcept
    {
        if constexpr (STRING_TRUNCATION_IS_ERROR == customize::StringTruncationIsError::Error)
        {
            if (preconditions::test(new_capacity <= MAXIMUM_LENGTH))
            {
                Checking::length_error(new_capacity, loc);
            }
        }
        // Do nothing
    }
    [[nodiscard]] constexpr std::size_t capacity() const noexcept { return max_size(); }

    constexpr void clear() noexcept
    {
        vec().clear();
        null_terminate(std_transition::source_location::current());
    }

    constexpr iterator insert(
        const_iterator pos,
        CharT character,
        const std_transition::source_location& loc = std_transition::source_location::current())
    {
        const ScopedNullTermination guard{this, loc};
        return vec().insert(pos, character, loc);
    }
    template <InputIterator InputIt>
    constexpr iterator insert(
        const_iterator pos,
        InputIt first,
        InputIt last,
        const std_transition::source_location& loc = std_transition::source_location::current())
    {
        const ScopedNullTermination guard{this, loc};
        return vec().insert(pos, first, last, loc);
    }
    constexpr iterator insert(
        const_iterator pos,
        std::initializer_list<CharT> ilist,
        const std_transition::source_location& loc = std_transition::source_location::current())
    {
        const ScopedNullTermination guard{this, loc};
        return vec().insert(pos, ilist, loc);
    }
    constexpr iterator insert(
        const_iterator pos,
        std::string_view view,
        const std_transition::source_location& loc = std_transition::source_location::current())
    {
        const ScopedNullTermination guard{this, loc};
        return vec().insert(pos, view.begin(), view.end(), loc);
    }

    constexpr iterator erase(
        const_iterator position,
        const std_transition::source_location& loc = std_transition::source_location::current())
    {
        const ScopedNullTermination guard{this, loc};
        return vec().erase(position, loc);
    }
    constexpr iterator erase(
        const_iterator first,
        const_iterator last,
        const std_transition::source_location& loc = std_transition::source_location::current())
    {
        const ScopedNullTermination guard{this, loc};
        return vec().erase(first, last, loc);
    }

    constexpr void push_back(
        CharT character,
        const std_transition::source_location& loc = std_transition::source_location::current())
    {
        vec().push_back(character, loc);
        null_terminate(loc);
    }

    constexpr void pop_back(
        const std_transition::source_location& loc = std_transition::source_location::current())
    {
        vec().pop_back(loc);
        null_terminate(loc);
    }

    template <class InputIt>
    constexpr FixedString& append(
        InputIt first,
        InputIt last,
        const std_transition::source_location& loc = std_transition::source_location::current())
    {
        vec().insert(vec().cend(), first, last, loc);
        null_terminate(loc);
        return *this;
    }
    constexpr FixedString& append(
        std::initializer_list<CharT> ilist,
        const std_transition::source_location& loc = std_transition::source_location::current())
    {
        vec().insert(vec().cend(), ilist, loc);
        null_terminate(loc);
        return *this;
    }
    constexpr FixedString& append(
        std::size_t length, const CharT& chr,
        const std_transition::source_location& loc = std_transition::source_location::current())
    {
        const FixedString str (length, chr, loc);
        return append(std::string_view{str}, loc);
    }
    constexpr void append(const CharT* s, std::size_t length,
                          const std_transition::source_location& loc = std_transition::source_location::current()) {
        append(std::string_view{s, length}, loc);
    }
    constexpr FixedString& append(
        const std::string_view& view,
        const std_transition::source_location& loc = std_transition::source_location::current())
    {
        if constexpr (STRING_TRUNCATION_IS_ERROR == customize::StringTruncationIsError::NoError)
        {
            if(size() < max_size() && !view.empty()) {
                const auto available_space = vec().max_size() - vec().size() - 1;
                if (view.length() >= available_space) {
                    vec().insert(vec().cend(), view.begin(), std::next(view.begin(), available_space), loc);
                    IMPLEMENTATION_DETAIL_DO_NOT_USE_truncated_ = true;
                    //null_terminate(available_space+1);
                    null_terminate_at_max_length();
                } else {
                    vec().insert(vec().cend(), view.begin(), view.end(), loc);
                    IMPLEMENTATION_DETAIL_DO_NOT_USE_truncated_ = false;
                    null_terminate(loc);
                }
            } else {
                IMPLEMENTATION_DETAIL_DO_NOT_USE_truncated_ = !view.empty();
            }
        }
        else
        {
            vec().insert(vec().cend(), view.begin(), view.end(), loc);
            null_terminate(loc);
        }
        return *this;
    }

    constexpr FixedString& operator+=(CharT character)
    {
        return append(character, std_transition::source_location::current());
    }
    constexpr FixedString& operator+=(const CharT* char_ptr)
    {
        return append(std::string_view{char_ptr}, std_transition::source_location::current());
    }
    constexpr FixedString& operator+=(std::initializer_list<CharT> ilist)
    {
        return append(ilist, std_transition::source_location::current());
    }
    constexpr FixedString& operator+=(const std::string_view& view)
    {
        return append(view, std_transition::source_location::current());
    }

    [[nodiscard]] constexpr int compare(std::string_view view) const
    {
        return std::string_view(*this).compare(view);
    }

    template <std::size_t MAXIMUM_LENGTH_2, customize::SequenceContainerChecking CheckingType2, customize::StringTruncationIsError STRING_TRUNCATION_IS_ERROR_2>
    constexpr bool operator==(const FixedString<MAXIMUM_LENGTH_2, CheckingType2, STRING_TRUNCATION_IS_ERROR_2>& other) const
    {
        return as_view() == std::string_view{other};
    }
    constexpr bool operator==(const CharT* other) const
    {
        return as_view() == std::string_view{other};
    }
    constexpr bool operator==(std::string_view view) const noexcept { return as_view() == view; }

    template <std::size_t MAXIMUM_LENGTH_2, customize::SequenceContainerChecking CheckingType2, customize::StringTruncationIsError STRING_TRUNCATION_IS_ERROR_2>
    constexpr std::strong_ordering operator<=>(
        const FixedString<MAXIMUM_LENGTH_2, CheckingType2, STRING_TRUNCATION_IS_ERROR_2>& other) const noexcept
    {
        return as_view() <=> other;
    }
    constexpr std::strong_ordering operator<=>(const CharT* other) const noexcept
    {
        return as_view() <=> std::string_view{other};
    }
    constexpr std::strong_ordering operator<=>(const std::string_view& other) const noexcept
    {
        return as_view() <=> other;
    }

    [[nodiscard]] constexpr bool starts_with(const std::string_view& prefix) const noexcept
    {
        return as_view().starts_with(prefix);
    }
    [[nodiscard]] constexpr bool starts_with(char character) const noexcept
    {
        return as_view().starts_with(character);
    }

    [[nodiscard]] constexpr bool starts_with(const char* char_ptr) const noexcept
    {
        return as_view().starts_with(char_ptr);
    }

    [[nodiscard]] constexpr bool ends_with(const std::string_view& suffix) const noexcept
    {
        return as_view().ends_with(suffix);
    }
    [[nodiscard]] constexpr bool ends_with(char character) const noexcept
    {
        return as_view().ends_with(character);
    }
    [[nodiscard]] constexpr bool ends_with(const char* char_ptr) const noexcept
    {
        return as_view().ends_with(char_ptr);
    }

    [[nodiscard]] constexpr std::string_view substr(
        size_type pos = 0,
        size_t len = MAXIMUM_LENGTH,
        const std_transition::source_location& loc =
            std_transition::source_location::current()) const
    {
        if (preconditions::test(pos < length()))
        {
            Checking::out_of_range(pos, length(), loc);
        }

        return std::string_view(*this).substr(pos, len);
    }

    constexpr void resize(
        size_type count,
        const std_transition::source_location& loc = std_transition::source_location::current())
    {
        resize(count, CharT{}, loc);
    }

    constexpr void resize(
        size_type count,
        CharT character,
        const std_transition::source_location& loc = std_transition::source_location::current())
    {
        vec().resize(count, character, loc);
        null_terminate(loc);
    }

    constexpr FixedString& fill(
        CharT ch,
        const std_transition::source_location& loc = std_transition::source_location::current())
    {
        assign(max_size(), ch, loc);
        null_terminate_at_max_length();
        return *this;
    }

    constexpr void repair(const std_transition::source_location& loc = std_transition::source_location::current()) {
        /// @TODO: get underlying string array for searching null-terminated byte
        if (auto it = std::find(vec().IMPLEMENTATION_DETAIL_DO_NOT_USE_array_.cbegin(), vec().IMPLEMENTATION_DETAIL_DO_NOT_USE_array_.cend(), '\0'); it != vec().IMPLEMENTATION_DETAIL_DO_NOT_USE_array_.cend())
        {
            std::size_t null_index = std::distance(vec().IMPLEMENTATION_DETAIL_DO_NOT_USE_array_.cbegin(), it);
            repair(null_index, loc);
        } else {
            // null-terminated byte string not found
            if constexpr (STRING_TRUNCATION_IS_ERROR == customize::StringTruncationIsError::NoError)
            {
                if (vec().max_size() > 0)
                {
                    repair(vec().max_size(), loc);
                } else {
                    vec().clear();
                    null_terminate(loc);
                }
            } else {
                // without null character
                Checking::out_of_range(vec().max_size(), vec().max_size(), loc);
            }
        }
    }
    constexpr void repair(const std::size_t null_index,
                          const std_transition::source_location& loc = std_transition::source_location::current()) {
        if (preconditions::test(null_index <= MAXIMUM_LENGTH))
        {
            Checking::length_error(null_index, loc);
        }
        vec().IMPLEMENTATION_DETAIL_DO_NOT_USE_size_ = null_index;
        vec().resize(null_index, CharT{}, loc);
        null_terminate(loc);
    }

    [[nodiscard]] constexpr bool truncated() const {
        return IMPLEMENTATION_DETAIL_DO_NOT_USE_truncated_;
    }

    constexpr void clear_truncated() {
        IMPLEMENTATION_DETAIL_DO_NOT_USE_truncated_ = false;
    }

    constexpr void remove_prefix(std::size_t length,
                                 const std_transition::source_location& loc = std_transition::source_location::current()) {
        std::copy(std::next(vec().cbegin(), std::min(vec().size(), length)), std::next(vec().cbegin(), vec().size()), vec().begin());
        vec().resize(vec().size() - std::min(vec().size(), length));
        null_terminate(loc);
    }

    constexpr void remove_suffix(std::size_t length,
                                 const std_transition::source_location& loc = std_transition::source_location::current()) {
        vec().resize(vec().size() - std::min(vec().size(), length));
        null_terminate(loc);
    }

private:
    constexpr void null_terminate(std::size_t n)
    {
        // This bypasses the vector's bounds check
        // This keeps the vector's size equal to the length of the string
        *std::next(vec().data(), static_cast<std::ptrdiff_t>(n)) = '\0';
    }
    constexpr void null_terminate(const std_transition::source_location& loc)
    {
        const std::size_t len = length();
        if (preconditions::test(len <= MAXIMUM_LENGTH))
        {
            Checking::length_error(len + 1, loc);
        }

        null_terminate(length());
    }
    constexpr void null_terminate_at_max_length() { null_terminate(MAXIMUM_LENGTH); }

    [[nodiscard]] constexpr std::string_view as_view() const { return *this; }

    [[nodiscard]] constexpr const FixedVecStorage& vec() const
    {
        return IMPLEMENTATION_DETAIL_DO_NOT_USE_data_;
    }
    constexpr FixedVecStorage& vec() { return IMPLEMENTATION_DETAIL_DO_NOT_USE_data_; }
};

#ifdef FIXED_CONTAINERS_IOSTREAM_SUPPORT
template <std::size_t MAXIMUM_LENGTH, typename CheckingType>
std::ostream& operator<<(std::ostream& stream, const FixedString<MAXIMUM_LENGTH, CheckingType>& str)
{
    return stream << std::string_view{str};
}
#endif

template <std::size_t MAXIMUM_LENGTH, typename CheckingType>
[[nodiscard]] constexpr bool is_full(const FixedString<MAXIMUM_LENGTH, CheckingType>& container)
{
    return container.size() >= container.max_size();
}

/**
 * Construct a FixedString with its capacity being deduced from the number of items being passed.
 */
template <
    customize::SequenceContainerChecking CheckingType,
    std::size_t MAXIMUM_LENGTH_WITH_NULL_TERMINATOR,
    // Exposing this as a template parameter is useful for customization (for example with
    // child classes that set the CheckingType)
    typename FixedStringType = FixedString<MAXIMUM_LENGTH_WITH_NULL_TERMINATOR - 1, CheckingType>>
[[nodiscard]] constexpr FixedStringType make_fixed_string(
    const char (&list)[MAXIMUM_LENGTH_WITH_NULL_TERMINATOR],
    const std_transition::source_location& loc =
        std_transition::source_location::current()) noexcept
{
    constexpr std::size_t MAXIMUM_LENGTH = MAXIMUM_LENGTH_WITH_NULL_TERMINATOR - 1;
    assert_or_abort(*std::next(list, MAXIMUM_LENGTH) == '\0');
    return {std::string_view{std::begin(list), MAXIMUM_LENGTH}, loc};
}

template <std::size_t MAXIMUM_LENGTH_WITH_NULL_TERMINATOR>
[[nodiscard]] constexpr auto make_fixed_string(
    const char (&list)[MAXIMUM_LENGTH_WITH_NULL_TERMINATOR],
    const std_transition::source_location& loc =
        std_transition::source_location::current()) noexcept
{
    constexpr std::size_t MAXIMUM_LENGTH = MAXIMUM_LENGTH_WITH_NULL_TERMINATOR - 1;
    using CheckingType = customize::SequenceContainerAbortChecking<char, MAXIMUM_LENGTH>;
    using FixedStringType = FixedString<MAXIMUM_LENGTH, CheckingType>;
    return make_fixed_string<CheckingType, MAXIMUM_LENGTH_WITH_NULL_TERMINATOR, FixedStringType>(
        list, loc);
}

template <std::size_t MAXIMUM_LENGTH,
          customize::SequenceContainerChecking CheckingType =
              customize::SequenceContainerAbortChecking<char, MAXIMUM_LENGTH>>
using FixedStringTruncable = FixedString<MAXIMUM_LENGTH, CheckingType, customize::StringTruncationIsError::NoError>;

}  // namespace fixed_containers

// Specializations
namespace std
{
template <std::size_t MAXIMUM_LENGTH,
          fixed_containers::customize::SequenceContainerChecking CheckingType>
struct tuple_size<fixed_containers::FixedString<MAXIMUM_LENGTH, CheckingType>>
  : std::integral_constant<std::size_t, 0>
{
    // Implicit Structured Binding due to the fields being public is disabled
};
}  // namespace std
