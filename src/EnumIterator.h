#pragma once

#include "Utility.h"

#include <iterator>

namespace scrap
{
template<class EnumT>
requires std::is_enum_v<EnumT>
class EnumIterator
{
public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = EnumT;
    using difference_type = std::underlying_type_t<EnumT>;

    constexpr EnumIterator() noexcept = default;
    constexpr EnumIterator(EnumT value): mValue(value) {}

    constexpr EnumIterator& operator=(const EnumIterator&) noexcept = default;
    [[nodiscard]] constexpr EnumT operator*() const noexcept { return mValue; }

    constexpr EnumIterator& operator++() noexcept
    {
        mValue = IncrementEnum(mValue);
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
        mValue = DecrementEnum(mValue);
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
        mValue = IncrementEnum(mValue, offset);
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
        return ToUnderlying(mValue) - ToUnderlying(rhs.mValue);
    }

    [[nodiscard]] constexpr bool operator==(const EnumIterator& rhs) const noexcept { return mValue == rhs.mValue; }

    [[nodiscard]] constexpr bool operator!=(const EnumIterator& rhs) const noexcept { return !(*this == rhs); }

    [[nodiscard]] constexpr bool operator<(const EnumIterator& rhs) const noexcept { return mValue < rhs.mValue; }

    [[nodiscard]] constexpr bool operator>(const EnumIterator& rhs) const noexcept { return rhs < *this; }

    [[nodiscard]] constexpr bool operator<=(const EnumIterator& rhs) const noexcept { return !(rhs < *this); }

    [[nodiscard]] constexpr bool operator>=(const EnumIterator& rhs) const noexcept { return !(*this < rhs); }

private:
    EnumT mValue = EnumT::First;
};

template<RangeEnum EnumT, EnumT First = EnumT::First, EnumT Last = EnumT::Last>
class Enumerator
{
public:
    [[nodiscard]] constexpr EnumIterator<EnumT> begin() const noexcept { return EnumIterator<EnumT>(First); }
    [[nodiscard]] constexpr EnumIterator<EnumT> end() const noexcept
    {
        return EnumIterator<EnumT>(static_cast<EnumT>(ToUnderlying(Last) + 1));
    }
};

template<MaskEnum EnumMaskT, class EnumT>
requires std::is_enum_v<EnumT>
[[nodiscard]] bool testEnumInMask(EnumMaskT mask, EnumT value)
{
    const EnumMaskT maskValue = static_cast<EnumMaskT>(1 << ToUnderlying(value));
    return (mask & maskValue) == maskValue;
}

template<MaskEnum EnumMaskT, RangeEnum EnumT>
class EnumMaskIterator
{
public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = EnumT;
    using difference_type = std::underlying_type_t<EnumT>;

    constexpr EnumMaskIterator() noexcept = default;
    constexpr EnumMaskIterator(EnumMaskT mask, EnumT value): mMask(mask), mValue(value)
    {
        while(!testEnumInMask(mMask, mValue) && mValue <= EnumT::Last)
        {
            mValue = IncrementEnum(mValue);
        }
    }

    constexpr EnumMaskIterator& operator=(const EnumMaskIterator&) noexcept = default;
    [[nodiscard]] constexpr EnumT operator*() const noexcept { return mValue; }

    constexpr EnumMaskIterator& operator++() noexcept
    {
        do
        {
            mValue = IncrementEnum(mValue);
        } while(!testEnumInMask(mMask, mValue) && mValue <= EnumT::Last);

        return *this;
    }

    constexpr EnumMaskIterator operator++(int) noexcept
    {
        EnumIterator ret = *this;
        ++(*this);
        return ret;
    }

    constexpr EnumMaskIterator& operator--() noexcept
    {
        do
        {
            mValue = DecrementEnum(mValue);
        } while(!testEnumInMask(mMask, mValue) && mValue > EnumT::First);

        return *this;
    }

    constexpr EnumMaskIterator operator--(int) noexcept
    {
        EnumIterator ret = *this;
        --(*this);
        return ret;
    }

    [[nodiscard]] constexpr difference_type operator-(const EnumMaskIterator& rhs) const noexcept
    {
        return mValue - rhs.mValue;
    }

    [[nodiscard]] constexpr bool operator==(const EnumMaskIterator& rhs) const noexcept
    {
        return mMask == rhs.mMask && mValue == rhs.mValue;
    }

    [[nodiscard]] constexpr bool operator!=(const EnumMaskIterator& rhs) const noexcept { return !(*this == rhs); }

    [[nodiscard]] constexpr bool operator<(const EnumMaskIterator& rhs) const noexcept { return mValue < rhs.mValue; }

    [[nodiscard]] constexpr bool operator>(const EnumMaskIterator& rhs) const noexcept { return rhs < *this; }

    [[nodiscard]] constexpr bool operator<=(const EnumMaskIterator& rhs) const noexcept { return !(rhs < *this); }

    [[nodiscard]] constexpr bool operator>=(const EnumMaskIterator& rhs) const noexcept { return !(*this < rhs); }

private:
    EnumMaskT mMask = EnumMaskT::None;
    EnumT mValue = 0;
};

template<MaskEnum EnumMaskT, RangeEnum EnumT>
class MaskPairEnumerator
{
public:
    explicit MaskPairEnumerator(EnumMaskT mask): mMask(mask) {}

    [[nodiscard]] constexpr EnumMaskIterator<EnumMaskT, EnumT> begin() const noexcept
    {
        return EnumMaskIterator<EnumMaskT, EnumT>(mMask, EnumT::First);
    }
    [[nodiscard]] constexpr EnumMaskIterator<EnumMaskT, EnumT> end() const noexcept
    {
        return EnumMaskIterator<EnumMaskT, EnumT>(mMask, static_cast<EnumT>(ToUnderlying(EnumT::Last) + 1));
    }

private:
    EnumMaskT mMask;
};

} // namespace scrap