#pragma once

#include "StringHash.h"

#include <algorithm>
#include <atomic>
#include <iterator>
#include <stdexcept>
#include <string>
#include <string_view>

namespace scrap
{
template<class CharT, class TraitsT>
class BasicSharedStringInitializer;

template<class CharT, class TraitsT>
class BasicSharedString;

namespace detail
{
//==============================
// BasicSharedStringData
//==============================
template<class CharT, class TraitsT>
class BasicSharedStringData
{
public:
    using difference_type = typename std::basic_string<CharT, TraitsT>::difference_type;
    using pointer = typename std::basic_string<CharT, TraitsT>::pointer;
    using const_pointer = typename std::basic_string<CharT, TraitsT>::const_pointer;
    using value_type = CharT;

    using RefCounterValueType = int;
    using RefCounterType = std::atomic<RefCounterValueType>;

private:
    class Header
    {
    public:
        Header() = default;
        Header(const CharT* str, size_t length): hash(std::basic_string_view<CharT>(str, length)) {}

        RefCounterType refCount = 1;
        BasicStringHash<CharT> hash;
    };

public:
    constexpr BasicSharedStringData() noexcept = default;
    explicit constexpr BasicSharedStringData(size_t length) noexcept
    {
        allocate(length);

        CharT* stringBuffer = accessStringBuffer();
        std::fill(stringBuffer, stringBuffer + mLength + 1, 0);
    }

    constexpr BasicSharedStringData(const CharT* str, size_t length) noexcept
    {
        if(str == nullptr || length == 0) { return; }

        allocate(length);
        CharT* stringBuffer = accessStringBuffer();

        std::memcpy(stringBuffer, str, sizeof(CharT) * length);

        if(!TraitsT::eq(str[length - 1], CharT(0))) { stringBuffer[length] = CharT(0); }
    }

    explicit constexpr BasicSharedStringData(const CharT* str) noexcept
        : BasicSharedStringData(str, TraitsT::length(str))
    {}

    template<size_t N>
    explicit constexpr BasicSharedStringData(const CharT (&str)[N]) noexcept: BasicSharedStringData(str, N)
    {}

    explicit constexpr BasicSharedStringData(std::string_view str) noexcept
        : BasicSharedStringData(str.data(), str.size())
    {}

    explicit constexpr BasicSharedStringData(const std::basic_string<CharT, TraitsT>& str) noexcept
        : BasicSharedStringData(str.data(), str.length())
    {}

    template<class InputIterator>
    BasicSharedStringData(InputIterator first, InputIterator last)
    {
        size_t length = std::distance(first, last);
        if(length == 0) { return; }

        allocate(length);

        mLength = (TraitsT::eq(*(first + (mLength - 1u)), CharT(0))) ? mLength - 1u : mLength;

        CharT* stringBuffer = accessStringBuffer();
        std::copy_n(first, mLength, stringBuffer);
        stringBuffer[mLength] = CharT(0);
    }

    BasicSharedStringData(const BasicSharedStringData<CharT, TraitsT>& other) noexcept: mData(other.mData)
    {
        if(other.mData == nullptr) { return; }

        other.addRef();

        mLength = other.mLength;
    }

    BasicSharedStringData(BasicSharedStringData<CharT, TraitsT>&& other) noexcept
        : mData(other.mData)
        , mLength(other.mLength)
    {
        other.mData = nullptr;
        other.mLength = 0;
    }

    BasicSharedStringData& operator=(const BasicSharedStringData<CharT, TraitsT>& other) noexcept
    {
        destroy();

        if(other.mData == nullptr) { return *this; }

        other.addRef();

        mData = other.mData;
        mLength = other.mLength;

        return *this;
    }

    BasicSharedStringData& operator=(BasicSharedStringData<CharT, TraitsT>&& other) noexcept
    {
        destroy();

        mData = other.mData;
        mLength = other.mLength;

        other.mData = nullptr;
        other.mLength = 0;

        return *this;
    }

    operator std::basic_string_view<CharT, TraitsT>() const noexcept
    {
        if(empty()) { return std::basic_string_view<CharT, TraitsT>(); }

        return std::basic_string_view<CharT, TraitsT>(getStringBuffer(), mLength);
    }

    [[nodiscard]] constexpr bool empty() const noexcept { return mData == nullptr; }

    [[nodiscard]] constexpr CharT* accessStringBuffer() noexcept
    {
        return reinterpret_cast<CharT*>(mData + sizeof(Header));
    }

    [[nodiscard]] constexpr const CharT* getStringBuffer() const noexcept
    {
        return reinterpret_cast<const CharT*>(mData + sizeof(Header));
    }

    [[nodiscard]] RefCounterType& getRefCount() const noexcept { return reinterpret_cast<Header*>(mData)->refCount; }

    RefCounterValueType addRef() const noexcept { return ++getRefCount(); }

    RefCounterValueType decRef() const noexcept { return --getRefCount(); }

    [[nodiscard]] constexpr BasicStringHash<CharT> getHash() const noexcept
    {
        return reinterpret_cast<const Header*>(mData)->hash;
    }

    void setHash(BasicStringHash<CharT> value) noexcept { return reinterpret_cast<Header*>(mData)->hash = value; }

protected:
    virtual ~BasicSharedStringData() { destroy(); }

    void allocate(size_t length) noexcept
    {
        mLength = length;
        if(length == 0) { return; }

        // Using mLength + 1 to account for the terminating \0
        mData = new uint8_t[(mLength + 1) * sizeof(CharT) + sizeof(Header)];
        new(mData) Header();
        CharT* charBuffer = new(mData + sizeof(Header)) CharT[mLength + 1];
    }

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

//==============================
// SharedStringConstIterator
//==============================

template<class SharedStringT, bool IsConstV = true>
class SharedStringConstIterator
{
public:
    using SharedStringType = std::conditional_t<IsConstV, const SharedStringT, SharedStringT>;
    using iterator_category = std::random_access_iterator_tag;
    using value_type = typename SharedStringT::value_type;
    using difference_type = typename SharedStringT::difference_type;
    using pointer = typename SharedStringT::const_pointer;
    using reference = const value_type&;

    constexpr SharedStringConstIterator() noexcept = default;

    constexpr SharedStringConstIterator(SharedStringType* sharedString, size_t index) noexcept
        : mSharedString(sharedString)
        , mIndex(index)
    {}

    constexpr SharedStringConstIterator& operator=(const SharedStringConstIterator&) noexcept = default;

    [[nodiscard]] constexpr reference operator*() const noexcept { return mSharedString->operator[](mIndex); }

    [[nodiscard]] constexpr pointer operator->() const noexcept { return mSharedString->data() + mIndex; }

    constexpr SharedStringConstIterator& operator++() noexcept
    {
        ++mIndex;
        return *this;
    }

    constexpr SharedStringConstIterator operator++(int) noexcept
    {
        SharedStringConstIterator ret = *this;
        ++(*this);
        return ret;
    }

    constexpr SharedStringConstIterator& operator--() noexcept
    {
        --mIndex;
        return *this;
    }

    constexpr SharedStringConstIterator operator--(int) noexcept
    {
        SharedStringConstIterator ret = *this;
        --(*this);
        return ret;
    }

    constexpr SharedStringConstIterator& operator+=(const difference_type offset) noexcept
    {
        mIndex += offset;
        return *this;
    }

    [[nodiscard]] constexpr SharedStringConstIterator operator+(const difference_type offset) const noexcept
    {
        SharedStringConstIterator ret = *this;
        ret += offset;
        return ret;
    }

    constexpr SharedStringConstIterator& operator-=(const difference_type offset) noexcept { return *this += -offset; }

    [[nodiscard]] constexpr SharedStringConstIterator operator-(const difference_type offset) const noexcept
    {
        SharedStringConstIterator ret = *this;
        ret -= offset;
        return ret;
    }

    [[nodiscard]] constexpr difference_type operator-(const SharedStringConstIterator& rhs) const noexcept
    {
        return mIndex - rhs.mIndex;
    }

    [[nodiscard]] constexpr reference operator[](const difference_type offset) const noexcept
    {
        return *(*this + offset);
    }

    [[nodiscard]] constexpr bool operator==(const SharedStringConstIterator& rhs) const noexcept
    {
        return mIndex == rhs.mIndex;
    }

    [[nodiscard]] bool operator!=(const SharedStringConstIterator& rhs) const noexcept { return !(*this == rhs); }

    [[nodiscard]] bool operator<(const SharedStringConstIterator& rhs) const noexcept { return mIndex < rhs.mIndex; }

    [[nodiscard]] bool operator>(const SharedStringConstIterator& rhs) const noexcept { return rhs < *this; }

    [[nodiscard]] bool operator<=(const SharedStringConstIterator& rhs) const noexcept { return !(rhs < *this); }

    [[nodiscard]] bool operator>=(const SharedStringConstIterator& rhs) const noexcept { return !(*this < rhs); }

    SharedStringType* mSharedString = nullptr;
    size_t mIndex = 0;
};

//==============================
// SharedStringIterator
//==============================

template<class SharedStringT>
class SharedStringIterator : public SharedStringConstIterator<SharedStringT, false>
{
public:
    using Base = SharedStringConstIterator<SharedStringT>;

    using iterator_category = std::random_access_iterator_tag;
    using value_type = typename SharedStringT::value_type;
    using difference_type = typename SharedStringT::difference_type;
    using pointer = typename SharedStringT::pointer;
    using reference = value_type&;

    using Base::Base;

    constexpr SharedStringIterator& operator=(const SharedStringIterator&) noexcept = default;

    [[nodiscard]] constexpr reference operator*() const noexcept { return const_cast<reference>(Base::operator*()); }

    [[nodiscard]] constexpr pointer operator->() const noexcept { return this->mSharedString->data() + this->mIndex; }

    constexpr SharedStringIterator& operator++() noexcept
    {
        Base::operator++();
        return *this;
    }

    constexpr SharedStringIterator operator++(int) noexcept
    {
        SharedStringIterator ret = *this;
        Base::operator++();
        return ret;
    }

    constexpr SharedStringIterator& operator--() noexcept
    {
        Base::operator--();
        return *this;
    }

    constexpr SharedStringIterator operator--(int) noexcept
    {
        SharedStringIterator ret = *this;
        Base::operator--();
        return ret;
    }

    constexpr SharedStringIterator& operator+=(const difference_type offset) noexcept
    {
        Base::operator+=(offset);
        return *this;
    }

    [[nodiscard]] constexpr SharedStringIterator operator+(const difference_type offset) const noexcept
    {
        SharedStringIterator ret = *this;
        ret += offset;
        return ret;
    }

    constexpr SharedStringIterator& operator-=(const difference_type offset) noexcept
    {
        Base::operator-=(offset);
        return *this;
    }

    using Base::operator-;

    [[nodiscard]] constexpr SharedStringIterator operator-(const difference_type offset) const noexcept
    {
        SharedStringIterator ret = *this;
        ret -= offset;
        return ret;
    }

    [[nodiscard]] constexpr reference operator[](const difference_type offset) const noexcept
    {
        return const_cast<reference>(Base::operator[](offset));
    }
};
} // namespace detail

template<class SharedStringT, bool IsConstV>
[[nodiscard]] constexpr detail::SharedStringConstIterator<SharedStringT, IsConstV>
operator+(typename detail::SharedStringConstIterator<SharedStringT, IsConstV>::difference_type offset,
          detail::SharedStringConstIterator<SharedStringT, IsConstV> next) noexcept
{
    return next += offset;
}

template<class SharedStringT, bool IsConstV>
[[nodiscard]] constexpr detail::SharedStringIterator<SharedStringT>
operator+(typename detail::SharedStringIterator<SharedStringT>::difference_type offset,
          detail::SharedStringIterator<SharedStringT> next) noexcept
{
    return next += offset;
}

//==============================
// BasicSharedStringInitializer
//==============================
template<class CharT, class TraitsT = std::char_traits<CharT>>
class BasicSharedStringInitializer : public detail::BasicSharedStringData<CharT, TraitsT>
{
private:
    using Base = detail::BasicSharedStringData<CharT, TraitsT>;

public:
    static const size_t npos = size_t(-1);

    using iterator = detail::SharedStringIterator<BasicSharedStringInitializer<CharT, TraitsT>>;
    using const_iterator = detail::SharedStringConstIterator<BasicSharedStringInitializer<CharT, TraitsT>>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    using Base::Base;

    explicit constexpr BasicSharedStringInitializer(size_t length)
        : detail::BasicSharedStringData<CharT, TraitsT>(length)
    {}

    BasicSharedStringInitializer(const BasicSharedStringInitializer<CharT, TraitsT>&) = delete;
    BasicSharedStringInitializer(BasicSharedStringInitializer<CharT, TraitsT>&&) noexcept = default;

    BasicSharedStringInitializer<CharT, TraitsT>&
    operator=(const BasicSharedStringInitializer<CharT, TraitsT>&) = delete;
    BasicSharedStringInitializer<CharT, TraitsT>&
    operator=(BasicSharedStringInitializer<CharT, TraitsT>&&) noexcept = default;

    const CharT at(size_t index) const
    {
        if(index >= this->mLength)
        {
            throw std::out_of_range("BasicSharedString::at() called with an index that is out of bounds");
        }

        return this->getStringBuffer()[index];
    }

    const CharT& operator[](size_t index) noexcept { return this->accessStringBuffer()[index]; }
    const CharT& operator[](size_t index) const noexcept { return this->getStringBuffer()[index]; }

    CharT* data() noexcept { return this->accessStringBuffer(); }
    const CharT* data() const noexcept { return this->getStringBuffer(); }

    const CharT* c_str() const noexcept { return this->getStringBuffer(); }

    constexpr iterator begin() const noexcept { return {this, 0}; }

    constexpr iterator end() const noexcept { return {this, this->size()}; }

    constexpr const_iterator cbegin() const noexcept { return {this, 0}; }

    constexpr const_iterator cend() const noexcept { return {this, this->size()}; }

    constexpr reverse_iterator rbegin() const noexcept { return reverse_iterator{this->end()}; }

    constexpr reverse_iterator rend() const noexcept { return reverse_iterator{this->begin()}; }

    constexpr const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator{this->cend()}; }

    constexpr const_reverse_iterator crend() const noexcept { return const_reverse_iterator{this->cbegin()}; }

    size_t size() const noexcept { return this->mLength; }

    size_t length() const noexcept { return this->mLength; }
};

//==============================
// BasicSharedString
//==============================
template<class CharT, class TraitsT = std::char_traits<CharT>>
class BasicSharedString : public detail::BasicSharedStringData<CharT, TraitsT>
{
private:
    using Base = detail::BasicSharedStringData<CharT, TraitsT>;

public:
    static const size_t npos = size_t(-1);

    using difference_type = typename std::allocator_traits<std::allocator<CharT>>::difference_type;
    using iterator = detail::SharedStringConstIterator<BasicSharedString<CharT, TraitsT>>;
    using const_iterator = detail::SharedStringConstIterator<BasicSharedString<CharT, TraitsT>>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using value_type = detail::BasicSharedStringData<CharT, TraitsT>::value_type;

    using Base::Base;

    BasicSharedString() = default;

    explicit constexpr BasicSharedString(const CharT* str) noexcept: detail::BasicSharedStringData<CharT, TraitsT>(str)
    {}

    constexpr BasicSharedString(const CharT* str, size_t length) noexcept
        : detail::BasicSharedStringData<CharT, TraitsT>(length)
    {}

    template<size_t N>
    explicit constexpr BasicSharedString(const CharT (&str)[N]) noexcept
        : detail::BasicSharedStringData<CharT, TraitsT>(str)
    {}

    explicit constexpr BasicSharedString(std::string_view str) noexcept
        : detail::BasicSharedStringData<CharT, TraitsT>(str)
    {}

    explicit constexpr BasicSharedString(const std::basic_string<CharT, TraitsT>& str) noexcept
        : detail::BasicSharedStringData<CharT, TraitsT>(str)
    {}

    template<class InputIterator>
    BasicSharedString(InputIterator first, InputIterator last)
        : detail::BasicSharedStringData<CharT, TraitsT>(first, last)
    {}

    BasicSharedString(BasicSharedStringInitializer<CharT, TraitsT>&& initializer) noexcept
        : detail::BasicSharedStringData<CharT, TraitsT>(std::move(initializer))
    {}

    constexpr BasicSharedString(const BasicSharedString<CharT, TraitsT>& other) noexcept = default;
    constexpr BasicSharedString(BasicSharedString<CharT, TraitsT>&& other) noexcept = default;

    ~BasicSharedString() noexcept final = default;

    BasicSharedString<CharT, TraitsT>& operator=(const BasicSharedString<CharT, TraitsT>& other) noexcept = default;

    BasicSharedString<CharT, TraitsT>& operator=(BasicSharedString<CharT, TraitsT>&& other) noexcept = default;

    operator BasicStringHash<CharT>() const noexcept
    {
        if(this->empty()) { return BasicStringHash<CharT>{}; }

        return this->getHash();
    }

    operator std::basic_string_view<CharT, TraitsT>() const noexcept
    {
        if(this->empty()) { return std::basic_string<CharT, TraitsT>(); }

        return std::basic_string_view<CharT, TraitsT>(this->getStringBuffer(), this->mLength);
    }

    explicit operator std::basic_string<CharT, TraitsT>() const noexcept
    {
        if(this->empty()) { return std::basic_string<CharT, TraitsT>(); }

        return std::basic_string<CharT, TraitsT>(this->getStringBuffer(), this->mLength);
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
        if(index >= this->mLength)
        {
            throw std::out_of_range("BasicSharedString::at() called with an index that is out of bounds");
        }

        return this->getStringBuffer()[index];
    }

    const CharT& operator[](size_t index) const noexcept { return this->getStringBuffer()[index]; }

    const CharT* data() const noexcept { return this->getStringBuffer(); }

    const CharT* c_str() const noexcept { return this->getStringBuffer(); }

    constexpr iterator begin() const noexcept { return {this, 0}; }

    constexpr iterator end() const noexcept { return {this, this->size()}; }

    constexpr const_iterator cbegin() const noexcept { return {this, 0}; }

    constexpr const_iterator cend() const noexcept { return {this, this->size()}; }

    constexpr reverse_iterator rbegin() const noexcept { return reverse_iterator{this->end()}; }

    constexpr reverse_iterator rend() const noexcept { return reverse_iterator{this->begin()}; }

    constexpr const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator{this->cend()}; }

    constexpr const_reverse_iterator crend() const noexcept { return const_reverse_iterator{this->cbegin()}; }

    size_t size() const noexcept { return this->mLength; }

    size_t length() const noexcept { return this->mLength; }

    BasicStringHash<CharT> hash() const noexcept
    {
        return (this->empty()) ? this->getHash() : BasicStringHash<CharT>{};
    }

    template<class TraitsU>
    size_t find(std::basic_string_view<CharT, TraitsU> str, size_t pos = 0) const
    {
        std::basic_string_view<CharT, TraitsU> sharedStrView{this->getStringBuffer(), this->mLength};
        return sharedStrView.find(str, pos);
    }

    size_t find(std::basic_string<CharT, TraitsT> str, size_t pos = 0) const
    {
        std::basic_string_view<CharT, TraitsT> sharedStrView{this->getStringBuffer(), this->mLength};
        return sharedStrView.find(str, pos);
    }

    size_t find(const CharT* s, size_t pos, size_t count) const
    {
        std::basic_string_view<CharT, TraitsT> sharedStrView{this->getStringBuffer(), this->mLength};
        return sharedStrView.find(s, pos, count);
    }

    size_t find(const CharT* s, size_t pos = 0) const
    {
        std::basic_string_view<CharT, TraitsT> sharedStrView{this->getStringBuffer(), this->mLength};
        return sharedStrView.find(s, pos);
    }

    size_t find(CharT ch, size_t pos = 0) const
    {
        std::basic_string_view<CharT, TraitsT> sharedStrView{this->getStringBuffer(), this->mLength};
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
        std::basic_string_view<CharT, TraitsT> sharedStrView{this->getStringBuffer(), this->mLength};
        return sharedStrView.rfind(str.data(), pos, str.size());
    }

    size_t rfind(const CharT* s, size_t pos, size_t count) const
    {
        std::basic_string_view<CharT, TraitsT> sharedStrView{this->getStringBuffer(), this->mLength};
        return sharedStrView.rfind(s, pos, count);
    }

    size_t rfind(const CharT* s, size_t pos = std::basic_string<CharT, TraitsT>::npos) const
    {
        std::basic_string_view<CharT, TraitsT> sharedStrView{this->getStringBuffer(), this->mLength};
        return sharedStrView.rfind(s, pos);
    }

    size_t rfind(CharT ch, size_t pos = std::basic_string<CharT, TraitsT>::npos) const
    {
        std::basic_string_view<CharT, TraitsT> sharedStrView{this->getStringBuffer(), this->mLength};
        return sharedStrView.rfind(ch, pos);
    }

    template<class CharU>
    size_t rfind(const CharU& ch, size_t pos = std::basic_string<CharT, TraitsT>::npos) const
    {
        size_t reverseOffset;

        if(pos == std::basic_string<CharT, TraitsT>::npos) { reverseOffset = 0; }
        else if(pos < this->mLength)
        {
            reverseOffset = this->mLength - pos;
        }
        else
        {
            return std::basic_string<CharT, TraitsT>::npos;
        }

        auto findItr = std::find_if(crbegin() + reverseOffset, crend(),
                                    [&ch](const CharT& val) { return TraitsT::eq_int_type(ch, val); });

        if(findItr == crend()) { return std::basic_string<CharT, TraitsT>::npos; }

        return this->mLength - std::distance(crbegin(), findItr) - 1;
    }
};

using SharedStringInitializer = BasicSharedStringInitializer<std::string::value_type>;
using WSharedStringInitializer = BasicSharedStringInitializer<std::wstring::value_type>;
using U16SharedStringInitializer = BasicSharedStringInitializer<std::u16string::value_type>;
using U32SharedStringInitializer = BasicSharedStringInitializer<std::u32string::value_type>;

using SharedString = BasicSharedString<std::string::value_type>;
using WSharedString = BasicSharedString<std::wstring::value_type>;
using U16SharedString = BasicSharedString<std::u16string::value_type>;
using U32SharedString = BasicSharedString<std::u32string::value_type>;
} // namespace scrap