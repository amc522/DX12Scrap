// Collection of functions to help convert D3D12 objects and enums to strings for printing and debugging.

#pragma once

#include <string_view>

#include <d3d12.h>
#include <d3dcommon.h>
#include <fmt/format.h>

namespace scrap
{
struct HRESULT_t
{
    HRESULT_t() = default;
    explicit HRESULT_t(HRESULT hr_): hr(hr_) {}

    HRESULT hr = S_OK;
};

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

constexpr std::string_view ToStringView(D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapType)
{
    switch(descriptorHeapType)
    {
    case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV: return "D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV";
    case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER: return "D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER";
    case D3D12_DESCRIPTOR_HEAP_TYPE_RTV: return "D3D12_DESCRIPTOR_HEAP_TYPE_RTV";
    case D3D12_DESCRIPTOR_HEAP_TYPE_DSV: return "D3D12_DESCRIPTOR_HEAP_TYPE_DSV";
    default: return "Unknown D3D12_DESCRIPTOR_HEAP_TYPE";
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
struct fmt::formatter<scrap::HRESULT_t> : public fmt::formatter<std::string_view>
{
    template<typename FormatContext>
    auto format(scrap::HRESULT_t value, FormatContext& ctx)
    {
        switch(value.hr)
        {
        case S_OK: return fmt::formatter<std::string_view>::format("S_OK", ctx);
        case S_FALSE: return fmt::formatter<std ::string_view>::format("S_FALSE", ctx);
        case E_ABORT: return fmt::formatter<std::string_view>::format("E_ABORT", ctx);
        case E_ACCESSDENIED: return fmt::formatter<std::string_view>::format("E_ACCESSDENIED", ctx);
        case E_FAIL: return fmt::formatter<std::string_view>::format("E_FAIL", ctx);
        case E_HANDLE: return fmt::formatter<std::string_view>::format("E_HANDLE", ctx);
        case E_INVALIDARG: return fmt::formatter<std::string_view>::format("E_INVALIDARG", ctx);
        case E_NOINTERFACE: return fmt::formatter<std::string_view>::format("E_NOINTERFACE", ctx);
        case E_NOTIMPL: return fmt::formatter<std::string_view>::format("E_NOTIMPL", ctx);
        case E_OUTOFMEMORY: return fmt::formatter<std::string_view>::format("E_OUTOFMEMORY", ctx);
        case E_POINTER: return fmt::formatter<std::string_view>::format("E_POINTER", ctx);
        case E_UNEXPECTED: return fmt::formatter<std::string_view>::format("E_UNEXPECTED", ctx);
        case DXGI_ERROR_ACCESS_DENIED: return fmt::formatter<std::string_view>::format("DXGI_ERROR_ACCESS_DENIED", ctx);
        case DXGI_ERROR_ACCESS_LOST: return fmt::formatter<std::string_view>::format("DXGI_ERROR_ACCESS_LOST", ctx);
        case DXGI_ERROR_ALREADY_EXISTS:
            return fmt::formatter<std::string_view>::format("DXGI_ERROR_ALREADY_EXISTS", ctx);
        case DXGI_ERROR_CANNOT_PROTECT_CONTENT:
            return fmt::formatter<std::string_view>::format("DXGI_ERROR_CANNOT_PROTECT_CONTENT", ctx);
        case DXGI_ERROR_DEVICE_HUNG: return fmt::formatter<std::string_view>::format("DXGI_ERROR_DEVICE_HUNG", ctx);
        case DXGI_ERROR_DEVICE_REMOVED:
            return fmt::formatter<std::string_view>::format("DXGI_ERROR_DEVICE_REMOVED", ctx);
        case DXGI_ERROR_DEVICE_RESET: return fmt::formatter<std::string_view>::format("DXGI_ERROR_DEVICE_RESET", ctx);
        case DXGI_ERROR_DRIVER_INTERNAL_ERROR:
            return fmt::formatter<std::string_view>::format("DXGI_ERROR_DRIVER_INTERNAL_ERROR", ctx);
        case DXGI_ERROR_FRAME_STATISTICS_DISJOINT:
            return fmt::formatter<std::string_view>::format("DXGI_ERROR_FRAME_STATISTICS_DISJOINT", ctx);
        case DXGI_ERROR_GRAPHICS_VIDPN_SOURCE_IN_USE:
            return fmt::formatter<std::string_view>::format("DXGI_ERROR_GRAPHICS_VIDPN_SOURCE_IN_USE", ctx);
        case DXGI_ERROR_INVALID_CALL: return fmt::formatter<std::string_view>::format("DXGI_ERROR_INVALID_CALL", ctx);
        case DXGI_ERROR_MORE_DATA: return fmt::formatter<std::string_view>::format("DXGI_ERROR_MORE_DATA", ctx);
        case DXGI_ERROR_NAME_ALREADY_EXISTS:
            return fmt::formatter<std::string_view>::format("DXGI_ERROR_NAME_ALREADY_EXISTS", ctx);
        case DXGI_ERROR_NONEXCLUSIVE: return fmt::formatter<std::string_view>::format("DXGI_ERROR_NONEXCLUSIVE", ctx);
        case DXGI_ERROR_NOT_CURRENTLY_AVAILABLE:
            return fmt::formatter<std::string_view>::format("DXGI_ERROR_NOT_CURRENTLY_AVAILABLE", ctx);
        case DXGI_ERROR_NOT_FOUND: return fmt::formatter<std::string_view>::format("DXGI_ERROR_NOT_FOUND", ctx);
        case DXGI_ERROR_REMOTE_CLIENT_DISCONNECTED:
            return fmt::formatter<std::string_view>::format("DXGI_ERROR_REMOTE_CLIENT_DISCONNECTED", ctx);
        case DXGI_ERROR_REMOTE_OUTOFMEMORY:
            return fmt::formatter<std::string_view>::format("DXGI_ERROR_REMOTE_OUTOFMEMORY", ctx);
        case DXGI_ERROR_RESTRICT_TO_OUTPUT_STALE:
            return fmt::formatter<std::string_view>::format("DXGI_ERROR_RESTRICT_TO_OUTPUT_STALE", ctx);
        case DXGI_ERROR_SDK_COMPONENT_MISSING:
            return fmt::formatter<std::string_view>::format("DXGI_ERROR_SDK_COMPONENT_MISSING", ctx);
        case DXGI_ERROR_SESSION_DISCONNECTED:
            return fmt::formatter<std::string_view>::format("DXGI_ERROR_SESSION_DISCONNECTED", ctx);
        case DXGI_ERROR_UNSUPPORTED: return fmt::formatter<std::string_view>::format("DXGI_ERROR_UNSUPPORTED", ctx);
        case DXGI_ERROR_WAIT_TIMEOUT: return fmt::formatter<std::string_view>::format("DXGI_ERROR_WAIT_TIMEOUT", ctx);
        case DXGI_ERROR_WAS_STILL_DRAWING:
            return fmt::formatter<std::string_view>::format("DXGI_ERROR_WAS_STILL_DRAWING", ctx);
        case D3D12_ERROR_ADAPTER_NOT_FOUND:
            return fmt::formatter<std::string_view>::format("D3D12_ERROR_ADAPTER_NOT_FOUND", ctx);
        case D3D12_ERROR_DRIVER_VERSION_MISMATCH:
            return fmt::formatter<std::string_view>::format("D3D12_ERROR_DRIVER_VERSION_MISMATCH", ctx);
        default: return fmt::format_to(ctx.out(), "{:#x}", std::make_unsigned_t<HRESULT>(value.hr));
        }
    }
};

template<>
struct fmt::formatter<D3D_FEATURE_LEVEL> : public scrap::ToStringViewFormatter<D3D_FEATURE_LEVEL>
{
};

template<>
struct fmt::formatter<D3D_SHADER_MODEL> : public scrap::ToStringViewFormatter<D3D_SHADER_MODEL>
{
};

template<>
struct fmt::formatter<D3D12_DESCRIPTOR_HEAP_TYPE> : public scrap::ToStringViewFormatter<D3D12_DESCRIPTOR_HEAP_TYPE>
{
};

template<>
struct fmt::formatter<D3D12_RESOURCE_BINDING_TIER> : public scrap::ToStringViewFormatter<D3D12_RESOURCE_BINDING_TIER>
{
};