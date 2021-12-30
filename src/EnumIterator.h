#pragma once

#include "Utility.h"

#include <iterator>

namespace scrap
{
template<class T>
concept RangeEnum = std::is_enum_v<T> && requires
{
    T::First;
    T::Last;
    T::Count;
        requires(ToUnderlying(T::Last) - ToUnderlying(T::First) + 1) == ToUnderlying(T::Count);
};

template<class EnumT> requires std::is_enum_v<EnumT>
class EnumIterator
{
public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = EnumT;
    using difference_type = std::underlying_type_t<EnumT>;

    constexpr EnumIterator() noexcept = default;
    constexpr EnumIterator(std::underlying_type_t<EnumT> index): mIndex(index) {}

    constexpr EnumIterator& operator=(const EnumIterator&) noexcept = default;
    [[nodiscard]] constexpr EnumT operator*() const noexcept { return static_cast<EnumT>(mIndex); }

    constexpr EnumIterator& operator++() noexcept
    {
        ++mIndex;
        return *this;
    }

    constexpr EnumIterator operator++(int) noexcept
    {
        EnumIterator ret = *this;
        ++(*this);
        return ret;
    }

    constexpr EnumIterator& operator--() noexcept
    {
        --mIndex;
        return *this;
    }

    constexpr EnumIterator operator--(int) noexcept
    {
        EnumIterator ret = *this;
        --(*this);
        return ret;
    }

    constexpr EnumIterator& operator+=(const difference_type offset) noexcept
    {
        mIndex += offset;
        return *this;
    }

    [[nodiscard]] constexpr EnumIterator operator+(const difference_type offset) const noexcept
    {
        EnumIterator ret = *this;
        ret += offset;
        return ret;
    }

    constexpr EnumIterator& operator-=(const difference_type offset) noexcept { return *this += -offset; }

    [[nodiscard]] constexpr EnumIterator operator-(const difference_type offset) const noexcept
    {
        EnumIterator ret = *this;
        ret -= offset;
        return ret;
    }

    [[nodiscard]] constexpr difference_type operator-(const EnumIterator& rhs) const noexcept
    {
        return mIndex - rhs.mIndex;
    }

    [[nodiscard]] constexpr bool operator==(const EnumIterator& rhs) const noexcept { return mIndex == rhs.mIndex; }

    [[nodiscard]] constexpr bool operator!=(const EnumIterator& rhs) const noexcept { return !(*this == rhs); }

    [[nodiscard]] constexpr bool operator<(const EnumIterator& rhs) const noexcept { return mIndex < rhs.mIndex; }

    [[nodiscard]] constexpr bool operator>(const EnumIterator& rhs) const noexcept { return rhs < *this; }

    [[nodiscard]] constexpr bool operator<=(const EnumIterator& rhs) const noexcept { return !(rhs < *this); }

    [[nodiscard]] constexpr bool operator>=(const EnumIterator& rhs) const noexcept { return !(*this < rhs); }

private:
    std::underlying_type_t<EnumT> mIndex = 0;
};

template<RangeEnum EnumT, EnumT First = EnumT::First, EnumT Last = EnumT::Last>
class Enumerator
{
public:
    [[nodiscard]] constexpr EnumIterator<EnumT> begin() const noexcept
    {
        return EnumIterator<EnumT>(ToUnderlying(First));
    }
    [[nodiscard]] constexpr EnumIterator<EnumT> end() const noexcept
    {
        return EnumIterator<EnumT>(ToUnderlying(Last) + 1);
    }
};
} // namespace scrap