#pragma once

#include <string_view>

#include <fmt/core.h>

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
