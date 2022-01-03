#pragma once

#include "Utility.h"

#include <iterator>

namespace scrap
{
namespace detail
{
template<class EnumT>
requires std::is_enum_v<EnumT>
class RangeEnumEnumeratorIterator
{
public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = EnumT;
    using difference_type = std::underlying_type_t<EnumT>;

    constexpr RangeEnumEnumeratorIterator() noexcept = default;
    constexpr RangeEnumEnumeratorIterator(EnumT value): mValue(value) {}

    constexpr RangeEnumEnumeratorIterator& operator=(const RangeEnumEnumeratorIterator&) noexcept = default;
    [[nodiscard]] constexpr EnumT operator*() const noexcept { return mValue; }

    constexpr RangeEnumEnumeratorIterator& operator++() noexcept
    {
        mValue = IncrementEnum(mValue);
        return *this;
    }

    constexpr RangeEnumEnumeratorIterator operator++(int) noexcept
    {
        RangeEnumEnumeratorIterator ret = *this;
        ++(*this);
        return ret;
    }

    constexpr RangeEnumEnumeratorIterator& operator--() noexcept
    {
        mValue = DecrementEnum(mValue);
        return *this;
    }

    constexpr RangeEnumEnumeratorIterator operator--(int) noexcept
    {
        RangeEnumEnumeratorIterator ret = *this;
        --(*this);
        return ret;
    }

    constexpr RangeEnumEnumeratorIterator& operator+=(const difference_type offset) noexcept
    {
        mValue = IncrementEnum(mValue, offset);
        return *this;
    }

    [[nodiscard]] constexpr RangeEnumEnumeratorIterator operator+(const difference_type offset) const noexcept
    {
        RangeEnumEnumeratorIterator ret = *this;
        ret += offset;
        return ret;
    }

    constexpr RangeEnumEnumeratorIterator& operator-=(const difference_type offset) noexcept
    {
        return *this += -offset;
    }

    [[nodiscard]] constexpr RangeEnumEnumeratorIterator operator-(const difference_type offset) const noexcept
    {
        RangeEnumEnumeratorIterator ret = *this;
        ret -= offset;
        return ret;
    }

    [[nodiscard]] constexpr difference_type operator-(const RangeEnumEnumeratorIterator& rhs) const noexcept
    {
        return ToUnderlying(mValue) - ToUnderlying(rhs.mValue);
    }

    [[nodiscard]] constexpr bool operator==(const RangeEnumEnumeratorIterator& rhs) const noexcept
    {
        return mValue == rhs.mValue;
    }

    [[nodiscard]] constexpr bool operator!=(const RangeEnumEnumeratorIterator& rhs) const noexcept
    {
        return !(*this == rhs);
    }

    [[nodiscard]] constexpr bool operator<(const RangeEnumEnumeratorIterator& rhs) const noexcept
    {
        return mValue < rhs.mValue;
    }

    [[nodiscard]] constexpr bool operator>(const RangeEnumEnumeratorIterator& rhs) const noexcept
    {
        return rhs < *this;
    }

    [[nodiscard]] constexpr bool operator<=(const RangeEnumEnumeratorIterator& rhs) const noexcept
    {
        return !(rhs < *this);
    }

    [[nodiscard]] constexpr bool operator>=(const RangeEnumEnumeratorIterator& rhs) const noexcept
    {
        return !(*this < rhs);
    }

private:
    EnumT mValue = EnumT::First;
};

template<RangeEnum EnumT, EnumT First = EnumT::First, EnumT Last = EnumT::Last>
class RangeEnumEnumerator
{
public:
    [[nodiscard]] constexpr RangeEnumEnumeratorIterator<EnumT> begin() const noexcept
    {
        return RangeEnumEnumeratorIterator<EnumT>(First);
    }
    [[nodiscard]] constexpr RangeEnumEnumeratorIterator<EnumT> end() const noexcept
    {
        return RangeEnumEnumeratorIterator<EnumT>(static_cast<EnumT>(ToUnderlying(Last) + 1));
    }
};
} // namespace detail

template<RangeEnum EnumT>
[[nodiscard]] constexpr detail::RangeEnumEnumerator<EnumT> enumerate() noexcept
{
    return detail::RangeEnumEnumerator<EnumT>();
}

template<MaskEnum EnumMaskT, class EnumT>
requires std::is_enum_v<EnumT>
[[nodiscard]] bool testEnumInMask(EnumMaskT mask, EnumT value)
{
    const EnumMaskT maskValue = static_cast<EnumMaskT>(1 << ToUnderlying(value));
    return (mask & maskValue) == maskValue;
}

namespace detail
{
template<MaskEnum EnumMaskT, RangeEnum EnumT>
class MaskPairEnumeratorIterator
{
public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = EnumT;
    using difference_type = std::underlying_type_t<EnumT>;

    constexpr MaskPairEnumeratorIterator() noexcept = default;
    constexpr MaskPairEnumeratorIterator(EnumMaskT mask, EnumT value): mMask(mask), mValue(value)
    {
        while(!testEnumInMask(mMask, mValue) && mValue <= EnumT::Last)
        {
            mValue = IncrementEnum(mValue);
        }
    }

    constexpr MaskPairEnumeratorIterator& operator=(const MaskPairEnumeratorIterator&) noexcept = default;
    [[nodiscard]] constexpr EnumT operator*() const noexcept { return mValue; }

    constexpr MaskPairEnumeratorIterator& operator++() noexcept
    {
        do
        {
            mValue = IncrementEnum(mValue);
        } while(!testEnumInMask(mMask, mValue) && mValue <= EnumT::Last);

        return *this;
    }

    constexpr MaskPairEnumeratorIterator operator++(int) noexcept
    {
        RangeEnumEnumeratorIterator ret = *this;
        ++(*this);
        return ret;
    }

    constexpr MaskPairEnumeratorIterator& operator--() noexcept
    {
        do
        {
            mValue = DecrementEnum(mValue);
        } while(!testEnumInMask(mMask, mValue) && mValue > EnumT::First);

        return *this;
    }

    constexpr MaskPairEnumeratorIterator operator--(int) noexcept
    {
        RangeEnumEnumeratorIterator ret = *this;
        --(*this);
        return ret;
    }

    [[nodiscard]] constexpr difference_type operator-(const MaskPairEnumeratorIterator& rhs) const noexcept
    {
        return mValue - rhs.mValue;
    }

    [[nodiscard]] constexpr bool operator==(const MaskPairEnumeratorIterator& rhs) const noexcept
    {
        return mMask == rhs.mMask && mValue == rhs.mValue;
    }

    [[nodiscard]] constexpr bool operator!=(const MaskPairEnumeratorIterator& rhs) const noexcept
    {
        return !(*this == rhs);
    }

    [[nodiscard]] constexpr bool operator<(const MaskPairEnumeratorIterator& rhs) const noexcept
    {
        return mValue < rhs.mValue;
    }

    [[nodiscard]] constexpr bool operator>(const MaskPairEnumeratorIterator& rhs) const noexcept { return rhs < *this; }

    [[nodiscard]] constexpr bool operator<=(const MaskPairEnumeratorIterator& rhs) const noexcept
    {
        return !(rhs < *this);
    }

    [[nodiscard]] constexpr bool operator>=(const MaskPairEnumeratorIterator& rhs) const noexcept
    {
        return !(*this < rhs);
    }

private:
    EnumMaskT mMask = EnumMaskT::None;
    EnumT mValue = 0;
};

template<MaskEnum EnumMaskT, RangeEnum EnumT>
class MaskPairEnumerator
{
public:
    constexpr explicit MaskPairEnumerator(EnumMaskT mask): mMask(mask) {}

    [[nodiscard]] constexpr MaskPairEnumeratorIterator<EnumMaskT, EnumT> begin() const noexcept
    {
        return MaskPairEnumeratorIterator<EnumMaskT, EnumT>(mMask, EnumT::First);
    }
    [[nodiscard]] constexpr MaskPairEnumeratorIterator<EnumMaskT, EnumT> end() const noexcept
    {
        return MaskPairEnumeratorIterator<EnumMaskT, EnumT>(mMask, static_cast<EnumT>(ToUnderlying(EnumT::Last) + 1));
    }

private:
    EnumMaskT mMask;
};
} // namespace detail

template<MaskEnum EnumMaskT, RangeEnum EnumT>
[[nodiscard]] constexpr detail::MaskPairEnumerator<EnumMaskT, EnumT> enumerate(EnumMaskT mask) noexcept
{
    return detail::MaskPairEnumerator<EnumMaskT, EnumT>(mask);
}
} // namespace scrap