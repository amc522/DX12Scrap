// Collection of functions to help convert D3D12 objects and enums to strings for printing and debugging.

#pragma once

#include <string_view>

#include <d3d12.h>
#include <d3dcommon.h>
#include <fmt/format.h>

namespace scrap
{
constexpr std::string_view ToStringView(D3D_FEATURE_LEVEL featureLevel)
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
    default: return "Unknown D3D_FEATURE_LEVEL";
    }
}

constexpr std::string_view ToStringView(D3D_SHADER_MODEL shaderModel)
{
    switch(shaderModel)
    {
    case D3D_SHADER_MODEL_5_1: return "D3D_SHADER_MODEL_5_1";
    case D3D_SHADER_MODEL_6_0: return "D3D_SHADER_MODEL_6_0";
    case D3D_SHADER_MODEL_6_1: return "D3D_SHADER_MODEL_6_1";
    case D3D_SHADER_MODEL_6_2: return "D3D_SHADER_MODEL_6_2";
    case D3D_SHADER_MODEL_6_3: return "D3D_SHADER_MODEL_6_3";
    case D3D_SHADER_MODEL_6_4: return "D3D_SHADER_MODEL_6_4";
    case D3D_SHADER_MODEL_6_5: return "D3D_SHADER_MODEL_6_5";
    case D3D_SHADER_MODEL_6_6: return "D3D_SHADER_MODEL_6_6";
    default: return "Unknown D3D_SHADER_MODEL";
    }
}

constexpr std::string_view ToStringView(D3D12_RESOURCE_BINDING_TIER resourceBindingTier)
{
    switch(resourceBindingTier)
    {
    case D3D12_RESOURCE_BINDING_TIER_1: return "D3D12_RESOURCE_BINDING_TIER_1";
    case D3D12_RESOURCE_BINDING_TIER_2: return "D3D12_RESOURCE_BINDING_TIER_2";
    case D3D12_RESOURCE_BINDING_TIER_3: return "D3D12_RESOURCE_BINDING_TIER_3";
    default: return "Unknow D3D12_RESOURCE_BIND_TIER";
    }
}
} // namespace scrap

#include "StringUtils.h"

template<>
struct fmt::formatter<D3D_FEATURE_LEVEL> : public scrap::ToStringViewFormatter<D3D_FEATURE_LEVEL>
{
};

template<>
struct fmt::formatter<D3D_SHADER_MODEL> : public scrap::ToStringViewFormatter<D3D_SHADER_MODEL>
{
};

template<>
struct fmt::formatter<D3D12_RESOURCE_BINDING_TIER> : public scrap::ToStringViewFormatter<D3D12_RESOURCE_BINDING_TIER>
{
};