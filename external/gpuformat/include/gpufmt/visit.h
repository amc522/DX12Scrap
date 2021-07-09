#pragma once

#include <gpufmt/format.h>

#include <glm/vec4.hpp>
#include <glm/fwd.hpp>

namespace gpufmt {
    template<template<gpufmt::Format> class T, class ...Args>
    static constexpr std::invoke_result_t<T<gpufmt::Format::UNDEFINED>, Args...> visitFormat(gpufmt::Format format, Args&&...args) {
        switch(format)
        {
        case gpufmt::Format::R4G4_UNORM_PACK8:
            return std::invoke(T<gpufmt::Format::R4G4_UNORM_PACK8>(), std::forward<Args>(args)...);
        case gpufmt::Format::R4G4B4A4_UNORM_PACK16:
            return std::invoke(T<gpufmt::Format::R4G4B4A4_UNORM_PACK16>(), std::forward<Args>(args)...);
        case gpufmt::Format::B4G4R4A4_UNORM_PACK16:
            return std::invoke(T<gpufmt::Format::B4G4R4A4_UNORM_PACK16>(), std::forward<Args>(args)...);
        case gpufmt::Format::A4R4G4B4_UNORM_PACK16:
            return std::invoke(T<gpufmt::Format::A4R4G4B4_UNORM_PACK16>(), std::forward<Args>(args)...);
        case gpufmt::Format::R5G6B5_UNORM_PACK16:
            return std::invoke(T<gpufmt::Format::R5G6B5_UNORM_PACK16>(), std::forward<Args>(args)...);
        case gpufmt::Format::B5G6R5_UNORM_PACK16:
            return std::invoke(T<gpufmt::Format::B5G6R5_UNORM_PACK16>(), std::forward<Args>(args)...);
        case gpufmt::Format::R5G5B5A1_UNORM_PACK16:
            return std::invoke(T<gpufmt::Format::R5G5B5A1_UNORM_PACK16>(), std::forward<Args>(args)...);
        case gpufmt::Format::B5G5R5A1_UNORM_PACK16:
            return std::invoke(T<gpufmt::Format::B5G5R5A1_UNORM_PACK16>(), std::forward<Args>(args)...);
        case gpufmt::Format::A1R5G5B5_UNORM_PACK16:
            return std::invoke(T<gpufmt::Format::A1R5G5B5_UNORM_PACK16>(), std::forward<Args>(args)...);
        case gpufmt::Format::R8_UNORM:
            return std::invoke(T<gpufmt::Format::R8_UNORM>(), std::forward<Args>(args)...);
        case gpufmt::Format::R8_SNORM:
            return std::invoke(T<gpufmt::Format::R8_SNORM>(), std::forward<Args>(args)...);
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::R8_USCALED:
            return std::invoke(T<gpufmt::Format::R8_USCALED>(), std::forward<Args>(args)...);
        case gpufmt::Format::R8_SSCALED:
            return std::invoke(T<gpufmt::Format::R8_SSCALED>(), std::forward<Args>(args)...);
#endif
        case gpufmt::Format::R8_UINT:
            return std::invoke(T<gpufmt::Format::R8_UINT>(), std::forward<Args>(args)...);
        case gpufmt::Format::R8_SINT:
            return std::invoke(T<gpufmt::Format::R8_SINT>(), std::forward<Args>(args)...);
        case gpufmt::Format::R8_SRGB:
            return std::invoke(T<gpufmt::Format::R8_SRGB>(), std::forward<Args>(args)...);
        case gpufmt::Format::A8_UNORM:
            return std::invoke(T<gpufmt::Format::A8_UNORM>(), std::forward<Args>(args)...);
        case gpufmt::Format::R8G8_UNORM:
            return std::invoke(T<gpufmt::Format::R8G8_UNORM>(), std::forward<Args>(args)...);
        case gpufmt::Format::R8G8_SNORM:
            return std::invoke(T<gpufmt::Format::R8G8_SNORM>(), std::forward<Args>(args)...);
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::R8G8_USCALED:
            return std::invoke(T<gpufmt::Format::R8G8_USCALED>(), std::forward<Args>(args)...);
        case gpufmt::Format::R8G8_SSCALED:
            return std::invoke(T<gpufmt::Format::R8G8_SSCALED>(), std::forward<Args>(args)...);
#endif
        case gpufmt::Format::R8G8_UINT:
            return std::invoke(T<gpufmt::Format::R8G8_UINT>(), std::forward<Args>(args)...);
        case gpufmt::Format::R8G8_SINT:
            return std::invoke(T<gpufmt::Format::R8G8_SINT>(), std::forward<Args>(args)...);
        case gpufmt::Format::R8G8_SRGB:
            return std::invoke(T<gpufmt::Format::R8G8_SRGB>(), std::forward<Args>(args)...);
        case gpufmt::Format::R8G8B8_UNORM:
            return std::invoke(T<gpufmt::Format::R8G8B8_UNORM>(), std::forward<Args>(args)...);
        case gpufmt::Format::R8G8B8_SNORM:
            return std::invoke(T<gpufmt::Format::R8G8B8_SNORM>(), std::forward<Args>(args)...);
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::R8G8B8_USCALED:
            return std::invoke(T<gpufmt::Format::R8G8B8_USCALED>(), std::forward<Args>(args)...);
        case gpufmt::Format::R8G8B8_SSCALED:
            return std::invoke(T<gpufmt::Format::R8G8B8_SSCALED>(), std::forward<Args>(args)...);
#endif
        case gpufmt::Format::R8G8B8_UINT:
            return std::invoke(T<gpufmt::Format::R8G8B8_UINT>(), std::forward<Args>(args)...);
        case gpufmt::Format::R8G8B8_SINT:
            return std::invoke(T<gpufmt::Format::R8G8B8_SINT>(), std::forward<Args>(args)...);
        case gpufmt::Format::R8G8B8_SRGB:
            return std::invoke(T<gpufmt::Format::R8G8B8_SRGB>(), std::forward<Args>(args)...);
        case gpufmt::Format::B8G8R8_UNORM:
            return std::invoke(T<gpufmt::Format::B8G8R8_UNORM>(), std::forward<Args>(args)...);
        case gpufmt::Format::B8G8R8_SNORM:
            return std::invoke(T<gpufmt::Format::B8G8R8_SNORM>(), std::forward<Args>(args)...);
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::B8G8R8_USCALED:
            return std::invoke(T<gpufmt::Format::B8G8R8_USCALED>(), std::forward<Args>(args)...);
        case gpufmt::Format::B8G8R8_SSCALED:
            return std::invoke(T<gpufmt::Format::B8G8R8_SSCALED>(), std::forward<Args>(args)...);
#endif
        case gpufmt::Format::B8G8R8_UINT:
            return std::invoke(T<gpufmt::Format::B8G8R8_UINT>(), std::forward<Args>(args)...);
        case gpufmt::Format::B8G8R8_SINT:
            return std::invoke(T<gpufmt::Format::B8G8R8_SINT>(), std::forward<Args>(args)...);
        case gpufmt::Format::B8G8R8_SRGB:
            return std::invoke(T<gpufmt::Format::B8G8R8_SRGB>(), std::forward<Args>(args)...);
        case gpufmt::Format::R8G8B8A8_UNORM:
            return std::invoke(T<gpufmt::Format::R8G8B8A8_UNORM>(), std::forward<Args>(args)...);
        case gpufmt::Format::R8G8B8A8_SNORM:
            return std::invoke(T<gpufmt::Format::R8G8B8A8_SNORM>(), std::forward<Args>(args)...);
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::R8G8B8A8_USCALED:
            return std::invoke(T<gpufmt::Format::R8G8B8A8_USCALED>(), std::forward<Args>(args)...);
        case gpufmt::Format::R8G8B8A8_SSCALED:
            return std::invoke(T<gpufmt::Format::R8G8B8A8_SSCALED>(), std::forward<Args>(args)...);
#endif
        case gpufmt::Format::R8G8B8A8_UINT:
            return std::invoke(T<gpufmt::Format::R8G8B8A8_UINT>(), std::forward<Args>(args)...);
        case gpufmt::Format::R8G8B8A8_SINT:
            return std::invoke(T<gpufmt::Format::R8G8B8A8_SINT>(), std::forward<Args>(args)...);
        case gpufmt::Format::R8G8B8A8_SRGB:
            return std::invoke(T<gpufmt::Format::R8G8B8A8_SRGB>(), std::forward<Args>(args)...);
        case gpufmt::Format::B8G8R8A8_UNORM:
            return std::invoke(T<gpufmt::Format::B8G8R8A8_UNORM>(), std::forward<Args>(args)...);
        case gpufmt::Format::B8G8R8A8_SNORM:
            return std::invoke(T<gpufmt::Format::B8G8R8A8_SNORM>(), std::forward<Args>(args)...);
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::B8G8R8A8_USCALED:
            return std::invoke(T<gpufmt::Format::B8G8R8A8_USCALED>(), std::forward<Args>(args)...);
        case gpufmt::Format::B8G8R8A8_SSCALED:
            return std::invoke(T<gpufmt::Format::B8G8R8A8_SSCALED>(), std::forward<Args>(args)...);
#endif
        case gpufmt::Format::B8G8R8A8_UINT:
            return std::invoke(T<gpufmt::Format::B8G8R8A8_UINT>(), std::forward<Args>(args)...);
        case gpufmt::Format::B8G8R8A8_SINT:
            return std::invoke(T<gpufmt::Format::B8G8R8A8_SINT>(), std::forward<Args>(args)...);
        case gpufmt::Format::B8G8R8A8_SRGB:
            return std::invoke(T<gpufmt::Format::B8G8R8A8_SRGB>(), std::forward<Args>(args)...);
        case gpufmt::Format::B8G8R8X8_UNORM:
            return std::invoke(T<gpufmt::Format::B8G8R8X8_UNORM>(), std::forward<Args>(args)...);
        case gpufmt::Format::B8G8R8X8_SRGB:
            return std::invoke(T<gpufmt::Format::B8G8R8X8_SRGB>(), std::forward<Args>(args)...);
        case gpufmt::Format::A8B8G8R8_UNORM_PACK32:
            return std::invoke(T<gpufmt::Format::A8B8G8R8_UNORM_PACK32>(), std::forward<Args>(args)...);
        case gpufmt::Format::A8B8G8R8_SNORM_PACK32:
            return std::invoke(T<gpufmt::Format::A8B8G8R8_SNORM_PACK32>(), std::forward<Args>(args)...);
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::A8B8G8R8_USCALED_PACK32:
            return std::invoke(T<gpufmt::Format::A8B8G8R8_USCALED_PACK32>(), std::forward<Args>(args)...);
        case gpufmt::Format::A8B8G8R8_SSCALED_PACK32:
            return std::invoke(T<gpufmt::Format::A8B8G8R8_SSCALED_PACK32>(), std::forward<Args>(args)...);
#endif
        case gpufmt::Format::A8B8G8R8_UINT_PACK32:
            return std::invoke(T<gpufmt::Format::A8B8G8R8_UINT_PACK32>(), std::forward<Args>(args)...);
        case gpufmt::Format::A8B8G8R8_SINT_PACK32:
            return std::invoke(T<gpufmt::Format::A8B8G8R8_SINT_PACK32>(), std::forward<Args>(args)...);
        case gpufmt::Format::A8B8G8R8_SRGB_PACK32:
            return std::invoke(T<gpufmt::Format::A8B8G8R8_SRGB_PACK32>(), std::forward<Args>(args)...);
        case gpufmt::Format::A2R10G10B10_UNORM_PACK32:
            return std::invoke(T<gpufmt::Format::A2R10G10B10_UNORM_PACK32>(), std::forward<Args>(args)...);
        case gpufmt::Format::A2R10G10B10_SNORM_PACK32:
            return std::invoke(T<gpufmt::Format::A2R10G10B10_SNORM_PACK32>(), std::forward<Args>(args)...);
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::A2R10G10B10_USCALED_PACK32:
            return std::invoke(T<gpufmt::Format::A2R10G10B10_USCALED_PACK32>(), std::forward<Args>(args)...);
        case gpufmt::Format::A2R10G10B10_SSCALED_PACK32:
            return std::invoke(T<gpufmt::Format::A2R10G10B10_SSCALED_PACK32>(), std::forward<Args>(args)...);
#endif
        case gpufmt::Format::A2R10G10B10_UINT_PACK32:
            return std::invoke(T<gpufmt::Format::A2R10G10B10_UINT_PACK32>(), std::forward<Args>(args)...);
        case gpufmt::Format::A2R10G10B10_SINT_PACK32:
            return std::invoke(T<gpufmt::Format::A2R10G10B10_SINT_PACK32>(), std::forward<Args>(args)...);
        case gpufmt::Format::A2B10G10R10_UNORM_PACK32:
            return std::invoke(T<gpufmt::Format::A2B10G10R10_UNORM_PACK32>(), std::forward<Args>(args)...);
        case gpufmt::Format::A2B10G10R10_SNORM_PACK32:
            return std::invoke(T<gpufmt::Format::A2B10G10R10_SNORM_PACK32>(), std::forward<Args>(args)...);
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::A2B10G10R10_USCALED_PACK32:
            return std::invoke(T<gpufmt::Format::A2B10G10R10_USCALED_PACK32>(), std::forward<Args>(args)...);
        case gpufmt::Format::A2B10G10R10_SSCALED_PACK32:
            return std::invoke(T<gpufmt::Format::A2B10G10R10_SSCALED_PACK32>(), std::forward<Args>(args)...);
#endif
        case gpufmt::Format::A2B10G10R10_UINT_PACK32:
            return std::invoke(T<gpufmt::Format::A2B10G10R10_UINT_PACK32>(), std::forward<Args>(args)...);
        case gpufmt::Format::A2B10G10R10_SINT_PACK32:
            return std::invoke(T<gpufmt::Format::A2B10G10R10_SINT_PACK32>(), std::forward<Args>(args)...);
        case gpufmt::Format::R16_UNORM:
            return std::invoke(T<gpufmt::Format::R16_UNORM>(), std::forward<Args>(args)...);
        case gpufmt::Format::R16_SNORM:
            return std::invoke(T<gpufmt::Format::R16_SNORM>(), std::forward<Args>(args)...);
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::R16_USCALED:
            return std::invoke(T<gpufmt::Format::R16_USCALED>(), std::forward<Args>(args)...);
        case gpufmt::Format::R16_SSCALED:
            return std::invoke(T<gpufmt::Format::R16_SSCALED>(), std::forward<Args>(args)...);
#endif
        case gpufmt::Format::R16_UINT:
            return std::invoke(T<gpufmt::Format::R16_UINT>(), std::forward<Args>(args)...);
        case gpufmt::Format::R16_SINT:
            return std::invoke(T<gpufmt::Format::R16_SINT>(), std::forward<Args>(args)...);
        case gpufmt::Format::R16_SFLOAT:
            return std::invoke(T<gpufmt::Format::R16_SFLOAT>(), std::forward<Args>(args)...);
        case gpufmt::Format::R16G16_UNORM:
            return std::invoke(T<gpufmt::Format::R16G16_UNORM>(), std::forward<Args>(args)...);
        case gpufmt::Format::R16G16_SNORM:
            return std::invoke(T<gpufmt::Format::R16G16_SNORM>(), std::forward<Args>(args)...);
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::R16G16_USCALED:
            return std::invoke(T<gpufmt::Format::R16G16_USCALED>(), std::forward<Args>(args)...);
        case gpufmt::Format::R16G16_SSCALED:
            return std::invoke(T<gpufmt::Format::R16G16_SSCALED>(), std::forward<Args>(args)...);
#endif
        case gpufmt::Format::R16G16_UINT:
            return std::invoke(T<gpufmt::Format::R16G16_UINT>(), std::forward<Args>(args)...);
        case gpufmt::Format::R16G16_SINT:
            return std::invoke(T<gpufmt::Format::R16G16_SINT>(), std::forward<Args>(args)...);
        case gpufmt::Format::R16G16_SFLOAT:
            return std::invoke(T<gpufmt::Format::R16G16_SFLOAT>(), std::forward<Args>(args)...);
        case gpufmt::Format::R16G16B16_UNORM:
            return std::invoke(T<gpufmt::Format::R16G16B16_UNORM>(), std::forward<Args>(args)...);
        case gpufmt::Format::R16G16B16_SNORM:
            return std::invoke(T<gpufmt::Format::R16G16B16_SNORM>(), std::forward<Args>(args)...);
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::R16G16B16_USCALED:
            return std::invoke(T<gpufmt::Format::R16G16B16_USCALED>(), std::forward<Args>(args)...);
        case gpufmt::Format::R16G16B16_SSCALED:
            return std::invoke(T<gpufmt::Format::R16G16B16_SSCALED>(), std::forward<Args>(args)...);
#endif
        case gpufmt::Format::R16G16B16_UINT:
            return std::invoke(T<gpufmt::Format::R16G16B16_UINT>(), std::forward<Args>(args)...);
        case gpufmt::Format::R16G16B16_SINT:
            return std::invoke(T<gpufmt::Format::R16G16B16_SINT>(), std::forward<Args>(args)...);
        case gpufmt::Format::R16G16B16_SFLOAT:
            return std::invoke(T<gpufmt::Format::R16G16B16_SFLOAT>(), std::forward<Args>(args)...);
        case gpufmt::Format::R16G16B16A16_UNORM:
            return std::invoke(T<gpufmt::Format::R16G16B16A16_UNORM>(), std::forward<Args>(args)...);
        case gpufmt::Format::R16G16B16A16_SNORM:
            return std::invoke(T<gpufmt::Format::R16G16B16A16_SNORM>(), std::forward<Args>(args)...);
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::R16G16B16A16_USCALED:
            return std::invoke(T<gpufmt::Format::R16G16B16A16_USCALED>(), std::forward<Args>(args)...);
        case gpufmt::Format::R16G16B16A16_SSCALED:
            return std::invoke(T<gpufmt::Format::R16G16B16A16_SSCALED>(), std::forward<Args>(args)...);
#endif
        case gpufmt::Format::R16G16B16A16_UINT:
            return std::invoke(T<gpufmt::Format::R16G16B16A16_UINT>(), std::forward<Args>(args)...);
        case gpufmt::Format::R16G16B16A16_SINT:
            return std::invoke(T<gpufmt::Format::R16G16B16A16_SINT>(), std::forward<Args>(args)...);
        case gpufmt::Format::R16G16B16A16_SFLOAT:
            return std::invoke(T<gpufmt::Format::R16G16B16A16_SFLOAT>(), std::forward<Args>(args)...);
        case gpufmt::Format::R32_UINT:
            return std::invoke(T<gpufmt::Format::R32_UINT>(), std::forward<Args>(args)...);
        case gpufmt::Format::R32_SINT:
            return std::invoke(T<gpufmt::Format::R32_SINT>(), std::forward<Args>(args)...);
        case gpufmt::Format::R32_SFLOAT:
            return std::invoke(T<gpufmt::Format::R32_SFLOAT>(), std::forward<Args>(args)...);
        case gpufmt::Format::R32G32_UINT:
            return std::invoke(T<gpufmt::Format::R32G32_UINT>(), std::forward<Args>(args)...);
        case gpufmt::Format::R32G32_SINT:
            return std::invoke(T<gpufmt::Format::R32G32_SINT>(), std::forward<Args>(args)...);
        case gpufmt::Format::R32G32_SFLOAT:
            return std::invoke(T<gpufmt::Format::R32G32_SFLOAT>(), std::forward<Args>(args)...);
        case gpufmt::Format::R32G32B32_UINT:
            return std::invoke(T<gpufmt::Format::R32G32B32_UINT>(), std::forward<Args>(args)...);
        case gpufmt::Format::R32G32B32_SINT:
            return std::invoke(T<gpufmt::Format::R32G32B32_SINT>(), std::forward<Args>(args)...);
        case gpufmt::Format::R32G32B32_SFLOAT:
            return std::invoke(T<gpufmt::Format::R32G32B32_SFLOAT>(), std::forward<Args>(args)...);
        case gpufmt::Format::R32G32B32A32_UINT:
            return std::invoke(T<gpufmt::Format::R32G32B32A32_UINT>(), std::forward<Args>(args)...);
        case gpufmt::Format::R32G32B32A32_SINT:
            return std::invoke(T<gpufmt::Format::R32G32B32A32_SINT>(), std::forward<Args>(args)...);
        case gpufmt::Format::R32G32B32A32_SFLOAT:
            return std::invoke(T<gpufmt::Format::R32G32B32A32_SFLOAT>(), std::forward<Args>(args)...);
#ifndef GF_EXCLUDE_64BIT_FORMATS
        case gpufmt::Format::R64_UINT:
            return std::invoke(T<gpufmt::Format::R64_UINT>(), std::forward<Args>(args)...);
        case gpufmt::Format::R64_SINT:
            return std::invoke(T<gpufmt::Format::R64_SINT>(), std::forward<Args>(args)...);
        case gpufmt::Format::R64_SFLOAT:
            return std::invoke(T<gpufmt::Format::R64_SFLOAT>(), std::forward<Args>(args)...);
        case gpufmt::Format::R64G64_UINT:
            return std::invoke(T<gpufmt::Format::R64G64_UINT>(), std::forward<Args>(args)...);
        case gpufmt::Format::R64G64_SINT:
            return std::invoke(T<gpufmt::Format::R64G64_SINT>(), std::forward<Args>(args)...);
        case gpufmt::Format::R64G64_SFLOAT:
            return std::invoke(T<gpufmt::Format::R64G64_SFLOAT>(), std::forward<Args>(args)...);
        case gpufmt::Format::R64G64B64_UINT:
            return std::invoke(T<gpufmt::Format::R64G64B64_UINT>(), std::forward<Args>(args)...);
        case gpufmt::Format::R64G64B64_SINT:
            return std::invoke(T<gpufmt::Format::R64G64B64_SINT>(), std::forward<Args>(args)...);
        case gpufmt::Format::R64G64B64_SFLOAT:
            return std::invoke(T<gpufmt::Format::R64G64B64_SFLOAT>(), std::forward<Args>(args)...);
        case gpufmt::Format::R64G64B64A64_UINT:
            return std::invoke(T<gpufmt::Format::R64G64B64A64_UINT>(), std::forward<Args>(args)...);
        case gpufmt::Format::R64G64B64A64_SINT:
            return std::invoke(T<gpufmt::Format::R64G64B64A64_SINT>(), std::forward<Args>(args)...);
        case gpufmt::Format::R64G64B64A64_SFLOAT:
            return std::invoke(T<gpufmt::Format::R64G64B64A64_SFLOAT>(), std::forward<Args>(args)...);
#endif // GF_EXCLUDE_64BIT_FORMATS
        case gpufmt::Format::B10G11R11_UFLOAT_PACK32:
            return std::invoke(T<gpufmt::Format::B10G11R11_UFLOAT_PACK32>(), std::forward<Args>(args)...);
        case gpufmt::Format::E5B9G9R9_UFLOAT_PACK32:
            return std::invoke(T<gpufmt::Format::E5B9G9R9_UFLOAT_PACK32>(), std::forward<Args>(args)...);
        case gpufmt::Format::D16_UNORM:
            return std::invoke(T<gpufmt::Format::D16_UNORM>(), std::forward<Args>(args)...);
        case gpufmt::Format::X8_D24_UNORM_PACK32:
            return std::invoke(T<gpufmt::Format::X8_D24_UNORM_PACK32>(), std::forward<Args>(args)...);
        case gpufmt::Format::D32_SFLOAT:
            return std::invoke(T<gpufmt::Format::D32_SFLOAT>(), std::forward<Args>(args)...);
        case gpufmt::Format::S8_UINT:
            return std::invoke(T<gpufmt::Format::S8_UINT>(), std::forward<Args>(args)...);
        case gpufmt::Format::D16_UNORM_S8_UINT:
            return std::invoke(T<gpufmt::Format::D16_UNORM_S8_UINT>(), std::forward<Args>(args)...);
        case gpufmt::Format::D24_UNORM_S8_UINT:
            return std::invoke(T<gpufmt::Format::D24_UNORM_S8_UINT>(), std::forward<Args>(args)...);
        case gpufmt::Format::D32_SFLOAT_S8_UINT:
            return std::invoke(T<gpufmt::Format::D32_SFLOAT_S8_UINT>(), std::forward<Args>(args)...);
#ifndef GF_EXCLUDE_COMPRESSED_FORMATS
#ifndef GF_EXCLUDE_BC_COMPRESSED_FORMATS
        case gpufmt::Format::BC1_RGB_UNORM_BLOCK:
            return std::invoke(T<gpufmt::Format::BC1_RGB_UNORM_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::BC1_RGB_SRGB_BLOCK:
            return std::invoke(T<gpufmt::Format::BC1_RGB_SRGB_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::BC1_RGBA_UNORM_BLOCK:
            return std::invoke(T<gpufmt::Format::BC1_RGBA_UNORM_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::BC1_RGBA_SRGB_BLOCK:
            return std::invoke(T<gpufmt::Format::BC1_RGBA_SRGB_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::BC2_UNORM_BLOCK:
            return std::invoke(T<gpufmt::Format::BC2_UNORM_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::BC2_SRGB_BLOCK:
            return std::invoke(T<gpufmt::Format::BC2_SRGB_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::BC3_UNORM_BLOCK:
            return std::invoke(T<gpufmt::Format::BC3_UNORM_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::BC3_SRGB_BLOCK:
            return std::invoke(T<gpufmt::Format::BC3_SRGB_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::BC4_UNORM_BLOCK:
            return std::invoke(T<gpufmt::Format::BC4_UNORM_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::BC4_SNORM_BLOCK:
            return std::invoke(T<gpufmt::Format::BC4_SNORM_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::BC5_UNORM_BLOCK:
            return std::invoke(T<gpufmt::Format::BC5_UNORM_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::BC5_SNORM_BLOCK:
            return std::invoke(T<gpufmt::Format::BC5_SNORM_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::BC6H_UFLOAT_BLOCK:
            return std::invoke(T<gpufmt::Format::BC6H_UFLOAT_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::BC6H_SFLOAT_BLOCK:
            return std::invoke(T<gpufmt::Format::BC6H_SFLOAT_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::BC7_UNORM_BLOCK:
            return std::invoke(T<gpufmt::Format::BC7_UNORM_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::BC7_SRGB_BLOCK:
            return std::invoke(T<gpufmt::Format::BC7_SRGB_BLOCK>(), std::forward<Args>(args)...);
#endif //GF_EXCLUDE_BC_COMPRESSED_FORMATS
#ifndef GF_EXCLUDE_ETC_COMPRESSED_FORMATS
        case gpufmt::Format::ETC2_R8G8B8_UNORM_BLOCK:
            return std::invoke(T<gpufmt::Format::ETC2_R8G8B8_UNORM_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::ETC2_R8G8B8_SRGB_BLOCK:
            return std::invoke(T<gpufmt::Format::ETC2_R8G8B8_SRGB_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::ETC2_R8G8B8A1_UNORM_BLOCK:
            return std::invoke(T<gpufmt::Format::ETC2_R8G8B8A1_UNORM_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::ETC2_R8G8B8A1_SRGB_BLOCK:
            return std::invoke(T<gpufmt::Format::ETC2_R8G8B8A1_SRGB_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::ETC2_R8G8B8A8_UNORM_BLOCK:
            return std::invoke(T<gpufmt::Format::ETC2_R8G8B8A8_UNORM_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::ETC2_R8G8B8A8_SRGB_BLOCK:
            return std::invoke(T<gpufmt::Format::ETC2_R8G8B8A8_SRGB_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::EAC_R11_UNORM_BLOCK:
            return std::invoke(T<gpufmt::Format::EAC_R11_UNORM_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::EAC_R11_SNORM_BLOCK:
            return std::invoke(T<gpufmt::Format::EAC_R11_SNORM_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::EAC_R11G11_UNORM_BLOCK:
            return std::invoke(T<gpufmt::Format::EAC_R11G11_UNORM_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::EAC_R11G11_SNORM_BLOCK:
            return std::invoke(T<gpufmt::Format::EAC_R11G11_SNORM_BLOCK>(), std::forward<Args>(args)...);
#endif //GF_EXCLUDE_ETC_COMPRESSED_FORMATS
#ifndef GF_EXCLUDE_ASTC_COMPRESSED_FORMATS
        case gpufmt::Format::ASTC_4x4_UNORM_BLOCK:
            return std::invoke(T<gpufmt::Format::ASTC_4x4_UNORM_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::ASTC_4x4_SRGB_BLOCK:
            return std::invoke(T<gpufmt::Format::ASTC_4x4_SRGB_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::ASTC_5x4_UNORM_BLOCK:
            return std::invoke(T<gpufmt::Format::ASTC_5x4_UNORM_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::ASTC_5x4_SRGB_BLOCK:
            return std::invoke(T<gpufmt::Format::ASTC_5x4_SRGB_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::ASTC_5x5_UNORM_BLOCK:
            return std::invoke(T<gpufmt::Format::ASTC_5x5_UNORM_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::ASTC_5x5_SRGB_BLOCK:
            return std::invoke(T<gpufmt::Format::ASTC_5x5_SRGB_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::ASTC_6x5_UNORM_BLOCK:
            return std::invoke(T<gpufmt::Format::ASTC_6x5_UNORM_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::ASTC_6x5_SRGB_BLOCK:
            return std::invoke(T<gpufmt::Format::ASTC_6x5_SRGB_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::ASTC_6x6_UNORM_BLOCK:
            return std::invoke(T<gpufmt::Format::ASTC_6x6_UNORM_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::ASTC_6x6_SRGB_BLOCK:
            return std::invoke(T<gpufmt::Format::ASTC_6x6_SRGB_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::ASTC_8x5_UNORM_BLOCK:
            return std::invoke(T<gpufmt::Format::ASTC_8x5_UNORM_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::ASTC_8x5_SRGB_BLOCK:
            return std::invoke(T<gpufmt::Format::ASTC_8x5_SRGB_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::ASTC_8x6_UNORM_BLOCK:
            return std::invoke(T<gpufmt::Format::ASTC_8x6_UNORM_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::ASTC_8x6_SRGB_BLOCK:
            return std::invoke(T<gpufmt::Format::ASTC_8x6_SRGB_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::ASTC_8x8_UNORM_BLOCK:
            return std::invoke(T<gpufmt::Format::ASTC_8x8_UNORM_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::ASTC_8x8_SRGB_BLOCK:
            return std::invoke(T<gpufmt::Format::ASTC_8x8_SRGB_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::ASTC_10x5_UNORM_BLOCK:
            return std::invoke(T<gpufmt::Format::ASTC_10x5_UNORM_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::ASTC_10x5_SRGB_BLOCK:
            return std::invoke(T<gpufmt::Format::ASTC_10x5_SRGB_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::ASTC_10x6_UNORM_BLOCK:
            return std::invoke(T<gpufmt::Format::ASTC_10x6_UNORM_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::ASTC_10x6_SRGB_BLOCK:
            return std::invoke(T<gpufmt::Format::ASTC_10x6_SRGB_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::ASTC_10x8_UNORM_BLOCK:
            return std::invoke(T<gpufmt::Format::ASTC_10x8_UNORM_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::ASTC_10x8_SRGB_BLOCK:
            return std::invoke(T<gpufmt::Format::ASTC_10x8_SRGB_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::ASTC_10x10_UNORM_BLOCK:
            return std::invoke(T<gpufmt::Format::ASTC_10x10_UNORM_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::ASTC_10x10_SRGB_BLOCK:
            return std::invoke(T<gpufmt::Format::ASTC_10x10_SRGB_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::ASTC_12x10_UNORM_BLOCK:
            return std::invoke(T<gpufmt::Format::ASTC_12x10_UNORM_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::ASTC_12x10_SRGB_BLOCK:
            return std::invoke(T<gpufmt::Format::ASTC_12x10_SRGB_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::ASTC_12x12_UNORM_BLOCK:
            return std::invoke(T<gpufmt::Format::ASTC_12x12_UNORM_BLOCK>(), std::forward<Args>(args)...);
        case gpufmt::Format::ASTC_12x12_SRGB_BLOCK:
            return std::invoke(T<gpufmt::Format::ASTC_12x12_SRGB_BLOCK>(), std::forward<Args>(args)...);
#endif //GF_EXCLUDE_ASTC_COMPRESSED_FORMATS
#ifndef GF_EXCLUDE_PVRTC_COMPRESSED_FORMATS
        case gpufmt::Format::PVRTC1_2BPP_UNORM_BLOCK_IMG:
            return std::invoke(T<gpufmt::Format::PVRTC1_2BPP_UNORM_BLOCK_IMG>(), std::forward<Args>(args)...);
        case gpufmt::Format::PVRTC1_4BPP_UNORM_BLOCK_IMG:
            return std::invoke(T<gpufmt::Format::PVRTC1_4BPP_UNORM_BLOCK_IMG>(), std::forward<Args>(args)...);
        case gpufmt::Format::PVRTC2_2BPP_UNORM_BLOCK_IMG:
            return std::invoke(T<gpufmt::Format::PVRTC2_2BPP_UNORM_BLOCK_IMG>(), std::forward<Args>(args)...);
        case gpufmt::Format::PVRTC2_4BPP_UNORM_BLOCK_IMG:
            return std::invoke(T<gpufmt::Format::PVRTC2_4BPP_UNORM_BLOCK_IMG>(), std::forward<Args>(args)...);
        case gpufmt::Format::PVRTC1_2BPP_SRGB_BLOCK_IMG:
            return std::invoke(T<gpufmt::Format::PVRTC1_2BPP_SRGB_BLOCK_IMG>(), std::forward<Args>(args)...);
        case gpufmt::Format::PVRTC1_4BPP_SRGB_BLOCK_IMG:
            return std::invoke(T<gpufmt::Format::PVRTC1_4BPP_SRGB_BLOCK_IMG>(), std::forward<Args>(args)...);
        case gpufmt::Format::PVRTC2_2BPP_SRGB_BLOCK_IMG:
            return std::invoke(T<gpufmt::Format::PVRTC2_2BPP_SRGB_BLOCK_IMG>(), std::forward<Args>(args)...);
        case gpufmt::Format::PVRTC2_4BPP_SRGB_BLOCK_IMG:
            return std::invoke(T<gpufmt::Format::PVRTC2_4BPP_SRGB_BLOCK_IMG>(), std::forward<Args>(args)...);
#endif //GF_EXCLUDE_PVRTC_COMPRESSED_FORMATS
#endif //GF_EXCLUDE_COMPRESSED_FORMATS
        case gpufmt::Format::UNDEFINED:
            [[fallthrough]];
        default:
            return std::invoke(T<gpufmt::Format::UNDEFINED>(), std::forward<Args>(args)...);
        }
    }
}