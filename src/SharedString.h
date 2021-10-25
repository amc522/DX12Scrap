#pragma once

#include "StringHash.h"

#include <algorithm>
#include <atomic>
#include <iterator>
#include <string>
#include <string_view>

namespace scrap
{
template<class CharT, class TraitsT>
class BasicSharedString;

namespace detail
{
template<class CharT, class TraitsT>
class BasicSharedStringIterator
{
public:
#ifdef _MSC_VER
    // Tell Microsoft standard library that BasicSharedStringIterator's are checked.
    using _Unchecked_type = typename CharT*;
#endif

    using iterator_category = std::random_access_iterator_tag;
    using value_type = std::remove_cv_t<CharT>;
    using difference_type = ptrdiff_t;

    using reference = const value_type&;
    using pointer = std::add_pointer_t<reference>;

    BasicSharedStringIterator() = default;

    constexpr BasicSharedStringIterator(const BasicSharedString<CharT, TraitsT>* shared_string, size_t idx) noexcept
        : mSharedString(shared_string)
        , mIndex(idx)
    {}

    constexpr BasicSharedStringIterator(const BasicSharedStringIterator<CharT, TraitsT>& other) noexcept
        : BasicSharedStringIterator(other.mSharedString, other.mIndex)
    {}

    constexpr reference operator*() const
    {
        // Expects(mIndex != mSharedString->size());
        return *(mSharedString->data() + mIndex);
    }

    constexpr pointer operator->() const
    {
        // Expects(mIndex != mSharedString->size());
        return mSharedString->data() + mIndex;
    }

    constexpr BasicSharedStringIterator<CharT, TraitsT>& operator++()
    {
        // Expects(0 <= mIndex && mIndex != mSharedString->size());
        ++mIndex;
        return *this;
    }

    constexpr BasicSharedStringIterator<CharT, TraitsT> operator++(int)
    {
        auto ret = *this;
        ++(*this);
        return ret;
    }

    constexpr BasicSharedStringIterator<CharT, TraitsT>& operator--()
    {
        // Expects(mIndex != 0 && mIndex <= mSharedString->size());
        --mIndex;
        return *this;
    }

    constexpr BasicSharedStringIterator<CharT, TraitsT> operator--(int)
    {
        auto ret = *this;
        --(*this);
        return ret;
    }

    constexpr BasicSharedStringIterator<CharT, TraitsT> operator+(difference_type n) const
    {
        auto ret = *this;
        return ret += n;
    }

    constexpr BasicSharedStringIterator<CharT, TraitsT>& operator+=(difference_type n)
    {
        // Expects((mIndex + n) >= 0 && (mIndex + n) <= mSharedString->size());
        mIndex += n;
        return *this;
    }

    constexpr BasicSharedStringIterator<CharT, TraitsT> operator-(difference_type n) const
    {
        auto ret = *this;
        return ret -= n;
    }

    constexpr BasicSharedStringIterator<CharT, TraitsT>& operator-=(difference_type n) { return *this += -n; }

    constexpr difference_type operator-(BasicSharedStringIterator<CharT, TraitsT> rhs) const
    {
        // Expects(mSharedString == rhs.mSharedString);
        return mIndex - rhs.mIndex;
    }

    constexpr reference operator[](difference_type n) const { return *(*this + n); }

    constexpr friend bool operator==(BasicSharedStringIterator<CharT, TraitsT> lhs,
                                     BasicSharedStringIterator<CharT, TraitsT> rhs) noexcept
    {
        return lhs.mSharedString == rhs.mSharedString && lhs.mIndex == rhs.mIndex;
    }

    constexpr friend bool operator!=(BasicSharedStringIterator<CharT, TraitsT> lhs,
                                     BasicSharedStringIterator<CharT, TraitsT> rhs) noexcept
    {
        return !(lhs == rhs);
    }

    constexpr friend bool operator<(BasicSharedStringIterator<CharT, TraitsT> lhs,
                                    BasicSharedStringIterator<CharT, TraitsT> rhs) noexcept
    {
        return lhs.mIndex < rhs.mIndex;
    }

    constexpr friend bool operator<=(BasicSharedStringIterator<CharT, TraitsT> lhs,
                                     BasicSharedStringIterator<CharT, TraitsT> rhs) noexcept
    {
        return !(rhs < lhs);
    }

    constexpr friend bool operator>(BasicSharedStringIterator<CharT, TraitsT> lhs,
                                    BasicSharedStringIterator<CharT, TraitsT> rhs) noexcept
    {
        return rhs < lhs;
    }

    constexpr friend bool operator>=(BasicSharedStringIterator<CharT, TraitsT> lhs,
                                     BasicSharedStringIterator<CharT, TraitsT> rhs) noexcept
    {
        return !(rhs > lhs);
    }

private:
    const BasicSharedString<CharT, TraitsT>* mSharedString = nullptr;
    size_t mIndex = 0;
};
} // namespace detail

template<class CharT, class TraitsT>
constexpr detail::BasicSharedStringIterator<CharT, TraitsT>
operator+(typename detail::BasicSharedStringIterator<CharT, TraitsT>::difference_type n,
          detail::BasicSharedStringIterator<CharT, TraitsT> rhs)
{
    return rhs + n;
}

template<class CharT, class TraitsT>
constexpr detail::BasicSharedStringIterator<CharT, TraitsT>
operator-(typename detail::BasicSharedStringIterator<CharT, TraitsT>::difference_type n,
          detail::BasicSharedStringIterator<CharT, TraitsT> rhs)
{
    return rhs - n;
}

template<class CharT, class TraitsT = std::char_traits<CharT>>
class BasicSharedString
{
private:
    using RefCounterValueType = int;
    using RefCounterType = std::atomic<RefCounterValueType>;

public:
    static const size_t npos = size_t(-1);

    using iterator = detail::BasicSharedStringIterator<CharT, TraitsT>;
    using const_iterator = detail::BasicSharedStringIterator<CharT, TraitsT>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using value_type = CharT;

public:
    BasicSharedString() = default;

    explicit constexpr BasicSharedString(const CharT* str) noexcept: BasicSharedString(str, TraitsT::length(str)) {}

    constexpr BasicSharedString(const CharT* str, size_t length) noexcept: mLength(length)
    {
        if(str == nullptr)
        {
            length = 0;
            return;
        }

        mData = new uint8_t[(length + 1) * sizeof(CharT) + sizeof(Header)];
        new(mData) Header(str, length);
        CharT* charBuffer = new(mData + sizeof(Header)) CharT[length + 1];

        if(length == 0) { charBuffer[0] = CharT(0); }
        else
        {
            std::memcpy(charBuffer, str, sizeof(CharT) * length);

            if(!TraitsT::eq(str[length - 1], CharT(0))) { charBuffer[length] = CharT(0); }
        }
    }

    template<size_t N>
    explicit constexpr BasicSharedString(const CharT (&str)[N]) noexcept: BasicSharedString(str, N - 1)
    {}

    explicit constexpr BasicSharedString(std::string_view str) noexcept: BasicSharedString(str.data(), str.size()) {}

    explicit constexpr BasicSharedString(const std::basic_string<CharT, TraitsT>& str) noexcept
        : BasicSharedString(str.data(), str.length())
    {}

    constexpr BasicSharedString(const BasicSharedString<CharT, TraitsT>& other) noexcept
    {
        if(other.mData == nullptr) { return; }

        other.addRef();

        mData = other.mData;
        mLength = other.mLength;
    }

    constexpr BasicSharedString(BasicSharedString<CharT, TraitsT>&& other) noexcept
    {
        std::swap(mData, other.mData);
        std::swap(mLength, other.mLength);
    }

    template<class InputIterator>
    BasicSharedString(InputIterator first, InputIterator last)
    {
        mLength = std::distance(first, last);

        if(mLength == 0) { return; }

        mLength = (TraitsT::eq(*(first + (mLength - 1u)), CharT(0))) ? mLength - 1u : mLength;

        mData = new uint8_t[(mLength + 1) * sizeof(CharT) + sizeof(Header)];
        CharT* charBuffer = new(mData + sizeof(Header)) CharT[mLength + 1];

        {
            std::copy_n(first, mLength, charBuffer);

            charBuffer[mLength] = CharT(0);
        }

        new(mData) Header(charBuffer, mLength);
    }

    ~BasicSharedString() noexcept { destroy(); }

    BasicSharedString<CharT, TraitsT>& operator=(const BasicSharedString<CharT, TraitsT>& other) noexcept
    {
        if(mData != nullptr) { destroy(); }

        if(other.mData == nullptr) { return *this; }

        other.addRef();

        mData = other.mData;
        mLength = other.mLength;

        return *this;
    }

    BasicSharedString<CharT, TraitsT>& operator=(BasicSharedString<CharT, TraitsT>&& other) noexcept
    {
        if(mData != nullptr) { destroy(); }

        if(other.mData == nullptr) { return *this; }

        std::swap(mData, other.mData);
        std::swap(mLength, other.mLength);

        return *this;
    }

    operator BasicStringHash<CharT>() const noexcept
    {
        if(mData == nullptr) { return BasicStringHash<CharT>{}; }

        return getHash();
    }

    operator std::basic_string_view<CharT, TraitsT>() const noexcept
    {
        if(mData == nullptr) { return std::basic_string_view<CharT, TraitsT>(); }

        return std::basic_string_view<CharT, TraitsT>(getStringBuffer(), mLength);
    }

    explicit operator std::basic_string<CharT, TraitsT>() const noexcept
    {
        if(mData == nullptr) { return std::basic_string<CharT, TraitsT>(); }

        return std::basic_string<CharT, TraitsT>(getStringBuffer(), mLength);
    }

    friend bool operator==(const BasicSharedString<CharT, TraitsT>& left, const CharT* right)
    {
        size_t index = 0;
        size_t leftSize = left.size();

        while(index < leftSize && right[index] != '\0')
        {
            if(!TraitsT::eq(left[index], right[index])) { return false; }

            ++index;
        }

        if(index < leftSize) { return false; }

        if(right[index] != '\0') { return false; }

        return true;
    }

    friend bool operator!=(const BasicSharedString<CharT, TraitsT>& left, const CharT* right)
    {
        return !operator==(left, right);
    }

    friend bool operator==(const BasicSharedString<CharT, TraitsT>& left,
                           const std::basic_string_view<CharT, TraitsT>& right)
    {
        if(left.size() != right.size()) { return false; }

        auto leftItr = left.cbegin();
        auto rightItr = right.cbegin();
        for(; leftItr != left.cend(); ++leftItr, ++rightItr)
        {
            if(!TraitsT::eq(*leftItr, *rightItr)) { return false; }
        }

        return true;
    }

    friend bool operator!=(const BasicSharedString<CharT, TraitsT>& left,
                           const std::basic_string_view<CharT, TraitsT>& right)
    {
        return !(operator==(left, right));
    }

    friend bool operator==(const BasicSharedString<CharT, TraitsT>& left,
                           const std::basic_string<CharT, TraitsT>& right)
    {
        if(left.size() != right.size()) { return false; }

        auto leftItr = left.cbegin();
        auto rightItr = right.cbegin();
        for(; leftItr != left.cend(); ++leftItr, ++rightItr)
        {
            if(!TraitsT::eq(*leftItr, *rightItr)) { return false; }
        }

        return true;
    }

    friend bool operator!=(const BasicSharedString<CharT, TraitsT>& left,
                           const std::basic_string<CharT, TraitsT>& right)
    {
        return !operator==(left, right);
    }

    friend bool operator==(const BasicSharedString<CharT, TraitsT>& left,
                           const BasicSharedString<CharT, TraitsT>& right)
    {
        return left.hash() == right.hash();
    }

    friend bool operator!=(const BasicSharedString<CharT, TraitsT>& left,
                           const BasicSharedString<CharT, TraitsT>& right)
    {
        return !operator==(left, right);
    }

    const CharT at(size_t index) const
    {
        if(index >= mLength)
        {
            throw std::out_of_range("BasicSharedString::at() called with an index that is out of bounds");
        }

        return getStringBuffer()[index];
    }

    const CharT operator[](size_t index) const noexcept { return getStringBuffer()[index]; }

    const CharT* data() const noexcept { return getStringBuffer(); }

    const CharT* c_str() const noexcept { return getStringBuffer(); }

    constexpr iterator begin() const noexcept { return {this, 0}; }

    constexpr iterator end() const noexcept { return {this, this->size()}; }

    constexpr const_iterator cbegin() const noexcept { return {this, 0}; }

    constexpr const_iterator cend() const noexcept { return {this, size()}; }

    constexpr reverse_iterator rbegin() const noexcept { return reverse_iterator{end()}; }

    constexpr reverse_iterator rend() const noexcept { return reverse_iterator{begin()}; }

    constexpr const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator{cend()}; }

    constexpr const_reverse_iterator crend() const noexcept { return const_reverse_iterator{cbegin()}; }

    bool empty() const noexcept { return mData == nullptr || mLength == 0; }

    size_t size() const noexcept { return mLength; }

    size_t length() const noexcept { return mLength; }

    BasicStringHash<CharT> hash() const noexcept { return (mData != nullptr) ? getHash() : BasicStringHash<CharT>{}; }

    template<class TraitsU>
    size_t find(std::basic_string_view<CharT, TraitsU> str, size_t pos = 0) const
    {
        std::basic_string_view<CharT, TraitsU> sharedStrView{getStringBuffer(), mLength};
        return sharedStrView.find(str, pos);
    }

    size_t find(std::basic_string<CharT, TraitsT> str, size_t pos = 0) const
    {
        std::basic_string_view<CharT, TraitsT> sharedStrView{getStringBuffer(), mLength};
        return sharedStrView.find(str, pos);
    }

    size_t find(const CharT* s, size_t pos, size_t count) const
    {
        std::basic_string_view<CharT, TraitsT> sharedStrView{getStringBuffer(), mLength};
        return sharedStrView.find(s, pos, count);
    }

    size_t find(const CharT* s, size_t pos = 0) const
    {
        std::basic_string_view<CharT, TraitsT> sharedStrView{getStringBuffer(), mLength};
        return sharedStrView.find(s, pos);
    }

    size_t find(CharT ch, size_t pos = 0) const
    {
        std::basic_string_view<CharT, TraitsT> sharedStrView{getStringBuffer(), mLength};
        return sharedStrView.find(ch, pos);
    }

    template<class CharU>
    size_t find(const CharU& ch, size_t pos = 0) const
    {
        auto findItr =
            std::find_if(cbegin() + pos, cend(), [&ch](const CharT& val) { return TraitsT::eq_int_type(ch, val); });

        if(findItr == cend()) { return std::basic_string<CharT, TraitsT>::npos; }

        return std::distance(cbegin(), findItr);
    }

    size_t rfind(const std::basic_string<CharT, TraitsT>& str,
                 size_t pos = std::basic_string<CharT, TraitsT>::npos) const
    {
        std::basic_string_view<CharT, TraitsT> sharedStrView{getStringBuffer(), mLength};
        return sharedStrView.rfind(str.data(), pos, str.size());
    }

    size_t rfind(const CharT* s, size_t pos, size_t count) const
    {
        std::basic_string_view<CharT, TraitsT> sharedStrView{getStringBuffer(), mLength};
        return sharedStrView.rfind(s, pos, count);
    }

    size_t rfind(const CharT* s, size_t pos = std::basic_string<CharT, TraitsT>::npos) const
    {
        std::basic_string_view<CharT, TraitsT> sharedStrView{getStringBuffer(), mLength};
        return sharedStrView.rfind(s, pos);
    }

    size_t rfind(CharT ch, size_t pos = std::basic_string<CharT, TraitsT>::npos) const
    {
        std::basic_string_view<CharT, TraitsT> sharedStrView{getStringBuffer(), mLength};
        return sharedStrView.rfind(ch, pos);
    }

    template<class CharU>
    size_t rfind(const CharU& ch, size_t pos = std::basic_string<CharT, TraitsT>::npos) const
    {
        size_t reverseOffset;

        if(pos == std::basic_string<CharT, TraitsT>::npos) { reverseOffset = 0; }
        else if(pos < mLength)
        {
            reverseOffset = mLength - pos;
        }
        else
        {
            return std::basic_string<CharT, TraitsT>::npos;
        }

        auto findItr = std::find_if(crbegin() + reverseOffset, crend(),
                                    [&ch](const CharT& val) { return TraitsT::eq_int_type(ch, val); });

        if(findItr == crend()) { return std::basic_string<CharT, TraitsT>::npos; }

        return mLength - std::distance(crbegin(), findItr) - 1;
    }

private:
    class Header
    {
    public:
        Header() = default;
        Header(const CharT* str, size_t length): hash(std::basic_string_view<CharT>(str, length)) {}

        RefCounterType refCount = 1;
        BasicStringHash<CharT> hash;
    };

    const CharT* getStringBuffer() const noexcept { return reinterpret_cast<const CharT*>(mData + sizeof(Header)); }

    RefCounterType& getRefCounter() const noexcept { return reinterpret_cast<Header*>(mData)->refCount; }

    RefCounterValueType addRef() const noexcept { return ++getRefCounter(); }

    RefCounterValueType decRef() const noexcept { return --getRefCounter(); }

    BasicStringHash<CharT> getHash() const noexcept { return reinterpret_cast<const Header*>(mData)->hash; }

    void setHash(BasicStringHash<CharT> value) noexcept { return reinterpret_cast<Header*>(mData)->hash = value; }

    void destroy() noexcept
    {
        if(mData != nullptr)
        {
            int count = decRef();

            if(count == 0) { delete[] mData; }
        }

        mData = nullptr;
        mLength = 0;
    }

    uint8_t* mData = nullptr;
    size_t mLength = 0;
};

using SharedString = BasicSharedString<std::string::value_type>;
using WSharedString = BasicSharedString<std::wstring::value_type>;
using U16SharedString = BasicSharedString<std::u16string::value_type>;
using U32Sharedu32string = BasicSharedString<std::u32string::value_type>;
} // namespace scrap