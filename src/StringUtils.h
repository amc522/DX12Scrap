#pragma once

#include <string_view>

#include <fmt/format.h>

namespace scrap
{
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