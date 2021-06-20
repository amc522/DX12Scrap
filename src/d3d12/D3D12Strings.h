// Collection of functions to help convert D3D12 objects and enums to strings for printing and debugging.

#pragma once

#include <d3dcommon.h>
#include <fmt/format.h>
#include <string_view>

namespace scrap
{
constexpr std::string_view ToString(D3D_FEATURE_LEVEL featureLevel)
{
    switch(featureLevel)
    {
    case D3D_FEATURE_LEVEL_1_0_CORE: return "D3D_FEATURE_LEVEL_1_0_CORE";
    case D3D_FEATURE_LEVEL_9_1: return "D3D_FEATURE_LEVEL_9_1";
    case D3D_FEATURE_LEVEL_9_2: return "D3D_FEATURE_LEVEL_9_2";
    case D3D_FEATURE_LEVEL_9_3: return "D3D_FEATURE_LEVEL_9_3";
    case D3D_FEATURE_LEVEL_10_0: return "D3D_FEATURE_LEVEL_10_0";
    case D3D_FEATURE_LEVEL_10_1: return "D3D_FEATURE_LEVEL_10_1";
    case D3D_FEATURE_LEVEL_11_0: return "D3D_FEATURE_LEVEL_11_0";
    case D3D_FEATURE_LEVEL_11_1: return "D3D_FEATURE_LEVEL_11_1";
    case D3D_FEATURE_LEVEL_12_0: return "D3D_FEATURE_LEVEL_12_0";
    case D3D_FEATURE_LEVEL_12_1: return "D3D_FEATURE_LEVEL_12_1";
    default: return "Unknown feature level";
    }
}
} // namespace scrap

template<>
struct fmt::formatter<D3D_FEATURE_LEVEL> : public fmt::formatter<std::string_view>
{
    template<typename FormatContext>
    auto format(D3D_FEATURE_LEVEL featureLevel, FormatContext& ctx)
    {
        return format_to(ctx.out(), scrap::ToString(featureLevel));
    }
};