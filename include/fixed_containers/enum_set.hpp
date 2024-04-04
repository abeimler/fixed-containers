#pragma once

#include "fixed_containers/assert_or_abort.hpp"
#include "fixed_containers/bidirectional_iterator.hpp"
#include "fixed_containers/concepts.hpp"
#include "fixed_containers/enum_utils.hpp"
#include "fixed_containers/erase_if.hpp"
#include "fixed_containers/filtered_integer_range_iterator.hpp"
#include "fixed_containers/max_size.hpp"

#ifdef USE_BIT_SET_FOR_ENUM_SET
#include "bit_set.hpp"
#endif
#include <array>
#include <cstddef>
#include <initializer_list>

namespace fixed_containers::enum_set_detail
{
template <class K, class EnumSetType>
class EnumSetBuilder
{
public:
    constexpr EnumSetBuilder() {}

    constexpr EnumSetBuilder& insert(const K& key) & noexcept
    {
        enum_set_.insert(key);
        return *this;
    }
    constexpr EnumSetBuilder&& insert(const K& key) && noexcept
    {
        enum_set_.insert(key);
        return std::move(*this);
    }

    constexpr EnumSetBuilder& insert(std::initializer_list<K> list) & noexcept
    {
        enum_set_.insert(list);
        return *this;
    }
    constexpr EnumSetBuilder&& insert(std::initializer_list<K> list) && noexcept
    {
        enum_set_.insert(list);
        return std::move(*this);
    }

    template <InputIterator InputIt>
    constexpr EnumSetBuilder& insert(InputIt first, InputIt last) & noexcept
    {
        enum_set_.insert(first, last);
        return *this;
    }
    template <InputIterator InputIt>
    constexpr EnumSetBuilder&& insert(InputIt first, InputIt last) && noexcept
    {
        enum_set_.insert(first, last);
        return std::move(*this);
    }

    template <class Container>
    constexpr EnumSetBuilder& insert(const Container& container) & noexcept
    {
        enum_set_.insert(container.cbegin(), container.cend());
        return *this;
    }
    template <class Container>
    constexpr EnumSetBuilder&& insert(const Container& container) && noexcept
    {
        enum_set_.insert(container.cbegin(), container.cend());
        return std::move(*this);
    }

    constexpr EnumSetBuilder& erase(const K& key) & noexcept
    {
        enum_set_.erase(key);
        return *this;
    }
    constexpr EnumSetBuilder&& erase(const K& key) && noexcept
    {
        enum_set_.erase(key);
        return std::move(*this);
    }

    constexpr EnumSetType build() const& { return enum_set_; }
    constexpr EnumSetType build() && { return std::move(enum_set_); }

private:
    EnumSetType enum_set_;
};
}  // namespace fixed_containers::enum_set_detail

namespace fixed_containers
{
/**
 * Fixed-capacity set for enum keys. Properties:
 *  - constexpr
 *  - retains the properties of V (e.g. if T is trivially copyable, then so is EnumMap<K, V>)
 *  - no pointers stored (data layout is purely self-referential and can be serialized directly)
 *  - no dynamic allocations
 *
 * Note that despite what the underlying implementation might suggest, this is NOT a bitset. EnumSet
 * is a set for the special case when the keys are enum values, so the API matches that of std::set.
 */
template <class K>
class EnumSet
{
    using Self = EnumSet<K>;
#ifdef USE_BIT_SET_FOR_ENUM_SET
    using Block = std::size_t;
#endif

public:
    using key_type = K;
    using value_type = K;
    using const_reference = const value_type&;
    using reference = const_reference;
    using const_pointer = std::add_pointer_t<const_reference>;
    using pointer = const_pointer;

private:
    using EnumAdapterType = rich_enums::EnumAdapter<K>;
    static constexpr std::size_t ENUM_COUNT = EnumAdapterType::count();
    using KeyArrayType = std::array<K, ENUM_COUNT>;
    static constexpr const KeyArrayType& ENUM_VALUES = EnumAdapterType::values();
#ifdef USE_BIT_SET_FOR_ENUM_SET
    using ValueArrayType = xstd::bit_set<ENUM_COUNT, Block>;
#else
    // std::bitset is not sufficiently constexpr to use here, using a std::array instead.
    using ValueArrayType = std::array<bool, ENUM_COUNT>;
#endif

    struct IndexPredicate
    {
#ifdef USE_BIT_SET_FOR_ENUM_SET
        const ValueArrayType* array_set_;
        constexpr bool operator()(const int i) const { return array_set_->contains(i); }
#else
        const ValueArrayType* array_set_;
        constexpr bool operator()(const std::size_t i) const { return (*array_set_)[i]; }
#endif
        constexpr bool operator==(const IndexPredicate&) const = default;
    };

    class ReferenceProvider
    {
        FilteredIntegerRangeEntryProvider<IndexPredicate, CompileTimeIntegerRange<0, ENUM_COUNT>>
            present_indices_;

    public:
        constexpr ReferenceProvider()
          : ReferenceProvider(nullptr, ENUM_COUNT)
        {
        }

        constexpr ReferenceProvider(const ValueArrayType* array_set,
                                    const std::size_t current_index)
          : present_indices_{
                CompileTimeIntegerRange<0, ENUM_COUNT>{}, current_index, IndexPredicate{array_set}}
        {
        }

        constexpr void advance() noexcept { present_indices_.advance(); }
        constexpr void recede() noexcept { present_indices_.recede(); }
        constexpr const_reference get() const noexcept
        {
            return ENUM_VALUES[present_indices_.get()];
        }
        constexpr bool operator==(const ReferenceProvider&) const = default;
    };

    template <IteratorDirection DIRECTION>
    using IteratorImpl = BidirectionalIterator<ReferenceProvider,
                                               ReferenceProvider,
                                               IteratorConstness::CONSTANT_ITERATOR,
                                               DIRECTION>;

public:
    using const_iterator = IteratorImpl<IteratorDirection::FORWARD>;
    using iterator = const_iterator;
    using const_reverse_iterator = IteratorImpl<IteratorDirection::REVERSE>;
    using reverse_iterator = const_reverse_iterator;
    using size_type = typename KeyArrayType::size_type;
    using difference_type = typename KeyArrayType::difference_type;

public:
    using Builder = enum_set_detail::EnumSetBuilder<K, Self>;

    template <class EnumSetType = Self>
    static constexpr EnumSetType all()
    {
        EnumSetType output{};
        output.insert(ENUM_VALUES.cbegin(), ENUM_VALUES.cend());
        return output;
    }

    template <class EnumSetType = Self>
    static constexpr EnumSetType none()
    {
        return {};
    }

    template <class Container, class EnumSetType = Self>
    static constexpr EnumSetType complement_of(const Container& s)
    {
        EnumSetType output = all<EnumSetType>();
        for (const K& key : s)
        {
            output.erase(key);
        }

        return output;
    }

    template <class Container, class EnumSetType = Self>
    static constexpr EnumSetType copy_of(const Container& container)
    {
        EnumSetType output{};
        output.insert(container.begin(), container.end());
        return output;
    }

    [[nodiscard]] static constexpr std::size_t static_max_size() noexcept {
#ifdef USE_BIT_SET_FOR_ENUM_SET
        return ValueArrayType::max_size();
#else
        return ENUM_COUNT;
#endif
    }

public:  // Public so this type is a structural type and can thus be used in template parameters
    // std::bitset is not sufficiently constexpr to use here, using a std::array instead.
    ValueArrayType IMPLEMENTATION_DETAIL_DO_NOT_USE_array_set_;
    std::size_t IMPLEMENTATION_DETAIL_DO_NOT_USE_size_;

public:
    constexpr EnumSet() noexcept
      : IMPLEMENTATION_DETAIL_DO_NOT_USE_array_set_()
      , IMPLEMENTATION_DETAIL_DO_NOT_USE_size_{}
    {
    }

    template <InputIterator InputIt>
    constexpr EnumSet(InputIt first, InputIt last) noexcept
      : EnumSet()
    {
        insert(first, last);
    }

    constexpr EnumSet(std::initializer_list<K> list) noexcept
      : EnumSet()
    {
        insert(list);
    }

    constexpr EnumSet(std::initializer_list<std::pair<K, bool>> list) noexcept
        : EnumSet()
    {
      for(const auto& [key, value] : list) {
        if(value) {
          insert(key);
        }
      }
    }

public:
    constexpr const_iterator cbegin() const noexcept { return create_const_iterator(0); }
    constexpr const_iterator cend() const noexcept { return create_const_iterator(ENUM_COUNT); }
    constexpr const_iterator begin() const noexcept { return cbegin(); }
    constexpr const_iterator end() const noexcept { return cend(); }

    constexpr const_reverse_iterator crbegin() const noexcept
    {
        return create_const_reverse_iterator(ENUM_COUNT);
    }
    constexpr const_reverse_iterator crend() const noexcept
    {
        return create_const_reverse_iterator(0);
    }
    constexpr const_reverse_iterator rbegin() const noexcept { return crbegin(); }
    constexpr const_reverse_iterator rend() const noexcept { return crend(); }

    [[nodiscard]] constexpr std::size_t max_size() const noexcept { return static_max_size(); }
    [[nodiscard]] constexpr std::size_t size() const noexcept
    {
#ifdef USE_BIT_SET_FOR_ENUM_SET
        return IMPLEMENTATION_DETAIL_DO_NOT_USE_array_set_.size();
#else
        return IMPLEMENTATION_DETAIL_DO_NOT_USE_size_;
#endif
    }
    [[nodiscard]] constexpr bool empty() const noexcept {
#ifdef USE_BIT_SET_FOR_ENUM_SET
        return IMPLEMENTATION_DETAIL_DO_NOT_USE_array_set_.empty();
#else
        return size() == 0;
#endif
    }

    constexpr void clear() noexcept
    {
#ifdef USE_BIT_SET_FOR_ENUM_SET
        IMPLEMENTATION_DETAIL_DO_NOT_USE_array_set_.clear();
#else
        const std::size_t sz = array_set().size();
        for (std::size_t i = 0; i < sz; i++)
        {
            if (contains_at(i))
            {
                reset_at(i);
            }
        }
#endif
    }
    constexpr std::pair<const_iterator, bool> insert(const K& key) noexcept
    {
        const std::size_t ordinal = EnumAdapterType::ordinal(key);
#ifdef USE_BIT_SET_FOR_ENUM_SET
        const auto ret = IMPLEMENTATION_DETAIL_DO_NOT_USE_array_set_.insert(ordinal);
        return {create_const_iterator(ordinal), ret.second};
#else
        if (contains_at(ordinal))
        {
            return {create_const_iterator(ordinal), false};
        }

        increment_size();
        array_set_unchecked_at(EnumAdapterType::ordinal(key)) = true;
        return {create_const_iterator(ordinal), true};
#endif
    }
    constexpr const_iterator insert(const_iterator /*hint*/, const K& key) noexcept
    {
        return insert(key).first;
    }
    constexpr void insert(std::initializer_list<K> list) noexcept
    {
        this->insert(list.begin(), list.end());
    }
    template <InputIterator InputIt>
    constexpr void insert(InputIt first, InputIt last) noexcept
    {
        for (; first != last; std::advance(first, 1))
        {
            this->insert(*first);
        }
    }

    template <class... Args>
    constexpr std::pair<const_iterator, bool> emplace(Args&&... args)
    {
        return insert(K{std::forward<Args>(args)...});
    }
    template <class... Args>
    constexpr iterator emplace_hint(const_iterator hint, Args&&... args)
    {
        return insert(hint, K{std::forward<Args>(args)...});
    }

    constexpr const_iterator erase(const_iterator pos) noexcept
    {
        assert_or_abort(pos != cend());
        const std::size_t i = EnumAdapterType::ordinal(*pos);
#ifdef USE_BIT_SET_FOR_ENUM_SET
        IMPLEMENTATION_DETAIL_DO_NOT_USE_array_set_.erase(i);
        return create_const_iterator(i);
#else
        assert_or_abort(contains_at(i));
        reset_at(i);
        return create_const_iterator(i);
#endif
    }

    constexpr const_iterator erase(const_iterator first, const_iterator last) noexcept
    {
        const std::size_t from = first == end() ? ENUM_COUNT : EnumAdapterType::ordinal(*first);
        const std::size_t to = last == end() ? ENUM_COUNT : EnumAdapterType::ordinal(*last);
        assert_or_abort(from <= to);

        for (std::size_t i = from; i < to; i++)
        {
#ifdef USE_BIT_SET_FOR_ENUM_SET
            IMPLEMENTATION_DETAIL_DO_NOT_USE_array_set_.erase(i);
#else
            if (contains_at(i))
            {
                reset_at(i);
            }
#endif
        }

        return create_const_iterator(to);
    }

    constexpr size_type erase(const K& key) noexcept
    {
        const std::size_t i = EnumAdapterType::ordinal(key);
        if (!contains_at(i))
        {
            return 0;
        }
#ifdef USE_BIT_SET_FOR_ENUM_SET
        return IMPLEMENTATION_DETAIL_DO_NOT_USE_array_set_.erase(i);
#else
        reset_at(i);
        return 1;
#endif
    }

    [[nodiscard]] constexpr bool contains(const K& key) const noexcept
    {
#ifdef USE_BIT_SET_FOR_ENUM_SET
        return IMPLEMENTATION_DETAIL_DO_NOT_USE_array_set_.contains(EnumAdapterType::ordinal(key));
#else
        return contains_at(EnumAdapterType::ordinal(key));
#endif
    }

    constexpr bool operator==(const EnumSet<K>& other) const
    {
        return array_set() == other.array_set();
    }

    static constexpr auto keys() noexcept
    {
        return magic_enum::enum_values<K>();
    }

    template <class Container>
    constexpr void set(const Container& container)
    {
        insert(container.begin(), container.end());
    }
    constexpr void set(std::initializer_list<K> list) noexcept
    {
        insert(list.begin(), list.end());
    }
    constexpr void set(const K& key, bool value) noexcept
    {
        if (value) {
            insert(key);
        } else {
            erase(key);
        }
    }


    [[nodiscard]] constexpr const auto& data() const noexcept {
        return IMPLEMENTATION_DETAIL_DO_NOT_USE_array_set_;
    }
    constexpr void reset(const ValueArrayType& data) noexcept {
        IMPLEMENTATION_DETAIL_DO_NOT_USE_array_set_ = data;
    }

private:
    [[nodiscard]] constexpr const auto& array_set() const
    {
        return IMPLEMENTATION_DETAIL_DO_NOT_USE_array_set_;
    }
    [[nodiscard]] constexpr auto& array_set()
    {
        return IMPLEMENTATION_DETAIL_DO_NOT_USE_array_set_;
    }
    [[nodiscard]] constexpr const bool& array_set_unchecked_at(const std::size_t i) const
    {
        return IMPLEMENTATION_DETAIL_DO_NOT_USE_array_set_[i];
    }
    [[nodiscard]] constexpr bool& array_set_unchecked_at(const std::size_t i)
    {
        return IMPLEMENTATION_DETAIL_DO_NOT_USE_array_set_[i];
    }
    constexpr void increment_size(const std::size_t n = 1)
    {
        IMPLEMENTATION_DETAIL_DO_NOT_USE_size_ += n;
    }
    constexpr void decrement_size(const std::size_t n = 1)
    {
        IMPLEMENTATION_DETAIL_DO_NOT_USE_size_ -= n;
    }

    [[nodiscard]] constexpr const_iterator create_const_iterator(const std::size_t start_index) const noexcept
    {
        return const_iterator{ReferenceProvider{&array_set(), start_index}};
    }
    [[nodiscard]] constexpr const_reverse_iterator create_const_reverse_iterator(
        const std::size_t start_index) const noexcept
    {
        return const_reverse_iterator{ReferenceProvider{&array_set(), start_index}};
    }

    [[nodiscard]] constexpr bool contains_at(const std::size_t i) const noexcept
    {
#ifdef USE_BIT_SET_FOR_ENUM_SET
        return IMPLEMENTATION_DETAIL_DO_NOT_USE_array_set_.contains(i);
#else
        return array_set_unchecked_at(i);
#endif
    }

    constexpr void reset_at(const std::size_t i) noexcept
    {
#ifdef USE_BIT_SET_FOR_ENUM_SET
        IMPLEMENTATION_DETAIL_DO_NOT_USE_array_set_.erase(i);
#else
        assert_or_abort(contains_at(i));
        array_set_unchecked_at(i) = false;
        decrement_size();
#endif
    }
};

template <typename K>
[[nodiscard]] constexpr typename EnumSet<K>::size_type is_full(const EnumSet<K>& c)
{
    return c.size() >= c.max_size();
}

template <InputIterator InputIt>
EnumSet(InputIt first,
        InputIt last) noexcept -> EnumSet<typename std::iterator_traits<InputIt>::value_type>;

template <class K, class Predicate>
constexpr typename EnumSet<K>::size_type erase_if(EnumSet<K>& c, Predicate predicate)
{
    return erase_if_detail::erase_if_impl(c, predicate);
}

}  // namespace fixed_containers

// Specializations
namespace std
{
template <typename K>
struct tuple_size<fixed_containers::EnumSet<K>> : std::integral_constant<std::size_t, 0>
{
    // Implicit Structured Binding due to the fields being public is disabled
};
}  // namespace std
