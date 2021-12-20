// Collection of functions to help convert D3D12 objects and enums to strings for printing and debugging.

#pragma once

#include "StringUtils.h"

#include <string>
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

template<>
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
    default: assert(false); return "Unknown D3D_FEATURE_LEVEL";
    }
}

template<>
constexpr std::string_view ToStringView(D3D_ROOT_SIGNATURE_VERSION version)
{
    switch(version)
    {
    case D3D_ROOT_SIGNATURE_VERSION_1_0: return "D3D_ROOT_SIGNATURE_VERSION_1_0";
    case D3D_ROOT_SIGNATURE_VERSION_1_1: return "D3D_ROOT_SIGNATURE_VERSION_1_1";
    default: assert(false); return "Unknown D3D_ROOT_SIGNATURE_VERSION";
    }
}

template<>
constexpr std::string_view ToStringView(D3D_SHADER_INPUT_TYPE sit)
{
    switch(sit)
    {
    case D3D_SIT_CBUFFER: return "D3D_SIT_CBUFFER";
    case D3D_SIT_TBUFFER: return "D3D_SIT_TBUFFER";
    case D3D_SIT_TEXTURE: return "D3D_SIT_TEXTURE";
    case D3D_SIT_SAMPLER: return "D3D_SIT_SAMPLER";
    case D3D_SIT_UAV_RWTYPED: return "D3D_SIT_UAV_RWTYPED";
    case D3D_SIT_STRUCTURED: return "D3D_SIT_STRUCTURED";
    case D3D_SIT_UAV_RWSTRUCTURED: return "D3D_SIT_UAV_RWSTRUCTURED";
    case D3D_SIT_BYTEADDRESS: return "D3D_SIT_BYTEADDRESS";
    case D3D_SIT_UAV_RWBYTEADDRESS: return "D3D_SIT_UAV_RWBYTEADDRESS";
    case D3D_SIT_UAV_APPEND_STRUCTURED: return "D3D_SIT_UAV_APPEND_STRUCTURED";
    case D3D_SIT_UAV_CONSUME_STRUCTURED: return "D3D_SIT_UAV_CONSUME_STRUCTURED";
    case D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER: return "D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER";
    case D3D_SIT_RTACCELERATIONSTRUCTURE: return "D3D_SIT_RTACCELERATIONSTRUCTURE";
    case D3D_SIT_UAV_FEEDBACKTEXTURE: return "D3D_SIT_UAV_FEEDBACKTEXTURE";
    default: assert(false); return "Unknown D3D12_SHADER_INPUT_TYPE";
    }
}

template<>
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
    default: assert(false); return "Unknown D3D_SHADER_MODEL";
    }
}

template<>
constexpr std::string_view ToStringView(D3D_SHADER_VARIABLE_TYPE variableType)
{
    switch(variableType)
    {
    case D3D_SVT_VOID: return "D3D_SVT_VOID";
    case D3D_SVT_BOOL: return "D3D_SVT_BOOL";
    case D3D_SVT_INT: return "D3D_SVT_INT";
    case D3D_SVT_FLOAT: return "D3D_SVT_FLOAT";
    case D3D_SVT_STRING: return "D3D_SVT_STRING";
    case D3D_SVT_TEXTURE: return "D3D_SVT_TEXTURE";
    case D3D_SVT_TEXTURE1D: return "D3D_SVT_TEXTURE1D";
    case D3D_SVT_TEXTURE2D: return "D3D_SVT_TEXTURE2D";
    case D3D_SVT_TEXTURE3D: return "D3D_SVT_TEXTURE3D";
    case D3D_SVT_TEXTURECUBE: return "D3D_SVT_TEXTURECUBE";
    case D3D_SVT_SAMPLER: return "D3D_SVT_SAMPLER";
    case D3D_SVT_SAMPLER1D: return "D3D_SVT_SAMPLER1D";
    case D3D_SVT_SAMPLER2D: return "D3D_SVT_SAMPLER2D";
    case D3D_SVT_SAMPLER3D: return "D3D_SVT_SAMPLER3D";
    case D3D_SVT_SAMPLERCUBE: return "D3D_SVT_SAMPLERCUBE";
    case D3D_SVT_PIXELSHADER: return "D3D_SVT_PIXELSHADER";
    case D3D_SVT_VERTEXSHADER: return "D3D_SVT_VERTEXSHADER";
    case D3D_SVT_PIXELFRAGMENT: return "D3D_SVT_PIXELFRAGMENT";
    case D3D_SVT_VERTEXFRAGMENT: return "D3D_SVT_VERTEXFRAGMENT";
    case D3D_SVT_UINT: return "D3D_SVT_UINT";
    case D3D_SVT_UINT8: return "D3D_SVT_UINT8";
    case D3D_SVT_GEOMETRYSHADER: return "D3D_SVT_GEOMETRYSHADER";
    case D3D_SVT_RASTERIZER: return "D3D_SVT_RASTERIZER";
    case D3D_SVT_DEPTHSTENCIL: return "D3D_SVT_DEPTHSTENCIL";
    case D3D_SVT_BLEND: return "D3D_SVT_BLEND";
    case D3D_SVT_BUFFER: return "D3D_SVT_BUFFER";
    case D3D_SVT_CBUFFER: return "D3D_SVT_CBUFFER";
    case D3D_SVT_TBUFFER: return "D3D_SVT_TBUFFER";
    case D3D_SVT_TEXTURE1DARRAY: return "D3D_SVT_TEXTURE1DARRAY";
    case D3D_SVT_TEXTURE2DARRAY: return "D3D_SVT_TEXTURE2DARRAY";
    case D3D_SVT_RENDERTARGETVIEW: return "D3D_SVT_RENDERTARGETVIEW";
    case D3D_SVT_DEPTHSTENCILVIEW: return "D3D_SVT_DEPTHSTENCILVIEW";
    case D3D_SVT_TEXTURE2DMS: return "D3D_SVT_TEXTURE2DMS";
    case D3D_SVT_TEXTURE2DMSARRAY: return "D3D_SVT_TEXTURE2DMSARRAY";
    case D3D_SVT_TEXTURECUBEARRAY: return "D3D_SVT_TEXTURECUBEARRAY";
    case D3D_SVT_HULLSHADER: return "D3D_SVT_HULLSHADER";
    case D3D_SVT_DOMAINSHADER: return "D3D_SVT_DOMAINSHADER";
    case D3D_SVT_INTERFACE_POINTER: return "D3D_SVT_INTERFACE_POINTER";
    case D3D_SVT_COMPUTESHADER: return "D3D_SVT_COMPUTESHADER";
    case D3D_SVT_DOUBLE: return "D3D_SVT_DOUBLE";
    case D3D_SVT_RWTEXTURE1D: return "D3D_SVT_RWTEXTURE1D";
    case D3D_SVT_RWTEXTURE1DARRAY: return "D3D_SVT_RWTEXTURE1DARRAY";
    case D3D_SVT_RWTEXTURE2D: return "D3D_SVT_RWTEXTURE2D";
    case D3D_SVT_RWTEXTURE2DARRAY: return "D3D_SVT_RWTEXTURE2DARRAY";
    case D3D_SVT_RWTEXTURE3D: return "D3D_SVT_RWTEXTURE3D";
    case D3D_SVT_RWBUFFER: return "D3D_SVT_RWBUFFER";
    case D3D_SVT_BYTEADDRESS_BUFFER: return "D3D_SVT_BYTEADDRESS_BUFFER";
    case D3D_SVT_RWBYTEADDRESS_BUFFER: return "D3D_SVT_RWBYTEADDRESS_BUFFER";
    case D3D_SVT_STRUCTURED_BUFFER: return "D3D_SVT_STRUCTURED_BUFFER";
    case D3D_SVT_RWSTRUCTURED_BUFFER: return "D3D_SVT_RWSTRUCTURED_BUFFER";
    case D3D_SVT_APPEND_STRUCTURED_BUFFER: return "D3D_SVT_APPEND_STRUCTURED_BUFFER";
    case D3D_SVT_CONSUME_STRUCTURED_BUFFER: return "D3D_SVT_CONSUME_STRUCTURED_BUFFER";
    case D3D_SVT_MIN8FLOAT: return "D3D_SVT_MIN8FLOAT";
    case D3D_SVT_MIN10FLOAT: return "D3D_SVT_MIN10FLOAT";
    case D3D_SVT_MIN16FLOAT: return "D3D_SVT_MIN16FLOAT";
    case D3D_SVT_MIN12INT: return "D3D_SVT_MIN12INT";
    case D3D_SVT_MIN16INT: return "D3D_SVT_MIN16INT";
    case D3D_SVT_MIN16UINT: return "D3D_SVT_MIN16UINT";
    default: return "Unknown D3D_SHADER_VARIABLE_TYPE";
    }
}

template<>
constexpr std::string_view ToStringView(D3D12_COMMAND_LIST_TYPE commandListType)
{
    switch(commandListType)
    {
    case D3D12_COMMAND_LIST_TYPE_DIRECT: return "D3D12_COMMAND_LIST_TYPE_DIRECT";
    case D3D12_COMMAND_LIST_TYPE_BUNDLE: return "D3D12_COMMAND_LIST_TYPE_BUNDLE";
    case D3D12_COMMAND_LIST_TYPE_COMPUTE: return "D3D12_COMMAND_LIST_TYPE_COMPUTE";
    case D3D12_COMMAND_LIST_TYPE_COPY: return "D3D12_COMMAND_LIST_TYPE_COPY";
    case D3D12_COMMAND_LIST_TYPE_VIDEO_DECODE: return "D3D12_COMMAND_LIST_TYPE_VIDEO_DECODE";
    case D3D12_COMMAND_LIST_TYPE_VIDEO_PROCESS: return "D3D12_COMMAND_LIST_TYPE_VIDEO_PROCESS";
    case D3D12_COMMAND_LIST_TYPE_VIDEO_ENCODE: return "D3D12_COMMAND_LIST_TYPE_VIDEO_ENCODE";
    default: assert(false); return "Unknown D3D12_COMMAND_LIST_TYPE";
    }
}

template<>
constexpr std::string_view ToStringView(D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapType)
{
    switch(descriptorHeapType)
    {
    case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV: return "D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV";
    case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER: return "D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER";
    case D3D12_DESCRIPTOR_HEAP_TYPE_RTV: return "D3D12_DESCRIPTOR_HEAP_TYPE_RTV";
    case D3D12_DESCRIPTOR_HEAP_TYPE_DSV: return "D3D12_DESCRIPTOR_HEAP_TYPE_DSV";
    default: assert(false); return "Unknown D3D12_DESCRIPTOR_HEAP_TYPE";
    }
}

template<>
constexpr std::string_view ToStringView(D3D12_RAYTRACING_TIER raytracingTier)
{
    switch(raytracingTier)
    {
    case D3D12_RAYTRACING_TIER_NOT_SUPPORTED: return "D3D12_RAYTRACING_TIER_NOT_SUPPORTED";
    case D3D12_RAYTRACING_TIER_1_0: return "D3D12_RAYTRACING_TIER_1_0";
    case D3D12_RAYTRACING_TIER_1_1: return "D3D12_RAYTRACING_TIER_1_1";
    default: assert(false); return "Unknown D3D12_RAYTRACING_TIER";
    }
}

template<>
constexpr std::string_view ToStringView(D3D12_RESOURCE_BINDING_TIER resourceBindingTier)
{
    switch(resourceBindingTier)
    {
    case D3D12_RESOURCE_BINDING_TIER_1: return "D3D12_RESOURCE_BINDING_TIER_1";
    case D3D12_RESOURCE_BINDING_TIER_2: return "D3D12_RESOURCE_BINDING_TIER_2";
    case D3D12_RESOURCE_BINDING_TIER_3: return "D3D12_RESOURCE_BINDING_TIER_3";
    default: assert(false); return "Unknown D3D12_RESOURCE_BIND_TIER";
    }
}

constexpr std::string_view ToHlslVariableStringView(D3D_SHADER_VARIABLE_TYPE variableType)
{
    switch(variableType)
    {
    case D3D_SVT_VOID: return "void";
    case D3D_SVT_BOOL: return "bool";
    case D3D_SVT_INT: return "int";
    case D3D_SVT_FLOAT: return "float";
    case D3D_SVT_STRING: return "string";
    case D3D_SVT_TEXTURE: return "texture";
    case D3D_SVT_TEXTURE1D: return "Texture1D";
    case D3D_SVT_TEXTURE2D: return "Texture2D";
    case D3D_SVT_TEXTURE3D: return "Texture3D";
    case D3D_SVT_TEXTURECUBE: return "TextureCube";
    case D3D_SVT_SAMPLER: return "Sampler";
    case D3D_SVT_SAMPLER1D: return "Sampler1D";
    case D3D_SVT_SAMPLER2D: return "Sampler2D";
    case D3D_SVT_SAMPLER3D: return "Sampler3D";
    case D3D_SVT_SAMPLERCUBE: return "SamplerCube";
    case D3D_SVT_UINT: return "uint";
    case D3D_SVT_BUFFER: return "Buffer";
    case D3D_SVT_CBUFFER: return "cbuffer";
    case D3D_SVT_TEXTURE1DARRAY: return "Texture1DArray";
    case D3D_SVT_TEXTURE2DARRAY: return "Texture2DArray";
    case D3D_SVT_TEXTURE2DMS: return "Texture2DMS";
    case D3D_SVT_TEXTURE2DMSARRAY: return "Texture2DMSArray";
    case D3D_SVT_TEXTURECUBEARRAY: return "TextureCubeArray";
    case D3D_SVT_DOUBLE: return "double";
    case D3D_SVT_RWTEXTURE1D: return "RWTexture1D";
    case D3D_SVT_RWTEXTURE1DARRAY: return "RWTexture1DArray";
    case D3D_SVT_RWTEXTURE2D: return "RWTexture2D";
    case D3D_SVT_RWTEXTURE2DARRAY: return "RWTexture2DArray";
    case D3D_SVT_RWTEXTURE3D: return "RWTexture3D";
    case D3D_SVT_RWBUFFER: return "RWBuffer";
    case D3D_SVT_BYTEADDRESS_BUFFER: return "ByteAddressBuffer";
    case D3D_SVT_RWBYTEADDRESS_BUFFER: return "RWByteAddressBuffer";
    case D3D_SVT_STRUCTURED_BUFFER: return "StructureBuffer";
    case D3D_SVT_RWSTRUCTURED_BUFFER: return "RWStructuredBuffer";
    case D3D_SVT_APPEND_STRUCTURED_BUFFER: return "AppendStructuredBuffer";
    case D3D_SVT_CONSUME_STRUCTURED_BUFFER: return "ConsumeStructuredBuffer";
    case D3D_SVT_MIN8FLOAT: return "min8float";
    case D3D_SVT_MIN10FLOAT: return "min10float";
    case D3D_SVT_MIN16FLOAT: return "min16float";
    case D3D_SVT_MIN12INT: return "min12int";
    case D3D_SVT_MIN16INT: return "min16int";
    case D3D_SVT_MIN16UINT: return "min16uint";
    default: return "Unknown hlsl data type";
    }
}

constexpr std::string ToHlslVariableString(D3D_SHADER_VARIABLE_TYPE variableType,
                                           uint32_t rows = 1,
                                           uint32_t columns = 1,
                                           uint32_t elements = 1)
{
    if(elements <= 1)
    {
        if(rows <= 1 && columns <= 1) { return std::string(ToHlslVariableStringView(variableType)); }

        if(rows <= 1 && columns > 1) { return fmt::format("{}{}", ToHlslVariableStringView(variableType), columns); }

        if(rows > 1 && columns <= 1) { return fmt::format("{}{}", ToHlslVariableStringView(variableType), rows); }

        if(rows > 1 && columns > 1)
        {
            return fmt::format("{}{}x{}", ToHlslVariableStringView(variableType), rows, columns);
        }

        return std::string(ToHlslVariableStringView(variableType));
    }
    else
    {
        if(rows <= 1 && columns <= 1)
        {
            return fmt::format("{}[{}]", ToHlslVariableStringView(variableType), elements);
        }

        if(rows <= 1 && columns > 1)
        {
            return fmt::format("{}{}[{}]", ToHlslVariableStringView(variableType), columns, elements);
        }

        if(rows > 1 && columns <= 1)
        {
            return fmt::format("{}{}[{}]", ToHlslVariableStringView(variableType), rows, elements);
        }

        if(rows > 1 && columns > 1)
        {
            return fmt::format("{}{}x{}[{}]", ToHlslVariableStringView(variableType), rows, columns, elements);
        }

        return fmt::format("{}[{}]", ToHlslVariableStringView(variableType), elements);
    }
}

constexpr std::string ToHlslVariableString(D3D_SHADER_VARIABLE_TYPE variableType,
                                           std::string_view name,
                                           uint32_t rows = 1,
                                           uint32_t columns = 1,
                                           uint32_t elements = 1)
{
    if(elements <= 1)
    {
        if(rows <= 1 && columns <= 1) { return fmt::format("{} {}", ToHlslVariableStringView(variableType), name); }

        if(rows <= 1 && columns > 1)
        {
            return fmt::format("{}{} {}", ToHlslVariableStringView(variableType), columns, name);
        }

        if(rows > 1 && columns <= 1)
        {
            return fmt::format("{}{} {}", ToHlslVariableStringView(variableType), rows, name);
        }

        if(rows > 1 && columns > 1)
        {
            return fmt::format("{}{}x{} {}", ToHlslVariableStringView(variableType), rows, columns, name);
        }

        return fmt::format("{} {}", ToHlslVariableStringView(variableType), name);
    }
    else
    {
        if(rows <= 1 && columns <= 1)
        {
            return fmt::format("{} {}[{}]", ToHlslVariableStringView(variableType), name, elements);
        }

        if(rows <= 1 && columns > 1)
        {
            return fmt::format("{}{} {}[{}]", ToHlslVariableStringView(variableType), columns, name, elements);
        }

        if(rows > 1 && columns <= 1)
        {
            return fmt::format("{}{} {}[{}]", ToHlslVariableStringView(variableType), rows, name, elements);
        }

        if(rows > 1 && columns > 1)
        {
            return fmt::format("{}{}x{} {}[{}]", ToHlslVariableStringView(variableType), rows, columns, name, elements);
        }

        return fmt::format("{} {}[{}]", ToHlslVariableStringView(variableType), name, elements);
    }
}

} // namespace scrap

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
{};

template<>
struct fmt::formatter<D3D_ROOT_SIGNATURE_VERSION> : public scrap::ToStringViewFormatter<D3D_ROOT_SIGNATURE_VERSION>
{};

template<>
struct fmt::formatter<D3D_SHADER_INPUT_TYPE> : public scrap::ToStringViewFormatter<D3D_SHADER_INPUT_TYPE>
{};

template<>
struct fmt::formatter<D3D_SHADER_MODEL> : public scrap::ToStringViewFormatter<D3D_SHADER_MODEL>
{};

template<>
struct fmt::formatter<D3D_SHADER_VARIABLE_TYPE> : public scrap::ToStringViewFormatter<D3D_SHADER_VARIABLE_TYPE>
{};

template<>
struct fmt::formatter<D3D12_COMMAND_LIST_TYPE> : public scrap::ToStringViewFormatter<D3D12_COMMAND_LIST_TYPE>
{};

template<>
struct fmt::formatter<D3D12_DESCRIPTOR_HEAP_TYPE> : public scrap::ToStringViewFormatter<D3D12_DESCRIPTOR_HEAP_TYPE>
{};

template<>
struct fmt::formatter<D3D12_RAYTRACING_TIER> : public scrap::ToStringViewFormatter<D3D12_RAYTRACING_TIER>
{};

template<>
struct fmt::formatter<D3D12_RESOURCE_BINDING_TIER> : public scrap::ToStringViewFormatter<D3D12_RESOURCE_BINDING_TIER>
{};