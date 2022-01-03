#pragma once

#include "EnumIterator.h"
#include "Utility.h"

#include <iterator>

namespace scrap
{
template<class T, RangeEnum EnumT>
class EnumArrayConstIterator
{
public:
    using iterator_concept = std::contiguous_iterator_tag;
    using iterator_category = std::random_access_iterator_tag;
    using value_type = T;
    using difference_type = ptrdiff_t;
    using pointer = const T*;
    using reference = const T&;

    constexpr EnumArrayConstIterator() noexcept: mPtr() {}

    constexpr explicit EnumArrayConstIterator(pointer ptr, size_t offset = 0) noexcept: mPtr(ptr + offset) {}

    [[nodiscard]] constexpr reference operator*() const noexcept { return *mPtr; }

    [[nodiscard]] constexpr pointer operator->() const noexcept { return mPtr; }

    constexpr EnumArrayConstIterator& operator++() noexcept
    {
        ++mPtr;
        return *this;
    }

    constexpr EnumArrayConstIterator operator++(int) noexcept
    {
        EnumArrayConstIterator temp = *this;
        ++mPtr;
        return temp;
    }

    constexpr EnumArrayConstIterator& operator--() noexcept
    {
        --mPtr;
        return *this;
    }

    constexpr EnumArrayConstIterator operator--(int) noexcept
    {
        EnumArrayConstIterator temp = *this;
        --mPtr;
        return temp;
    }

    constexpr EnumArrayConstIterator& operator+=(const ptrdiff_t offset) noexcept
    {
        mPtr += offset;
        return *this;
    }

    [[nodiscard]] constexpr EnumArrayConstIterator operator+(const ptrdiff_t offset) const noexcept
    {
        EnumArrayConstIterator temp = *this;
        temp += offset;
        return temp;
    }

    constexpr EnumArrayConstIterator& operator-=(const ptrdiff_t offset) noexcept
    {
        mPtr -= offset;
        return *this;
    }

    [[nodiscard]] constexpr EnumArrayConstIterator operator-(const ptrdiff_t offset) const noexcept
    {
        EnumArrayConstIterator temp = *this;
        temp -= offset;
        return temp;
    }

    [[nodiscard]] constexpr ptrdiff_t operator-(const EnumArrayConstIterator& right) const noexcept
    {
        return mPtr - right.mPtr;
    }

    [[nodiscard]] constexpr reference operator[](const ptrdiff_t offset) const noexcept { return mPtr[offset]; }

    [[nodiscard]] constexpr bool operator==(const EnumArrayConstIterator& right) const noexcept
    {
        return mPtr == right.mPtr;
    }

    [[nodiscard]] constexpr std::strong_ordering operator<=>(const EnumArrayConstIterator& right) const noexcept
    {
        return mPtr <=> right.mPtr;
    }

    using _Prevent_inheriting_unwrap = EnumArrayConstIterator;

    [[nodiscard]] constexpr pointer _Unwrapped() const noexcept { return mPtr; }

    static constexpr bool _Unwrap_when_unverified = true;

    constexpr void _Seek_to(pointer _It) noexcept { mPtr = _It; }

private:
    pointer mPtr; // beginning of array
};

template<class T, RangeEnum EnumT>
class EnumArrayIterator : public EnumArrayConstIterator<T, EnumT>
{
public:
    using base = EnumArrayConstIterator<T, EnumT>;

    using iterator_concept = std::contiguous_iterator_tag;
    using iterator_category = std::random_access_iterator_tag;
    using value_type = T;
    using difference_type = ptrdiff_t;
    using pointer = T*;
    using reference = T&;

    constexpr EnumArrayIterator() noexcept {}

    constexpr explicit EnumArrayIterator(pointer ptr, size_t offset = 0) noexcept: base(ptr, offset) {}

    [[nodiscard]] constexpr reference operator*() const noexcept { return const_cast<reference>(base::operator*()); }

    [[nodiscard]] constexpr pointer operator->() const noexcept { return const_cast<pointer>(base::operator->()); }

    constexpr EnumArrayIterator& operator++() noexcept
    {
        base::operator++();
        return *this;
    }

    constexpr EnumArrayIterator operator++(int) noexcept
    {
        EnumArrayIterator temp = *this;
        base::operator++();
        return temp;
    }

    constexpr EnumArrayIterator& operator--() noexcept
    {
        base::operator--();
        return *this;
    }

    constexpr EnumArrayIterator operator--(int) noexcept
    {
        EnumArrayIterator temp = *this;
        base::operator--();
        return temp;
    }

    constexpr EnumArrayIterator& operator+=(const ptrdiff_t offset) noexcept
    {
        base::operator+=(offset);
        return *this;
    }

    [[nodiscard]] constexpr EnumArrayIterator operator+(const ptrdiff_t offset) const noexcept
    {
        EnumArrayIterator temp = *this;
        temp += offset;
        return temp;
    }

    constexpr EnumArrayIterator& operator-=(const ptrdiff_t offset) noexcept
    {
        base::operator-=(offset);
        return *this;
    }

    using base::operator-;

    [[nodiscard]] constexpr EnumArrayIterator operator-(const ptrdiff_t offset) const noexcept
    {
        EnumArrayIterator temp = *this;
        temp -= offset;
        return temp;
    }

    [[nodiscard]] constexpr reference operator[](const ptrdiff_t offset) const noexcept
    {
        return const_cast<reference>(base::operator[](offset));
    }

    using _Prevent_inheriting_unwrap = EnumArrayIterator;

    [[nodiscard]] constexpr pointer _Unwrapped() const noexcept { return const_cast<pointer>(base::_Unwrapped()); }
};

template<class T, RangeEnum EnumT>
[[nodiscard]] constexpr EnumArrayIterator<T, EnumT> operator+(const ptrdiff_t offset,
                                                              EnumArrayIterator<T, EnumT> next) noexcept
{
    next += offset;
    return next;
}

template<class T, RangeEnum EnumT>
class EnumArray
{
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using iterator = EnumArrayIterator<T, EnumT>;
    using const_iterator = EnumArrayConstIterator<T, EnumT>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    [[nodiscard]] constexpr reference operator[](EnumT index) noexcept { return mElems[ToUnderlying(index)]; }
    [[nodiscard]] constexpr const_reference operator[](EnumT index) const noexcept
    {
        return mElems[ToUnderlying(index)];
    }

    [[nodiscard]] constexpr reference at(EnumT index) { return mElems[ToUnderlying(index)]; }
    [[nodiscard]] constexpr const_reference at(EnumT index) const { return mElems[ToUnderlying(index)]; }

    [[nodiscard]] constexpr reference front() { return mElems[0]; }
    [[nodiscard]] constexpr const_reference front() const { return mElems[0]; }

    [[nodiscard]] constexpr reference back() { return mElems[ToUnderlying(EnumT::Count) - 1]; }
    [[nodiscard]] constexpr const_reference back() const { return mElems[ToUnderlying(EnumT::Count) - 1]; }

    [[nodiscard]] constexpr pointer data() noexcept { return mElems; }
    [[nodiscard]] constexpr const_pointer data() const noexcept { return mElems; }

    [[nodiscard]] constexpr iterator begin() noexcept { return EnumArrayIterator<T, EnumT>(mElems, 0); }
    [[nodiscard]] constexpr const_iterator begin() const noexcept
    {
        return EnumArrayConstIterator<T, EnumT>(mElems, 0);
    }
    [[nodiscard]] constexpr const_iterator cbegin() const noexcept
    {
        return EnumArrayConstIterator<T, EnumT>(mElems, 0);
    }

    [[nodiscard]] constexpr iterator end() noexcept
    {
        return EnumArrayIterator<T, EnumT>(mElems, ToUnderlying(EnumT::Count));
    }
    [[nodiscard]] constexpr const_iterator end() const noexcept
    {
        return EnumArrayConstIterator<T, EnumT>(mElems, ToUnderlying(EnumT::Count));
    }
    [[nodiscard]] constexpr const_iterator cend() const noexcept
    {
        return EnumArrayConstIterator<T, EnumT>(mElems, ToUnderlying(EnumT::Count));
    }

    [[nodiscard]] constexpr iterator rbegin() noexcept
    {
        return std::make_reverse_iterator(EnumArrayIterator<T, EnumT>(mElems, 0));
    }
    [[nodiscard]] constexpr const_iterator rbegin() const noexcept
    {
        return std::make_reverse_iterator(EnumArrayConstIterator<T, EnumT>(mElems, 0));
    }
    [[nodiscard]] constexpr const_iterator crbegin() const noexcept
    {
        return std::make_reverse_iterator(EnumArrayConstIterator<T, EnumT>(mElems, 0));
    }

    [[nodiscard]] constexpr iterator rend() noexcept
    {
        return std::make_reverse_iterator(EnumArrayIterator<T, EnumT>(mElems, ToUnderlying(EnumT::Count)));
    }
    [[nodiscard]] constexpr const_iterator rend() const noexcept
    {
        return std::make_reverse_iterator(EnumArrayConstIterator<T, EnumT>(mElems, ToUnderlying(EnumT::Count)));
    }
    [[nodiscard]] constexpr const_iterator crend() const noexcept
    {
        return std::make_reverse_iterator(EnumArrayConstIterator<T, EnumT>(mElems, ToUnderlying(EnumT::Count)));
    }

    [[nodiscard]] constexpr bool empty() const noexcept { return ToUnderlying(EnumT::Count) == 0; }

    [[nodiscard]] constexpr size_type size() const noexcept { return ToUnderlying(EnumT::Count); }
    [[nodiscard]] constexpr size_type max_size() const noexcept { return ToUnderlying(EnumT::Count); }

    constexpr void fill(const_reference value) noexcept
    {
        std::fill(mElems[0], mElems[ToUnderlying(EnumT::Count) - 1], value);
    }

    constexpr void swap(EnumArray<T, EnumT>& other) noexcept(std::is_nothrow_swappable_v<T>)
    {
        std::swap(mElems, other.mElems);
    }

    T mElems[ToUnderlying(EnumT::Count)];
};

namespace detail
{
template<class T>
concept BooleanTestableImpl = std::convertible_to<T, bool>;

template<class T>
concept BooleanTestable = BooleanTestableImpl<T> && requires(T&& t)
{
    {
        !static_cast<T&&>(t)
        } -> BooleanTestableImpl;
};

struct SynthThreeWay
{
    // clang-format off
    template<class T1, class T2>
    [[nodiscard]] constexpr auto operator()(const T1& left, const T2& right) const
        requires requires {
            { left < right } -> BooleanTestable;
            { right < left } -> BooleanTestable;
        }
    // clang-format on
    {
        if constexpr(std::three_way_comparable_with<T1, T2>) { return left <=> right; }
        else
        {
            if(left < right) { return std::weak_ordering::less; }
            else if(right < left)
            {
                return std::weak_ordering::greater;
            }
            else
            {
                return std::weak_ordering::equivalent;
            }
        }
    }
};

template<class T1, class T2 = T1>
using SynthThreeWayResult = decltype(SynthThreeWay{}(std::declval<T1&>(), std::declval<T2&>()));
} // namespace detail

template<class T, RangeEnum EnumT>
[[nodiscard]] constexpr detail::SynthThreeWayResult<T> operator<=>(const EnumArray<T, EnumT>& left,
                                                                   const EnumArray<T, EnumT>& right)
{
    return std::lexicographical_compare_three_way(left.begin(), left.end(), right.begin(), right.end(),
                                                  detail::SynthThreeWay{});
}

namespace detail
{
template<class T, RangeEnum EnumT>
class EnumArrayConstEnumeratorIterator
{
public:
    using iterator_concept = std::random_access_iterator_tag;
    using iterator_category = std::random_access_iterator_tag;
    using value_type = std::pair<EnumT, const T&>;
    using difference_type = ptrdiff_t;

    constexpr EnumArrayConstEnumeratorIterator(const EnumArray<T, EnumT>& enumArray, EnumT index) noexcept
        : mEnumArray(&enumArray)
        , mIndex(index)
    {}

    [[nodiscard]] constexpr value_type operator*() const noexcept
    {
        return std::pair<EnumT, const T&>(mIndex, (*mEnumArray)[mIndex]);
    }

    constexpr EnumArrayConstEnumeratorIterator& operator++() noexcept
    {
        mIndex = IncrementEnum(mIndex);
        return *this;
    }

    constexpr EnumArrayConstEnumeratorIterator operator++(int) noexcept
    {
        EnumArrayConstEnumeratorIterator temp = *this;
        mIndex = IncrementEnum(mIndex);
        return temp;
    }

    constexpr EnumArrayConstEnumeratorIterator& operator--() noexcept
    {
        mIndex = DecrementEnum(mIndex);
        return *this;
    }

    constexpr EnumArrayConstEnumeratorIterator operator--(int) noexcept
    {
        EnumArrayConstEnumeratorIterator temp = *this;
        mIndex = DecrementEnum(mIndex);
        return temp;
    }

    constexpr EnumArrayConstEnumeratorIterator& operator+=(const ptrdiff_t offset) noexcept
    {
        mIndex = IncrementEnum(mIndex, offset);
        return *this;
    }

    [[nodiscard]] constexpr EnumArrayConstEnumeratorIterator operator+(const ptrdiff_t offset) const noexcept
    {
        EnumArrayConstEnumeratorIterator temp = *this;
        temp += offset;
        return temp;
    }

    constexpr EnumArrayConstEnumeratorIterator& operator-=(const ptrdiff_t offset) noexcept
    {
        mIndex = DecrementEnum(mIndex, offset);
        return *this;
    }

    [[nodiscard]] constexpr EnumArrayConstEnumeratorIterator operator-(const ptrdiff_t offset) const noexcept
    {
        EnumArrayConstEnumeratorIterator temp = *this;
        temp -= offset;
        return temp;
    }

    [[nodiscard]] constexpr ptrdiff_t operator-(const EnumArrayConstEnumeratorIterator& right) const noexcept
    {
        return ToUnderlying(mIndex) - ToUnderlying(right.mIndex);
    }

    [[nodiscard]] constexpr bool operator==(const EnumArrayConstEnumeratorIterator& right) const noexcept
    {
        return mEnumArray == right.mEnumArray && mIndex == right.mIndex;
    }

    [[nodiscard]] constexpr std::strong_ordering
    operator<=>(const EnumArrayConstEnumeratorIterator& right) const noexcept
    {
        if(mEnumArray == right.mEnumArray) { return mIndex <=> right.mIndex; }

        return mEnumArray <=> right.mEnumArray;
    }

private:
    const EnumArray<T, EnumT>* mEnumArray;
    EnumT mIndex = EnumT::First;
};

template<class T, RangeEnum EnumT>
class EnumArrayEnumeratorIterator : public EnumArrayConstEnumeratorIterator<T, EnumT>
{
public:
    using base = EnumArrayConstEnumeratorIterator<T, EnumT>;

    using iterator_concept = std::random_access_iterator_tag;
    using iterator_category = std::random_access_iterator_tag;
    using value_type = std::pair<EnumT, T&>;
    using difference_type = ptrdiff_t;

    constexpr EnumArrayEnumeratorIterator() noexcept {}

    constexpr explicit EnumArrayEnumeratorIterator(EnumArray<T, EnumT>& enumArray, EnumT index) noexcept
        : base(enumArray, index)
    {}

    [[nodiscard]] constexpr value_type operator*() const noexcept
    {
        std::pair<EnumT, const T&> constPair = base::operator*();
        return std::pair<EnumT, T&>(constPair.first, const_cast<T&>(constPair.second));
    }

    constexpr EnumArrayEnumeratorIterator& operator++() noexcept
    {
        base::operator++();
        return *this;
    }

    constexpr EnumArrayEnumeratorIterator operator++(int) noexcept
    {
        EnumArrayEnumeratorIterator temp = *this;
        base::operator++();
        return temp;
    }

    constexpr EnumArrayEnumeratorIterator& operator--() noexcept
    {
        base::operator--();
        return *this;
    }

    constexpr EnumArrayEnumeratorIterator operator--(int) noexcept
    {
        EnumArrayEnumeratorIterator temp = *this;
        base::operator--();
        return temp;
    }

    constexpr EnumArrayEnumeratorIterator& operator+=(const ptrdiff_t offset) noexcept
    {
        base::operator+=(offset);
        return *this;
    }

    [[nodiscard]] constexpr EnumArrayEnumeratorIterator operator+(const ptrdiff_t offset) const noexcept
    {
        EnumArrayEnumeratorIterator temp = *this;
        temp += offset;
        return temp;
    }

    constexpr EnumArrayEnumeratorIterator& operator-=(const ptrdiff_t offset) noexcept
    {
        base::operator-=(offset);
        return *this;
    }

    using base::operator-;

    [[nodiscard]] constexpr EnumArrayEnumeratorIterator operator-(const ptrdiff_t offset) const noexcept
    {
        EnumArrayEnumeratorIterator temp = *this;
        temp -= offset;
        return temp;
    }
};

template<class T, RangeEnum EnumT>
class EnumArrayEnumerator
{
public:
    explicit EnumArrayEnumerator(EnumArray<T, EnumT>& enumArray): mEnumArray(&enumArray) {}

    [[nodiscard]] constexpr EnumArrayEnumeratorIterator<T, EnumT> begin() const noexcept
    {
        return EnumArrayEnumeratorIterator<T, EnumT>(*mEnumArray, EnumT::First);
    }
    [[nodiscard]] constexpr EnumArrayEnumeratorIterator<T, EnumT> end() const noexcept
    {
        return EnumArrayEnumeratorIterator<T, EnumT>(*mEnumArray, static_cast<EnumT>(ToUnderlying(EnumT::Last) + 1));
    }

private:
    EnumArray<T, EnumT>* mEnumArray = nullptr;
};

template<class T, RangeEnum EnumT>
class EnumArrayConstEnumerator
{
public:
    explicit EnumArrayConstEnumerator(const EnumArray<T, EnumT>& enumArray): mEnumArray(&enumArray) {}

    [[nodiscard]] constexpr EnumArrayEnumeratorIterator<T, EnumT> begin() const noexcept
    {
        return EnumArrayEnumeratorIterator<T, EnumT>(*mEnumArray, EnumT::First);
    }
    [[nodiscard]] constexpr EnumArrayEnumeratorIterator<T, EnumT> end() const noexcept
    {
        return EnumArrayEnumeratorIterator<T, EnumT>(*mEnumArray, static_cast<EnumT>(ToUnderlying(EnumT::Last) + 1));
    }

private:
    const EnumArray<T, EnumT>* mEnumArray = nullptr;
};
} // namespace detail

template<class T, RangeEnum EnumT>
constexpr [[nodiscard]] detail::EnumArrayEnumerator<T, EnumT> enumerate(EnumArray<T, EnumT>& enumArray) noexcept
{
    return detail::EnumArrayEnumerator(enumArray);
}

template<class T, RangeEnum EnumT>
constexpr [[nodiscard]] detail::EnumArrayConstEnumerator<T, EnumT>
enumerate(const EnumArray<T, EnumT>& enumArray) noexcept
{
    return detail::EnumArrayConstEnumerator(enumArray);
}

} // namespace scrap

namespace std
{
template<class T, scrap::RangeEnum EnumT>
struct pointer_traits<scrap::EnumArrayConstIterator<T, EnumT>>
{
    using pointer = scrap::EnumArrayConstIterator<T, EnumT>;
    using element_type = const T;
    using difference_type = ptrdiff_t;

    [[nodiscard]] static constexpr element_type* to_address(const pointer iter) noexcept { return iter._Unwrapped(); }
};

template<class T, scrap::RangeEnum EnumT>
struct pointer_traits<scrap::EnumArrayIterator<T, EnumT>>
{
    using pointer = scrap::EnumArrayIterator<T, EnumT>;
    using element_type = T;
    using difference_type = ptrdiff_t;

    [[nodiscard]] static constexpr element_type* to_address(const pointer iter) noexcept { return iter._Unwrapped(); }
};
} // namespace std