#pragma once

#include <fmt/core.h>
#include <string_view>

namespace scrap
{
template<class T>
struct ToStringViewFormatter : public fmt::formatter<std::string_view>
{
    template<typename FormatContext>
    auto format(const T& value, FormatContext& ctx)
    {
        return fmt::formatter<std::string_view>::format(scrap::ToStringView(value), ctx);
    }
};
} // namespace scrap
