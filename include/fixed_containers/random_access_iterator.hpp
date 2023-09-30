#pragma once

#include "fixed_containers/arrow_proxy.hpp"
#include "fixed_containers/iterator_utils.hpp"

#include <cstddef>
#include <cstdint>
#include <iterator>

namespace fixed_containers
{
template <class P>
concept RandomAccessEntryProvider = requires(P p, P other, std::size_t unsigned_integer) {
    p.advance(unsigned_integer);
    p.recede(unsigned_integer);
    p.get();
    {
        p == other
    } -> std::same_as<bool>;
    {
        p <=> other
    };
    {
        p - other
    } -> std::same_as<std::ptrdiff_t>;
};

template <RandomAccessEntryProvider ConstEntryProvider,
          RandomAccessEntryProvider MutableEntryProvider,
          IteratorConstness CONSTNESS,
          IteratorDirection DIRECTION>
class RandomAccessIterator
{
    static constexpr IteratorConstness NEGATED_CONSTNESS = IteratorConstness(!bool(CONSTNESS));

    using Self =
        RandomAccessIterator<ConstEntryProvider, MutableEntryProvider, CONSTNESS, DIRECTION>;

    // Sibling has the same parameters, but different const-ness
    using Sibling = RandomAccessIterator<ConstEntryProvider,
                                         MutableEntryProvider,
                                         NEGATED_CONSTNESS,
                                         DIRECTION>;

    // Give Sibling access to private members
    friend class RandomAccessIterator<ConstEntryProvider,
                                      MutableEntryProvider,
                                      NEGATED_CONSTNESS,
                                      DIRECTION>;

    using ReverseBase = RandomAccessIterator<ConstEntryProvider,
                                             MutableEntryProvider,
                                             CONSTNESS,
                                             IteratorDirection(!bool(DIRECTION))>;

    using EntryProvider = std::conditional_t<CONSTNESS == IteratorConstness::CONSTANT_ITERATOR,
                                             ConstEntryProvider,
                                             MutableEntryProvider>;

    using ReturnedType = decltype(std::declval<EntryProvider>().get());
    static constexpr bool SAFE_LIFETIME = std::is_reference_v<ReturnedType>;

public:
    using reference = ReturnedType;
    using value_type = std::remove_cvref_t<reference>;
    using pointer =
        std::conditional_t<SAFE_LIFETIME, std::add_pointer_t<reference>, ArrowProxy<reference>>;
    using iterator = RandomAccessIterator;
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = std::ptrdiff_t;

private:
    EntryProvider reference_provider_;

public:
    constexpr RandomAccessIterator() noexcept
      : RandomAccessIterator{EntryProvider{}}
    {
    }

    template <typename First, typename... Args>
        requires(!std::same_as<Self, std::remove_cvref_t<First>> &&
                 !std::same_as<Sibling, std::remove_cvref_t<First>>)
    explicit constexpr RandomAccessIterator(First&& first, Args&&... args) noexcept
      : reference_provider_(std::forward<First>(first), std::forward<Args>(args)...)
    {
        if constexpr (DIRECTION == IteratorDirection::REVERSE)
        {
            advance();
        }
    }

    // Mutable iterator needs to be convertible to const iterator
    constexpr RandomAccessIterator(const Sibling& other) noexcept
        requires(CONSTNESS == IteratorConstness::CONSTANT_ITERATOR)
      : RandomAccessIterator{other.reference_provider_}
    {
    }

    constexpr reference operator*() const noexcept { return reference_provider_.get(); }

    constexpr pointer operator->() const noexcept
    {
        if constexpr (SAFE_LIFETIME)
        {
            return &reference_provider_.get();
        }
        else
        {
            return {reference_provider_.get()};
        }
    }

    constexpr reference operator[](difference_type n) const
    {
        Self tmp = *this;
        tmp.advance(n);
        return *tmp;
    }

    constexpr Self& operator++() noexcept
    {
        advance();
        return *this;
    }

    constexpr Self operator++(int) & noexcept
    {
        Self tmp = *this;
        advance();
        return tmp;
    }

    constexpr Self& operator--() noexcept
    {
        recede();
        return *this;
    }

    constexpr Self operator--(int) & noexcept
    {
        Self tmp = *this;
        recede();
        return tmp;
    }

    constexpr Self& operator+=(difference_type off)
    {
        advance(off);
        return *this;
    }

    constexpr Self operator+(difference_type off) const
    {
        Self tmp = *this;
        std::advance(tmp, off);
        return tmp;
    }

    friend constexpr Self operator+(difference_type off, const Self& other)
    {
        return Self(std::next(other.iterator_, off));
    }

    constexpr Self& operator-=(difference_type off)
    {
        recede(off);
        return *this;
    }

    constexpr Self operator-(difference_type off) const
    {
        Self tmp = *this;
        std::advance(tmp, -off);
        return tmp;
    }

    constexpr difference_type operator-(const Self& other) const
    {
        if constexpr (DIRECTION == IteratorDirection::FORWARD)
        {
            return reference_provider_ - other.reference_provider_;
        }
        else
        {
            return other.reference_provider_ - reference_provider_;
        }
    }
    constexpr difference_type operator-(const Sibling& other) const
    {
        if constexpr (DIRECTION == IteratorDirection::FORWARD)
        {
            return reference_provider_ - other.reference_provider_;
        }
        else
        {
            return other.reference_provider_ - reference_provider_;
        }
    }

    constexpr std::strong_ordering operator<=>(const Self& other) const
    {
        if constexpr (DIRECTION == IteratorDirection::FORWARD)
        {
            return reference_provider_ <=> other.reference_provider_;
        }
        else
        {
            return other.reference_provider_ <=> reference_provider_;
        }
    }

    constexpr std::strong_ordering operator<=>(const Sibling& other) const
    {
        if constexpr (DIRECTION == IteratorDirection::FORWARD)
        {
            return reference_provider_ <=> other.reference_provider_;
        }
        else
        {
            return other.reference_provider_ <=> reference_provider_;
        }
    }

    constexpr bool operator==(const Self& other) const noexcept
    {
        return this->reference_provider_ == other.reference_provider_;
    }

    constexpr bool operator==(const Sibling& other) const noexcept
    {
        return reference_provider_ == other.reference_provider_;
    }

    constexpr ReverseBase base() const noexcept
        requires(DIRECTION == IteratorDirection::REVERSE)
    {
        ReverseBase out{reference_provider_};
        ++out;
        return out;
    }

private:
    constexpr void advance(difference_type n = 1) noexcept
    {
        if (n < 0)
        {
            recede(-n);
            return;
        }

        if constexpr (DIRECTION == IteratorDirection::FORWARD)
        {
            reference_provider_.advance(static_cast<std::size_t>(n));
        }
        else
        {
            reference_provider_.recede(static_cast<std::size_t>(n));
        }
    }
    constexpr void recede(difference_type n = 1) noexcept
    {
        if (n < 0)
        {
            advance(-n);
            return;
        }

        if constexpr (DIRECTION == IteratorDirection::FORWARD)
        {
            reference_provider_.recede(static_cast<std::size_t>(n));
        }
        else
        {
            reference_provider_.advance(static_cast<std::size_t>(n));
        }
    }
};

}  // namespace fixed_containers
