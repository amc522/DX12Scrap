#pragma once

#include <string_view>

#include <fmt/format.h>

namespace scrap
{
template<class CharT>
bool StartsWith(std::basic_string_view<CharT> str, std::basic_string_view<CharT> potentialPrefix)
{
    return std::equal(potentialPrefix.begin(), potentialPrefix.end(), str.begin());
}

template<class CharT>
bool EndsWith(std::basic_string_view<CharT> str, std::basic_string_view<CharT> potentialSuffix)
{
    return std::equal(potentialSuffix.rbegin(), potentialSuffix.rend(), str.rbegin());
}

template<class T>
constexpr std::string_view ToStringView(T)
{
    return {};
}

template<class T>
class ToStringViewFormatter : public fmt::formatter<std::string_view>
{
public:
    template<typename FormatContext>
    auto format(const T& value, FormatContext& ctx)
    {
        return fmt::formatter<std::string_view>::format(scrap::ToStringView(value), ctx);
    }
};
} // namespace scrap