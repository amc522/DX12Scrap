#pragma once

#include <string>

#include <city.h>

namespace scrap
{
template<class CharT>
class BasicStringHash
{
public:
    using ValueType = size_t;

    BasicStringHash() = default;

    explicit BasicStringHash(const std::basic_string<CharT>& str);
    explicit BasicStringHash(std::basic_string_view<CharT> str);

    template<size_t N>
    explicit BasicStringHash(const CharT (&str)[N])
    {
        mHash = CityHash64(reinterpret_cast<const char*>(str),
                           (str[N - 1] == 0) ? (N - 1) * sizeof(CharT) : N * sizeof(CharT));
    }

    explicit BasicStringHash(ValueType hashValue);
    BasicStringHash(const BasicStringHash<CharT>&) = default;
    BasicStringHash(BasicStringHash<CharT>&&) = default;
    ~BasicStringHash() = default;

    ValueType hashValue() const;

    explicit operator size_t() const { return hashValue(); }

    BasicStringHash<CharT>& operator=(const BasicStringHash<CharT>&) = default;
    BasicStringHash<CharT>& operator=(BasicStringHash<CharT>&&) = default;

    friend bool operator==(const BasicStringHash<CharT>& left, const BasicStringHash<CharT>& right)
    {
        return left.hashValue() == right.hashValue();
    }

    friend bool operator!=(const BasicStringHash<CharT>& left, const BasicStringHash<CharT>& right)
    {
        return !(left == right);
    }

    friend bool operator<(const BasicStringHash<CharT>& left, const BasicStringHash<CharT>& right)
    {
        return left.hashValue() < right.hashValue();
    }

    friend bool operator>(const BasicStringHash<CharT>& left, const BasicStringHash<CharT>& right)
    {
        return left.hashValue() > right.hashValue();
    }
    friend bool operator<=(const BasicStringHash<CharT>& left, const BasicStringHash<CharT>& right)
    {
        return !(left > right);
    }

    friend bool operator>=(const BasicStringHash<CharT>& left, const BasicStringHash<CharT>& right)
    {
        return !(left < right);
    }

private:
    ValueType mHash = 0;
};

template<class CharT>
BasicStringHash<CharT>::BasicStringHash(const std::basic_string<CharT>& str)
{
    mHash = CityHash64(reinterpret_cast<const char*>(str.c_str()), str.size() * sizeof(CharT));
}

template<class CharT>
BasicStringHash<CharT>::BasicStringHash(std::basic_string_view<CharT> str)
{
    mHash = CityHash64(reinterpret_cast<const char*>(str.data()), str.size() * sizeof(CharT));
}

template<class CharT>
BasicStringHash<CharT>::BasicStringHash(ValueType value): mHash(value)
{}

template<class CharT>
inline size_t BasicStringHash<CharT>::hashValue() const
{
    return mHash;
}

using StringHash = BasicStringHash<std::string::value_type>;
using WStringHash = BasicStringHash<std::wstring::value_type>;
} // namespace scrap

namespace std
{
template<class CharT>
struct hash<scrap::BasicStringHash<CharT>>
{
    size_t operator()(const scrap::BasicStringHash<CharT>& stringHash) const { return stringHash.hashValue(); }
};
} // namespace std