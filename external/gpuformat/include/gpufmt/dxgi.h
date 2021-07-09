#pragma once

#include <gpufmt/format.h>

#if __has_include(<dxgi.h>)
#include <dxgi.h>
#else
typedef enum DXGI_FORMAT
{
    DXGI_FORMAT_UNKNOWN = 0,
    DXGI_FORMAT_R32G32B32A32_TYPELESS = 1,
    DXGI_FORMAT_R32G32B32A32_FLOAT = 2,
    DXGI_FORMAT_R32G32B32A32_UINT = 3,
    DXGI_FORMAT_R32G32B32A32_SINT = 4,
    DXGI_FORMAT_R32G32B32_TYPELESS = 5,
    DXGI_FORMAT_R32G32B32_FLOAT = 6,
    DXGI_FORMAT_R32G32B32_UINT = 7,
    DXGI_FORMAT_R32G32B32_SINT = 8,
    DXGI_FORMAT_R16G16B16A16_TYPELESS = 9,
    DXGI_FORMAT_R16G16B16A16_FLOAT = 10,
    DXGI_FORMAT_R16G16B16A16_UNORM = 11,
    DXGI_FORMAT_R16G16B16A16_UINT = 12,
    DXGI_FORMAT_R16G16B16A16_SNORM = 13,
    DXGI_FORMAT_R16G16B16A16_SINT = 14,
    DXGI_FORMAT_R32G32_TYPELESS = 15,
    DXGI_FORMAT_R32G32_FLOAT = 16,
    DXGI_FORMAT_R32G32_UINT = 17,
    DXGI_FORMAT_R32G32_SINT = 18,
    DXGI_FORMAT_R32G8X24_TYPELESS = 19,
    DXGI_FORMAT_D32_FLOAT_S8X24_UINT = 20,
    DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS = 21,
    DXGI_FORMAT_X32_TYPELESS_G8X24_UINT = 22,
    DXGI_FORMAT_R10G10B10A2_TYPELESS = 23,
    DXGI_FORMAT_R10G10B10A2_UNORM = 24,
    DXGI_FORMAT_R10G10B10A2_UINT = 25,
    DXGI_FORMAT_R11G11B10_FLOAT = 26,
    DXGI_FORMAT_R8G8B8A8_TYPELESS = 27,
    DXGI_FORMAT_R8G8B8A8_UNORM = 28,
    DXGI_FORMAT_R8G8B8A8_UNORM_SRGB = 29,
    DXGI_FORMAT_R8G8B8A8_UINT = 30,
    DXGI_FORMAT_R8G8B8A8_SNORM = 31,
    DXGI_FORMAT_R8G8B8A8_SINT = 32,
    DXGI_FORMAT_R16G16_TYPELESS = 33,
    DXGI_FORMAT_R16G16_FLOAT = 34,
    DXGI_FORMAT_R16G16_UNORM = 35,
    DXGI_FORMAT_R16G16_UINT = 36,
    DXGI_FORMAT_R16G16_SNORM = 37,
    DXGI_FORMAT_R16G16_SINT = 38,
    DXGI_FORMAT_R32_TYPELESS = 39,
    DXGI_FORMAT_D32_FLOAT = 40,
    DXGI_FORMAT_R32_FLOAT = 41,
    DXGI_FORMAT_R32_UINT = 42,
    DXGI_FORMAT_R32_SINT = 43,
    DXGI_FORMAT_R24G8_TYPELESS = 44,
    DXGI_FORMAT_D24_UNORM_S8_UINT = 45,
    DXGI_FORMAT_R24_UNORM_X8_TYPELESS = 46,
    DXGI_FORMAT_X24_TYPELESS_G8_UINT = 47,
    DXGI_FORMAT_R8G8_TYPELESS = 48,
    DXGI_FORMAT_R8G8_UNORM = 49,
    DXGI_FORMAT_R8G8_UINT = 50,
    DXGI_FORMAT_R8G8_SNORM = 51,
    DXGI_FORMAT_R8G8_SINT = 52,
    DXGI_FORMAT_R16_TYPELESS = 53,
    DXGI_FORMAT_R16_FLOAT = 54,
    DXGI_FORMAT_D16_UNORM = 55,
    DXGI_FORMAT_R16_UNORM = 56,
    DXGI_FORMAT_R16_UINT = 57,
    DXGI_FORMAT_R16_SNORM = 58,
    DXGI_FORMAT_R16_SINT = 59,
    DXGI_FORMAT_R8_TYPELESS = 60,
    DXGI_FORMAT_R8_UNORM = 61,
    DXGI_FORMAT_R8_UINT = 62,
    DXGI_FORMAT_R8_SNORM = 63,
    DXGI_FORMAT_R8_SINT = 64,
    DXGI_FORMAT_A8_UNORM = 65,
    DXGI_FORMAT_R1_UNORM = 66,
    DXGI_FORMAT_R9G9B9E5_SHAREDEXP = 67,
    DXGI_FORMAT_R8G8_B8G8_UNORM = 68,
    DXGI_FORMAT_G8R8_G8B8_UNORM = 69,
    DXGI_FORMAT_BC1_TYPELESS = 70,
    DXGI_FORMAT_BC1_UNORM = 71,
    DXGI_FORMAT_BC1_UNORM_SRGB = 72,
    DXGI_FORMAT_BC2_TYPELESS = 73,
    DXGI_FORMAT_BC2_UNORM = 74,
    DXGI_FORMAT_BC2_UNORM_SRGB = 75,
    DXGI_FORMAT_BC3_TYPELESS = 76,
    DXGI_FORMAT_BC3_UNORM = 77,
    DXGI_FORMAT_BC3_UNORM_SRGB = 78,
    DXGI_FORMAT_BC4_TYPELESS = 79,
    DXGI_FORMAT_BC4_UNORM = 80,
    DXGI_FORMAT_BC4_SNORM = 81,
    DXGI_FORMAT_BC5_TYPELESS = 82,
    DXGI_FORMAT_BC5_UNORM = 83,
    DXGI_FORMAT_BC5_SNORM = 84,
    DXGI_FORMAT_B5G6R5_UNORM = 85,
    DXGI_FORMAT_B5G5R5A1_UNORM = 86,
    DXGI_FORMAT_B8G8R8A8_UNORM = 87,
    DXGI_FORMAT_B8G8R8X8_UNORM = 88,
    DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM = 89,
    DXGI_FORMAT_B8G8R8A8_TYPELESS = 90,
    DXGI_FORMAT_B8G8R8A8_UNORM_SRGB = 91,
    DXGI_FORMAT_B8G8R8X8_TYPELESS = 92,
    DXGI_FORMAT_B8G8R8X8_UNORM_SRGB = 93,
    DXGI_FORMAT_BC6H_TYPELESS = 94,
    DXGI_FORMAT_BC6H_UF16 = 95,
    DXGI_FORMAT_BC6H_SF16 = 96,
    DXGI_FORMAT_BC7_TYPELESS = 97,
    DXGI_FORMAT_BC7_UNORM = 98,
    DXGI_FORMAT_BC7_UNORM_SRGB = 99,
    DXGI_FORMAT_AYUV = 100,
    DXGI_FORMAT_Y410 = 101,
    DXGI_FORMAT_Y416 = 102,
    DXGI_FORMAT_NV12 = 103,
    DXGI_FORMAT_P010 = 104,
    DXGI_FORMAT_P016 = 105,
    DXGI_FORMAT_420_OPAQUE = 106,
    DXGI_FORMAT_YUY2 = 107,
    DXGI_FORMAT_Y210 = 108,
    DXGI_FORMAT_Y216 = 109,
    DXGI_FORMAT_NV11 = 110,
    DXGI_FORMAT_AI44 = 111,
    DXGI_FORMAT_IA44 = 112,
    DXGI_FORMAT_P8 = 113,
    DXGI_FORMAT_A8P8 = 114,
    DXGI_FORMAT_B4G4R4A4_UNORM = 115,

    DXGI_FORMAT_P208 = 130,
    DXGI_FORMAT_V208 = 131,
    DXGI_FORMAT_V408 = 132,


    DXGI_FORMAT_FORCE_UINT = 0xffffffff
} DXGI_FORMAT;
#endif

#include <optional>
#include <string_view>

// Byte Order(LSB / MSB)
// Most formats have byte-aligned components, and the components are in C-array
// order (the least address comes first). For those formats that don't have
// power-of-2-aligned components, the first named component is in the
// least-significant bits. 
// Source: https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format
//
// Examples:
//
// R4G4B4A4
// MSB                                           LSB
// |15|14|13|12|11|10|09|08|07|06|05|04|03|02|01|00| Bits
// |A3|A2|A1|A0|B3|B2|B1|B0|G3|G2|G1|G0|R3|R2|R1|R0| Texel data
//
// B4G4R4A4
// MSB                                           LSB
// |15|14|13|12|11|10|09|08|07|06|05|04|03|02|01|00| Bits
// |A3|A2|A1|A0|R3|R2|R1|R0|G3|G2|G1|G0|B3|B2|B1|B0| Texel data

namespace gpufmt::dxgi {
    struct FormatConversion {
        std::optional<DXGI_FORMAT> exact;
        std::optional<DXGI_FORMAT> closest;

        [[nodiscard]]
        constexpr operator bool() const noexcept {
            return exact || closest;
        }
    };

    [[nodiscard]]
    static constexpr FormatConversion translateFormat(Format format) noexcept {
        FormatConversion conversion;

        switch(format)
        {
        case gpufmt::Format::UNDEFINED:
            conversion.exact = DXGI_FORMAT_UNKNOWN;
            break;
        case gpufmt::Format::R4G4_UNORM_PACK8:
            conversion.closest = DXGI_FORMAT_B4G4R4A4_UNORM;
            break;
        case gpufmt::Format::R4G4B4A4_UNORM_PACK16:
            conversion.closest = DXGI_FORMAT_B4G4R4A4_UNORM;
            break;
        case gpufmt::Format::B4G4R4A4_UNORM_PACK16:
            conversion.closest = DXGI_FORMAT_B4G4R4A4_UNORM;
            break;
        case gpufmt::Format::A4R4G4B4_UNORM_PACK16:
            conversion.exact = DXGI_FORMAT_B4G4R4A4_UNORM;
            break;
        case gpufmt::Format::R5G6B5_UNORM_PACK16:
            conversion.exact = DXGI_FORMAT_B5G6R5_UNORM;
            break;
        case gpufmt::Format::B5G6R5_UNORM_PACK16:
            conversion.closest = DXGI_FORMAT_B5G6R5_UNORM;
            break;
        case gpufmt::Format::R5G5B5A1_UNORM_PACK16:
            conversion.closest = DXGI_FORMAT_B5G5R5A1_UNORM;
            break;
        case gpufmt::Format::B5G5R5A1_UNORM_PACK16:
            conversion.closest = DXGI_FORMAT_B5G5R5A1_UNORM;
            break;
        case gpufmt::Format::A1R5G5B5_UNORM_PACK16:
            conversion.exact = DXGI_FORMAT_B5G5R5A1_UNORM;
            break;
        case gpufmt::Format::R8_UNORM:
            conversion.exact = DXGI_FORMAT_R8_UNORM;
            break;
        case gpufmt::Format::R8_SNORM:
            conversion.exact = DXGI_FORMAT_R8_SNORM;
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::R8_USCALED:
            conversion.closest = DXGI_FORMAT_R8_UINT;
            break;
        case gpufmt::Format::R8_SSCALED:
            conversion.closest = DXGI_FORMAT_R8_SINT;
            break;
#endif
        case gpufmt::Format::R8_UINT:
            conversion.exact = DXGI_FORMAT_R8_UINT;
            break;
        case gpufmt::Format::R8_SINT:
            conversion.exact = DXGI_FORMAT_R8_SINT;
            break;
        case gpufmt::Format::R8_SRGB:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            break;
        case gpufmt::Format::A8_UNORM:
            conversion.exact = DXGI_FORMAT_A8_UNORM;
            break;
        case gpufmt::Format::R8G8_UNORM:
            conversion.exact = DXGI_FORMAT_R8G8_UNORM;
            break;
        case gpufmt::Format::R8G8_SNORM:
            conversion.exact = DXGI_FORMAT_R8G8_SNORM;
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::R8G8_USCALED:
            conversion.closest = DXGI_FORMAT_R8G8_UINT;
            break;
        case gpufmt::Format::R8G8_SSCALED:
            conversion.closest = DXGI_FORMAT_R8G8_SINT;
            break;
#endif
        case gpufmt::Format::R8G8_UINT:
            conversion.exact = DXGI_FORMAT_R8G8_UINT;
            break;
        case gpufmt::Format::R8G8_SINT:
            conversion.exact = DXGI_FORMAT_R8G8_SINT;
            break;
        case gpufmt::Format::R8G8_SRGB:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            break;
        case gpufmt::Format::R8G8B8_UNORM:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM;
            break;
        case gpufmt::Format::R8G8B8_SNORM:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_SNORM;
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::R8G8B8_USCALED:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UINT;
            break;
        case gpufmt::Format::R8G8B8_SSCALED:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_SINT;
            break;
#endif
        case gpufmt::Format::R8G8B8_UINT:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UINT;
            break;
        case gpufmt::Format::R8G8B8_SINT:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_SINT;
            break;
        case gpufmt::Format::R8G8B8_SRGB:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            break;
        case gpufmt::Format::B8G8R8_UNORM:
            conversion.closest = DXGI_FORMAT_B8G8R8X8_UNORM;
            break;
        case gpufmt::Format::B8G8R8_SNORM:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_SNORM;
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::B8G8R8_USCALED:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UINT;
            break;
        case gpufmt::Format::B8G8R8_SSCALED:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_SINT;
            break;
#endif
        case gpufmt::Format::B8G8R8_UINT:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UINT;
            break;
        case gpufmt::Format::B8G8R8_SINT:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_SINT;
            break;
        case gpufmt::Format::B8G8R8_SRGB:
            conversion.closest = DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
            break;
        case gpufmt::Format::R8G8B8A8_UNORM:
            conversion.exact = DXGI_FORMAT_R8G8B8A8_UNORM;
            break;
        case gpufmt::Format::R8G8B8A8_SNORM:
            conversion.exact = DXGI_FORMAT_R8G8B8A8_SNORM;
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::R8G8B8A8_USCALED:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UINT;
            break;
        case gpufmt::Format::R8G8B8A8_SSCALED:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_SINT;
            break;
#endif
        case gpufmt::Format::R8G8B8A8_UINT:
            conversion.exact = DXGI_FORMAT_R8G8B8A8_UINT;
            break;
        case gpufmt::Format::R8G8B8A8_SINT:
            conversion.exact = DXGI_FORMAT_R8G8B8A8_SINT;
            break;
        case gpufmt::Format::R8G8B8A8_SRGB:
            conversion.exact = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            break;
        case gpufmt::Format::B8G8R8A8_UNORM:
            conversion.exact = DXGI_FORMAT_B8G8R8A8_UNORM;
            break;
        case gpufmt::Format::B8G8R8A8_SNORM:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_SNORM;
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::B8G8R8A8_USCALED:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UINT;
            break;
        case gpufmt::Format::B8G8R8A8_SSCALED:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_SINT;
            break;
#endif
        case gpufmt::Format::B8G8R8A8_UINT:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UINT;
            break;
        case gpufmt::Format::B8G8R8A8_SINT:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_SINT;
            break;
        case gpufmt::Format::B8G8R8A8_SRGB:
            conversion.exact = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
            break;
        case gpufmt::Format::B8G8R8X8_UNORM:
            conversion.exact = DXGI_FORMAT_B8G8R8X8_UNORM;
            break;
        case gpufmt::Format::B8G8R8X8_SRGB:
            conversion.exact = DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
            break;
        case gpufmt::Format::A8B8G8R8_UNORM_PACK32:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM;
            break;
        case gpufmt::Format::A8B8G8R8_SNORM_PACK32:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_SNORM;
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::A8B8G8R8_USCALED_PACK32:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UINT;
            break;
        case gpufmt::Format::A8B8G8R8_SSCALED_PACK32:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_SINT;
            break;
#endif
        case gpufmt::Format::A8B8G8R8_UINT_PACK32:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UINT;
            break;
        case gpufmt::Format::A8B8G8R8_SINT_PACK32:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_SINT;
            break;
        case gpufmt::Format::A8B8G8R8_SRGB_PACK32:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            break;
        case gpufmt::Format::A2R10G10B10_UNORM_PACK32:
            conversion.closest = DXGI_FORMAT_R10G10B10A2_UNORM;
            break;
        case gpufmt::Format::A2R10G10B10_SNORM_PACK32:
            conversion.closest = DXGI_FORMAT_R16G16B16A16_SNORM;
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::A2R10G10B10_USCALED_PACK32:
            conversion.closest = DXGI_FORMAT_R10G10B10A2_UINT;
            break;
        case gpufmt::Format::A2R10G10B10_SSCALED_PACK32:
            conversion.closest = DXGI_FORMAT_R16G16B16A16_SINT;
            break;
#endif
        case gpufmt::Format::A2R10G10B10_UINT_PACK32:
            conversion.closest = DXGI_FORMAT_R10G10B10A2_UINT;
            break;
        case gpufmt::Format::A2R10G10B10_SINT_PACK32:
            conversion.closest = DXGI_FORMAT_R16G16B16A16_SINT;
            break;
        case gpufmt::Format::A2B10G10R10_UNORM_PACK32:
            conversion.exact = DXGI_FORMAT_R10G10B10A2_UNORM;
            break;
        case gpufmt::Format::A2B10G10R10_SNORM_PACK32:
            conversion.closest = DXGI_FORMAT_R16G16B16A16_SNORM;
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::A2B10G10R10_USCALED_PACK32:
            conversion.closest = DXGI_FORMAT_R10G10B10A2_UINT;
            break;
        case gpufmt::Format::A2B10G10R10_SSCALED_PACK32:
            conversion.closest = DXGI_FORMAT_R16G16B16A16_SINT;
            break;
#endif
        case gpufmt::Format::A2B10G10R10_UINT_PACK32:
            conversion.exact = DXGI_FORMAT_R10G10B10A2_UINT;
            break;
        case gpufmt::Format::A2B10G10R10_SINT_PACK32:
            conversion.closest = DXGI_FORMAT_R16G16B16A16_SINT;
            break;
        case gpufmt::Format::R16_UNORM:
            conversion.exact = DXGI_FORMAT_R16_UNORM;
            break;
        case gpufmt::Format::R16_SNORM:
            conversion.exact = DXGI_FORMAT_R16_SNORM;
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::R16_USCALED:
            conversion.closest = DXGI_FORMAT_R16_UINT;
            break;
        case gpufmt::Format::R16_SSCALED:
            conversion.closest = DXGI_FORMAT_R16_SINT;
            break;
#endif
        case gpufmt::Format::R16_UINT:
            conversion.exact = DXGI_FORMAT_R16_UINT;
            break;
        case gpufmt::Format::R16_SINT:
            conversion.exact = DXGI_FORMAT_R16_SINT;
            break;
        case gpufmt::Format::R16_SFLOAT:
            conversion.exact = DXGI_FORMAT_R16_FLOAT;
            break;
        case gpufmt::Format::R16G16_UNORM:
            conversion.exact = DXGI_FORMAT_R16G16_UNORM;
            break;
        case gpufmt::Format::R16G16_SNORM:
            conversion.exact = DXGI_FORMAT_R16G16_SNORM;
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::R16G16_USCALED:
            conversion.closest = DXGI_FORMAT_R16G16_UINT;
            break;
        case gpufmt::Format::R16G16_SSCALED:
            conversion.closest = DXGI_FORMAT_R16G16_SINT;
            break;
#endif
        case gpufmt::Format::R16G16_UINT:
            conversion.exact = DXGI_FORMAT_R16G16_UINT;
            break;
        case gpufmt::Format::R16G16_SINT:
            conversion.exact = DXGI_FORMAT_R16G16_SINT;
            break;
        case gpufmt::Format::R16G16_SFLOAT:
            conversion.exact = DXGI_FORMAT_R16G16_FLOAT;
            break;
        case gpufmt::Format::R16G16B16_UNORM:
            conversion.closest = DXGI_FORMAT_R16G16B16A16_UNORM;
            break;
        case gpufmt::Format::R16G16B16_SNORM:
            conversion.closest = DXGI_FORMAT_R16G16B16A16_SNORM;
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::R16G16B16_USCALED:
            conversion.closest = DXGI_FORMAT_R16G16B16A16_UINT;
            break;
        case gpufmt::Format::R16G16B16_SSCALED:
            conversion.closest = DXGI_FORMAT_R16G16B16A16_SINT;
            break;
#endif
        case gpufmt::Format::R16G16B16_UINT:
            conversion.closest = DXGI_FORMAT_R16G16B16A16_UINT;
            break;
        case gpufmt::Format::R16G16B16_SINT:
            conversion.closest = DXGI_FORMAT_R16G16B16A16_SINT;
            break;
        case gpufmt::Format::R16G16B16_SFLOAT:
            conversion.closest = DXGI_FORMAT_R16G16B16A16_FLOAT;
            break;
        case gpufmt::Format::R16G16B16A16_UNORM:
            conversion.exact = DXGI_FORMAT_R16G16B16A16_UNORM;
            break;
        case gpufmt::Format::R16G16B16A16_SNORM:
            conversion.exact = DXGI_FORMAT_R16G16B16A16_SNORM;
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::R16G16B16A16_USCALED:
            conversion.closest = DXGI_FORMAT_R16G16B16A16_UINT;
            break;
        case gpufmt::Format::R16G16B16A16_SSCALED:
            conversion.closest = DXGI_FORMAT_R16G16B16A16_SINT;
            break;
#endif
        case gpufmt::Format::R16G16B16A16_UINT:
            conversion.exact = DXGI_FORMAT_R16G16B16A16_UINT;
            break;
        case gpufmt::Format::R16G16B16A16_SINT:
            conversion.exact = DXGI_FORMAT_R16G16B16A16_SINT;
            break;
        case gpufmt::Format::R16G16B16A16_SFLOAT:
            conversion.exact = DXGI_FORMAT_R16G16B16A16_FLOAT;
            break;
        case gpufmt::Format::R32_UINT:
            conversion.exact = DXGI_FORMAT_R32_UINT;
            break;
        case gpufmt::Format::R32_SINT:
            conversion.exact = DXGI_FORMAT_R32_SINT;
            break;
        case gpufmt::Format::R32_SFLOAT:
            conversion.exact = DXGI_FORMAT_R32_FLOAT;
            break;
        case gpufmt::Format::R32G32_UINT:
            conversion.exact = DXGI_FORMAT_R32G32_UINT;
            break;
        case gpufmt::Format::R32G32_SINT:
            conversion.exact = DXGI_FORMAT_R32G32_SINT;
            break;
        case gpufmt::Format::R32G32_SFLOAT:
            conversion.exact = DXGI_FORMAT_R32G32_FLOAT;
            break;
        case gpufmt::Format::R32G32B32_UINT:
            conversion.exact = DXGI_FORMAT_R32G32B32_UINT;
            break;
        case gpufmt::Format::R32G32B32_SINT:
            conversion.exact = DXGI_FORMAT_R32G32B32_SINT;
            break;
        case gpufmt::Format::R32G32B32_SFLOAT:
            conversion.exact = DXGI_FORMAT_R32G32B32_FLOAT;
            break;
        case gpufmt::Format::R32G32B32A32_UINT:
            conversion.exact = DXGI_FORMAT_R32G32B32A32_UINT;
            break;
        case gpufmt::Format::R32G32B32A32_SINT:
            conversion.exact = DXGI_FORMAT_R32G32B32A32_SINT;
            break;
        case gpufmt::Format::R32G32B32A32_SFLOAT:
            conversion.exact = DXGI_FORMAT_R32G32B32A32_FLOAT;
            break;
#ifndef GF_EXCLUDE_64BIT_FORMATS
        case gpufmt::Format::R64_UINT:
            // not supported
            break;
        case gpufmt::Format::R64_SINT:
            // not supported
            break;
        case gpufmt::Format::R64_SFLOAT:
            // not supported
            break;
        case gpufmt::Format::R64G64_UINT:
            // not supported
            break;
        case gpufmt::Format::R64G64_SINT:
            // not supported
            break;
        case gpufmt::Format::R64G64_SFLOAT:
            // not supported
            break;
        case gpufmt::Format::R64G64B64_UINT:
            // not supported
            break;
        case gpufmt::Format::R64G64B64_SINT:
            // not supported
            break;
        case gpufmt::Format::R64G64B64_SFLOAT:
            // not supported
            break;
        case gpufmt::Format::R64G64B64A64_UINT:
            // not supported
            break;
        case gpufmt::Format::R64G64B64A64_SINT:
            // not supported
            break;
        case gpufmt::Format::R64G64B64A64_SFLOAT:
            // not supported
            break;
#endif // GF_EXCLUDE_64BIT_FORMATS
        case gpufmt::Format::B10G11R11_UFLOAT_PACK32:
            conversion.exact = DXGI_FORMAT_R11G11B10_FLOAT;
            break;
        case gpufmt::Format::E5B9G9R9_UFLOAT_PACK32:
            conversion.exact = DXGI_FORMAT_R9G9B9E5_SHAREDEXP;
            break;
        case gpufmt::Format::D16_UNORM:
            conversion.exact = DXGI_FORMAT_D16_UNORM;
            break;
        case gpufmt::Format::X8_D24_UNORM_PACK32:
            conversion.closest = DXGI_FORMAT_D24_UNORM_S8_UINT;
            break;
        case gpufmt::Format::D32_SFLOAT:
            conversion.exact = DXGI_FORMAT_D32_FLOAT;
            break;
        case gpufmt::Format::S8_UINT:
            //not supported
            break;
        case gpufmt::Format::D16_UNORM_S8_UINT:
            //not supported
            break;
        case gpufmt::Format::D24_UNORM_S8_UINT:
            conversion.exact = DXGI_FORMAT_D24_UNORM_S8_UINT;
            break;
        case gpufmt::Format::D32_SFLOAT_S8_UINT:
            conversion.exact = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
            break;
#ifndef GF_EXCLUDE_COMPRESSED_FORMATS
#ifndef GF_EXCLUDE_BC_COMPRESSED_FORMATS
        case gpufmt::Format::BC1_RGB_UNORM_BLOCK:
            conversion.closest = DXGI_FORMAT_BC1_UNORM;
            break;
        case gpufmt::Format::BC1_RGB_SRGB_BLOCK:
            conversion.closest = DXGI_FORMAT_BC1_UNORM_SRGB;
            break;
        case gpufmt::Format::BC1_RGBA_UNORM_BLOCK:
            conversion.exact = DXGI_FORMAT_BC1_UNORM;
            break;
        case gpufmt::Format::BC1_RGBA_SRGB_BLOCK:
            conversion.exact = DXGI_FORMAT_BC1_UNORM_SRGB;
            break;
        case gpufmt::Format::BC2_UNORM_BLOCK:
            conversion.exact = DXGI_FORMAT_BC2_UNORM;
            break;
        case gpufmt::Format::BC2_SRGB_BLOCK:
            conversion.exact = DXGI_FORMAT_BC2_UNORM_SRGB;
            break;
        case gpufmt::Format::BC3_UNORM_BLOCK:
            conversion.exact = DXGI_FORMAT_BC3_UNORM;
            break;
        case gpufmt::Format::BC3_SRGB_BLOCK:
            conversion.exact = DXGI_FORMAT_BC3_UNORM_SRGB;
            break;
        case gpufmt::Format::BC4_UNORM_BLOCK:
            conversion.exact = DXGI_FORMAT_BC4_UNORM;
            break;
        case gpufmt::Format::BC4_SNORM_BLOCK:
            conversion.exact = DXGI_FORMAT_BC4_SNORM;
            break;
        case gpufmt::Format::BC5_UNORM_BLOCK:
            conversion.exact = DXGI_FORMAT_BC5_UNORM;
            break;
        case gpufmt::Format::BC5_SNORM_BLOCK:
            conversion.exact = DXGI_FORMAT_BC5_SNORM;
            break;
        case gpufmt::Format::BC6H_UFLOAT_BLOCK:
            conversion.exact = DXGI_FORMAT_BC6H_UF16;
            break;
        case gpufmt::Format::BC6H_SFLOAT_BLOCK:
            conversion.exact = DXGI_FORMAT_BC6H_SF16;
            break;
        case gpufmt::Format::BC7_UNORM_BLOCK:
            conversion.exact = DXGI_FORMAT_BC7_UNORM;
            break;
        case gpufmt::Format::BC7_SRGB_BLOCK:
            conversion.exact = DXGI_FORMAT_BC7_UNORM_SRGB;
            break;
#endif //GF_EXCLUDE_BC_COMPRESSED_FORMATS
#ifndef GF_EXCLUDE_ETC_COMPRESSED_FORMATS
        case gpufmt::Format::ETC2_R8G8B8_UNORM_BLOCK:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM;
            break;
        case gpufmt::Format::ETC2_R8G8B8_SRGB_BLOCK:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            break;
        case gpufmt::Format::ETC2_R8G8B8A1_UNORM_BLOCK:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM;
            break;
        case gpufmt::Format::ETC2_R8G8B8A1_SRGB_BLOCK:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            break;
        case gpufmt::Format::ETC2_R8G8B8A8_UNORM_BLOCK:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM;
            break;
        case gpufmt::Format::ETC2_R8G8B8A8_SRGB_BLOCK:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            break;
        case gpufmt::Format::EAC_R11_UNORM_BLOCK:
            conversion.closest = DXGI_FORMAT_R16_UNORM;
            break;
        case gpufmt::Format::EAC_R11_SNORM_BLOCK:
            conversion.closest = DXGI_FORMAT_R16_SNORM;
            break;
        case gpufmt::Format::EAC_R11G11_UNORM_BLOCK:
            conversion.closest = DXGI_FORMAT_R16G16_UNORM;
            break;
        case gpufmt::Format::EAC_R11G11_SNORM_BLOCK:
            conversion.closest = DXGI_FORMAT_R16G16_SNORM;
            break;
#endif //GF_EXCLUDE_ETC_COMPRESSED_FORMATS
#ifndef GF_EXCLUDE_ASTC_COMPRESSED_FORMATS
        case gpufmt::Format::ASTC_4x4_UNORM_BLOCK:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM;
            break;
        case gpufmt::Format::ASTC_4x4_SRGB_BLOCK:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            break;
        case gpufmt::Format::ASTC_5x4_UNORM_BLOCK:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM;
            break;
        case gpufmt::Format::ASTC_5x4_SRGB_BLOCK:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            break;
        case gpufmt::Format::ASTC_5x5_UNORM_BLOCK:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM;
            break;
        case gpufmt::Format::ASTC_5x5_SRGB_BLOCK:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            break;
        case gpufmt::Format::ASTC_6x5_UNORM_BLOCK:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM;
            break;
        case gpufmt::Format::ASTC_6x5_SRGB_BLOCK:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            break;
        case gpufmt::Format::ASTC_6x6_UNORM_BLOCK:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM;
            break;
        case gpufmt::Format::ASTC_6x6_SRGB_BLOCK:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            break;
        case gpufmt::Format::ASTC_8x5_UNORM_BLOCK:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM;
            break;
        case gpufmt::Format::ASTC_8x5_SRGB_BLOCK:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            break;
        case gpufmt::Format::ASTC_8x6_UNORM_BLOCK:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM;
            break;
        case gpufmt::Format::ASTC_8x6_SRGB_BLOCK:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            break;
        case gpufmt::Format::ASTC_8x8_UNORM_BLOCK:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM;
            break;
        case gpufmt::Format::ASTC_8x8_SRGB_BLOCK:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            break;
        case gpufmt::Format::ASTC_10x5_UNORM_BLOCK:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM;
            break;
        case gpufmt::Format::ASTC_10x5_SRGB_BLOCK:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            break;
        case gpufmt::Format::ASTC_10x6_UNORM_BLOCK:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM;
            break;
        case gpufmt::Format::ASTC_10x6_SRGB_BLOCK:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            break;
        case gpufmt::Format::ASTC_10x8_UNORM_BLOCK:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM;
            break;
        case gpufmt::Format::ASTC_10x8_SRGB_BLOCK:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            break;
        case gpufmt::Format::ASTC_10x10_UNORM_BLOCK:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM;
            break;
        case gpufmt::Format::ASTC_10x10_SRGB_BLOCK:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            break;
        case gpufmt::Format::ASTC_12x10_UNORM_BLOCK:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM;
            break;
        case gpufmt::Format::ASTC_12x10_SRGB_BLOCK:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            break;
        case gpufmt::Format::ASTC_12x12_UNORM_BLOCK:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM;
            break;
        case gpufmt::Format::ASTC_12x12_SRGB_BLOCK:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            break;
#endif //GF_EXCLUDE_ASTC_COMPRESSED_FORMATS
#ifndef GF_EXCLUDE_PVRTC_COMPRESSED_FORMATS
        case gpufmt::Format::PVRTC1_2BPP_UNORM_BLOCK_IMG:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM;
            break;
        case gpufmt::Format::PVRTC1_4BPP_UNORM_BLOCK_IMG:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM;
            break;
        case gpufmt::Format::PVRTC2_2BPP_UNORM_BLOCK_IMG:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM;
            break;
        case gpufmt::Format::PVRTC2_4BPP_UNORM_BLOCK_IMG:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM;
            break;
        case gpufmt::Format::PVRTC1_2BPP_SRGB_BLOCK_IMG:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            break;
        case gpufmt::Format::PVRTC1_4BPP_SRGB_BLOCK_IMG:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            break;
        case gpufmt::Format::PVRTC2_2BPP_SRGB_BLOCK_IMG:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            break;
        case gpufmt::Format::PVRTC2_4BPP_SRGB_BLOCK_IMG:
            conversion.closest = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            break;
#endif //GF_EXCLUDE_PVRTC_COMPRESSED_FORMATS
#endif //GF_EXCLUDE_COMPRESSED_FORMATS
        default:
            break;
        }

        return conversion;
    }

    [[nodiscard]]
    static constexpr std::optional<gpufmt::Format> translateFormat(DXGI_FORMAT dxgiFormat) noexcept {
        std::optional<gpufmt::Format> format;

        switch(dxgiFormat) {
        case DXGI_FORMAT_UNKNOWN:
            format = gpufmt::Format::UNDEFINED;
            break;
        case DXGI_FORMAT_R32G32B32A32_TYPELESS:
            break;
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
            format = gpufmt::Format::R32G32B32A32_SFLOAT;
            break;
        case DXGI_FORMAT_R32G32B32A32_UINT:
            format = gpufmt::Format::R32G32B32A32_UINT;
            break;
        case DXGI_FORMAT_R32G32B32A32_SINT:
            format = gpufmt::Format::R32G32B32A32_SINT;
            break;
        case DXGI_FORMAT_R32G32B32_TYPELESS:
            break;
        case DXGI_FORMAT_R32G32B32_FLOAT:
            format = gpufmt::Format::R32G32B32_SFLOAT;
            break;
        case DXGI_FORMAT_R32G32B32_UINT:
            format = gpufmt::Format::R32G32B32_UINT;
            break;
        case DXGI_FORMAT_R32G32B32_SINT:
            format = gpufmt::Format::R32G32B32_SINT;
            break;
        case DXGI_FORMAT_R16G16B16A16_TYPELESS:
            break;
        case DXGI_FORMAT_R16G16B16A16_FLOAT:
            format = gpufmt::Format::R16G16B16A16_SFLOAT;
            break;
        case DXGI_FORMAT_R16G16B16A16_UNORM:
            format = gpufmt::Format::R16G16B16A16_UNORM;
            break;
        case DXGI_FORMAT_R16G16B16A16_UINT:
            format = gpufmt::Format::R16G16B16A16_UINT;
            break;
        case DXGI_FORMAT_R16G16B16A16_SNORM:
            format = gpufmt::Format::R16G16B16A16_SNORM;
            break;
        case DXGI_FORMAT_R16G16B16A16_SINT:
            format = gpufmt::Format::R16G16B16A16_SINT;
            break;
        case DXGI_FORMAT_R32G32_TYPELESS:
            break;
        case DXGI_FORMAT_R32G32_FLOAT:
            format = gpufmt::Format::R32G32_SFLOAT;
            break;
        case DXGI_FORMAT_R32G32_UINT:
            format = gpufmt::Format::R32G32_UINT;
            break;
        case DXGI_FORMAT_R32G32_SINT:
            format = gpufmt::Format::R32G32_SINT;
            break;
        case DXGI_FORMAT_R32G8X24_TYPELESS:
            break;
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
            format = gpufmt::Format::D32_SFLOAT_S8_UINT;
            break;
        case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
            break;
        case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
            break;
        case DXGI_FORMAT_R10G10B10A2_TYPELESS:
            break;
        case DXGI_FORMAT_R10G10B10A2_UNORM:
            format = gpufmt::Format::A2B10G10R10_UNORM_PACK32;
            break;
        case DXGI_FORMAT_R10G10B10A2_UINT:
            format = gpufmt::Format::A2B10G10R10_UINT_PACK32;
            break;
        case DXGI_FORMAT_R11G11B10_FLOAT:
            format = gpufmt::Format::B10G11R11_UFLOAT_PACK32;
            break;
        case DXGI_FORMAT_R8G8B8A8_TYPELESS:
            break;
        case DXGI_FORMAT_R8G8B8A8_UNORM:
            format = gpufmt::Format::R8G8B8A8_UNORM;
            break;
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
            format = gpufmt::Format::R8G8B8A8_SRGB;
            break;
        case DXGI_FORMAT_R8G8B8A8_UINT:
            format = gpufmt::Format::R8G8B8A8_UINT;
            break;
        case DXGI_FORMAT_R8G8B8A8_SNORM:
            format = gpufmt::Format::R8G8B8A8_SNORM;
            break;
        case DXGI_FORMAT_R8G8B8A8_SINT:
            format = gpufmt::Format::R8G8B8A8_SINT;
            break;
        case DXGI_FORMAT_R16G16_TYPELESS:
            break;
        case DXGI_FORMAT_R16G16_FLOAT:
            format = gpufmt::Format::R16G16_SFLOAT;
            break;
        case DXGI_FORMAT_R16G16_UNORM:
            format = gpufmt::Format::R16G16_UNORM;
            break;
        case DXGI_FORMAT_R16G16_UINT:
            format = gpufmt::Format::R16G16_UINT;
            break;
        case DXGI_FORMAT_R16G16_SNORM:
            format = gpufmt::Format::R16G16_SNORM;
            break;
        case DXGI_FORMAT_R16G16_SINT:
            format = gpufmt::Format::R16G16_SINT;
            break;
        case DXGI_FORMAT_R32_TYPELESS:
            break;
        case DXGI_FORMAT_D32_FLOAT:
            format = gpufmt::Format::D32_SFLOAT;
            break;
        case DXGI_FORMAT_R32_FLOAT:
            format = gpufmt::Format::R32_SFLOAT;
            break;
        case DXGI_FORMAT_R32_UINT:
            format = gpufmt::Format::R32_UINT;
            break;
        case DXGI_FORMAT_R32_SINT:
            format = gpufmt::Format::R32_SINT;
            break;
        case DXGI_FORMAT_R24G8_TYPELESS:
            break;
        case DXGI_FORMAT_D24_UNORM_S8_UINT:
            format = gpufmt::Format::D24_UNORM_S8_UINT;
            break;
        case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
            break;
        case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
            break;
        case DXGI_FORMAT_R8G8_TYPELESS:
            break;
        case DXGI_FORMAT_R8G8_UNORM:
            format = gpufmt::Format::R8G8_UNORM;
            break;
        case DXGI_FORMAT_R8G8_UINT:
            format = gpufmt::Format::R8G8_UINT;
            break;
        case DXGI_FORMAT_R8G8_SNORM:
            format = gpufmt::Format::R8G8_SNORM;
            break;
        case DXGI_FORMAT_R8G8_SINT:
            format = gpufmt::Format::R8G8_SINT;
            break;
        case DXGI_FORMAT_R16_TYPELESS:
            break;
        case DXGI_FORMAT_R16_FLOAT:
            format = gpufmt::Format::R16_SFLOAT;
            break;
        case DXGI_FORMAT_D16_UNORM:
            format = gpufmt::Format::D16_UNORM;
            break;
        case DXGI_FORMAT_R16_UNORM:
            format = gpufmt::Format::R16_UNORM;
            break;
        case DXGI_FORMAT_R16_UINT:
            format = gpufmt::Format::R16_UINT;
            break;
        case DXGI_FORMAT_R16_SNORM:
            format = gpufmt::Format::R16_SNORM;
            break;
        case DXGI_FORMAT_R16_SINT:
            format = gpufmt::Format::R16_SINT;
            break;
        case DXGI_FORMAT_R8_TYPELESS:
            break;
        case DXGI_FORMAT_R8_UNORM:
            format = gpufmt::Format::R8_UNORM;
            break;
        case DXGI_FORMAT_R8_UINT:
            format = gpufmt::Format::R8_UINT;
            break;
        case DXGI_FORMAT_R8_SNORM:
            format = gpufmt::Format::R8_SNORM;
            break;
        case DXGI_FORMAT_R8_SINT:
            format = gpufmt::Format::R8_SINT;
            break;
        case DXGI_FORMAT_A8_UNORM:
            format = gpufmt::Format::A8_UNORM;
            break;
        case DXGI_FORMAT_R1_UNORM:
            break;
        case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
            format = gpufmt::Format::E5B9G9R9_UFLOAT_PACK32;
            break;
        case DXGI_FORMAT_R8G8_B8G8_UNORM:
            break;
        case DXGI_FORMAT_G8R8_G8B8_UNORM:
            break;
#if !defined(GF_EXCLUDE_COMPRESSED_FORMATS) && !defined(GF_EXCLUDE_BC_COMPRESSED_FORMATS)
        case DXGI_FORMAT_BC1_TYPELESS:
            break;
        case DXGI_FORMAT_BC1_UNORM:
            format = gpufmt::Format::BC1_RGBA_UNORM_BLOCK;
            break;
        case DXGI_FORMAT_BC1_UNORM_SRGB:
            format = gpufmt::Format::BC1_RGBA_SRGB_BLOCK;
            break;
        case DXGI_FORMAT_BC2_TYPELESS:
            break;
        case DXGI_FORMAT_BC2_UNORM:
            format = gpufmt::Format::BC2_UNORM_BLOCK;
            break;
        case DXGI_FORMAT_BC2_UNORM_SRGB:
            format = gpufmt::Format::BC2_SRGB_BLOCK;
            break;
        case DXGI_FORMAT_BC3_TYPELESS:
            break;
        case DXGI_FORMAT_BC3_UNORM:
            format = gpufmt::Format::BC3_UNORM_BLOCK;
            break;
        case DXGI_FORMAT_BC3_UNORM_SRGB:
            format = gpufmt::Format::BC3_SRGB_BLOCK;
            break;
        case DXGI_FORMAT_BC4_TYPELESS:
            break;
        case DXGI_FORMAT_BC4_UNORM:
            format = gpufmt::Format::BC4_UNORM_BLOCK;
            break;
        case DXGI_FORMAT_BC4_SNORM:
            format = gpufmt::Format::BC4_SNORM_BLOCK;
            break;
        case DXGI_FORMAT_BC5_TYPELESS:
            break;
        case DXGI_FORMAT_BC5_UNORM:
            format = gpufmt::Format::BC5_UNORM_BLOCK;
            break;
        case DXGI_FORMAT_BC5_SNORM:
            format = gpufmt::Format::BC5_SNORM_BLOCK;
            break;
#endif //!defined(GF_EXCLUDE_COMPRESSED_FORMATS) && !defined(GF_EXCLUDE_BC_COMPRESSED_FORMATS)
        case DXGI_FORMAT_B5G6R5_UNORM:
            format = gpufmt::Format::R5G6B5_UNORM_PACK16;
            break;
        case DXGI_FORMAT_B5G5R5A1_UNORM:
            format = gpufmt::Format::A1R5G5B5_UNORM_PACK16;
            break;
        case DXGI_FORMAT_B8G8R8A8_UNORM:
            format = gpufmt::Format::B8G8R8A8_UNORM;
            break;
        case DXGI_FORMAT_B8G8R8X8_UNORM:
            format = gpufmt::Format::B8G8R8X8_UNORM;
            break;
        case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
            break;
        case DXGI_FORMAT_B8G8R8A8_TYPELESS:
            break;
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
            format = gpufmt::Format::B8G8R8A8_SRGB;
            break;
        case DXGI_FORMAT_B8G8R8X8_TYPELESS:
            break;
        case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
            format = gpufmt::Format::B8G8R8X8_SRGB;
            break;
#if !defined(GF_EXCLUDE_COMPRESSED_FORMATS) && !defined(GF_EXCLUDE_BC_COMPRESSED_FORMATS)
        case DXGI_FORMAT_BC6H_TYPELESS:
            break;
        case DXGI_FORMAT_BC6H_UF16:
            format = gpufmt::Format::BC6H_UFLOAT_BLOCK;
            break;
        case DXGI_FORMAT_BC6H_SF16:
            format = gpufmt::Format::BC6H_SFLOAT_BLOCK;
            break;
        case DXGI_FORMAT_BC7_TYPELESS:
            break;
        case DXGI_FORMAT_BC7_UNORM:
            format = gpufmt::Format::BC7_UNORM_BLOCK;
            break;
        case DXGI_FORMAT_BC7_UNORM_SRGB:
            format = gpufmt::Format::BC7_SRGB_BLOCK;
            break;
#endif //!defined(GF_EXCLUDE_COMPRESSED_FORMATS) && !defined(GF_EXCLUDE_BC_COMPRESSED_FORMATS)
        case DXGI_FORMAT_B4G4R4A4_UNORM:
            format = gpufmt::Format::A4R4G4B4_UNORM_PACK16;
            break;
        default:
            break;
        }

        return format;
    }

    [[nodiscard]]
    static constexpr std::optional<DXGI_FORMAT> depthShaderResourceViewFormat(Format format) noexcept {
        std::optional<DXGI_FORMAT> depthView;

        switch(format)
        {
        case gpufmt::Format::D16_UNORM:
            depthView = DXGI_FORMAT_R16_UNORM;
            break;
        case gpufmt::Format::X8_D24_UNORM_PACK32:
            depthView = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
            break;
        case gpufmt::Format::D32_SFLOAT:
            depthView = DXGI_FORMAT_R32_FLOAT;
            break;
        case gpufmt::Format::D24_UNORM_S8_UINT:
            depthView = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
            break;
        case gpufmt::Format::D32_SFLOAT_S8_UINT:
            depthView = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
            break;
        default:
            break;
        }

        return depthView;
    }

    [[nodiscard]]
    static constexpr std::optional<DXGI_FORMAT> depthShaderResourceViewFormat(DXGI_FORMAT format) noexcept {
        std::optional<DXGI_FORMAT> depthView;

        switch(format)
        {
        case DXGI_FORMAT_D16_UNORM:
            depthView = DXGI_FORMAT_R16_UNORM;
            break;
        case DXGI_FORMAT_D24_UNORM_S8_UINT:
            depthView = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
            break;
        case DXGI_FORMAT_D32_FLOAT:
            depthView = DXGI_FORMAT_R32_FLOAT;
            break;
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
            depthView = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
            break;
        default:
            break;
        }

        return depthView;
    }

    [[nodiscard]]
    static constexpr std::optional<DXGI_FORMAT> stencilShaderResourceViewFormat(Format format) noexcept {
        std::optional<DXGI_FORMAT> stencilView;

        switch(format)
        {
        case gpufmt::Format::D24_UNORM_S8_UINT:
            stencilView = DXGI_FORMAT_X24_TYPELESS_G8_UINT;
            break;
        case gpufmt::Format::D32_SFLOAT_S8_UINT:
            stencilView = DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;
            break;
        default:
            break;
        }

        return stencilView;
    }

    [[nodiscard]]
    static constexpr std::optional<DXGI_FORMAT> stencilShaderResourceViewFormat(DXGI_FORMAT format) noexcept {
        std::optional<DXGI_FORMAT> stencilView;

        switch(format)
        {
        case DXGI_FORMAT_D24_UNORM_S8_UINT:
            stencilView = DXGI_FORMAT_X24_TYPELESS_G8_UINT;
            break;
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
            stencilView = DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;
            break;
        default:
            break;
        }

        return stencilView;
    }

    [[nodiscard]]
    static constexpr bool isTypelessFormat(DXGI_FORMAT format) noexcept {
        switch(format)
        {
        case DXGI_FORMAT_R32G32B32A32_TYPELESS:
            return true;
        case DXGI_FORMAT_R32G32B32_TYPELESS:
            return true;
        case DXGI_FORMAT_R16G16B16A16_TYPELESS:
            return true;
        case DXGI_FORMAT_R32G32_TYPELESS:
            return true;
        case DXGI_FORMAT_R32G8X24_TYPELESS:
            return true;
        case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
            return true;
        case DXGI_FORMAT_R10G10B10A2_TYPELESS:
            return true;
        case DXGI_FORMAT_R8G8B8A8_TYPELESS:
            return true;
        case DXGI_FORMAT_R16G16_TYPELESS:
            return true;
        case DXGI_FORMAT_R32_TYPELESS:
            return true;
        case DXGI_FORMAT_R24G8_TYPELESS:
            return true;
        case DXGI_FORMAT_R8G8_TYPELESS:
            return true;
        case DXGI_FORMAT_R16_TYPELESS:
            return true;
        case DXGI_FORMAT_R8_TYPELESS:
            return true;
        case DXGI_FORMAT_BC1_TYPELESS:
            return true;
        case DXGI_FORMAT_BC2_TYPELESS:
            return true;
        case DXGI_FORMAT_BC3_TYPELESS:
            return true;
        case DXGI_FORMAT_BC4_TYPELESS:
            return true;
        case DXGI_FORMAT_BC5_TYPELESS:
            return true;
        case DXGI_FORMAT_B8G8R8A8_TYPELESS:
            return true;
        case DXGI_FORMAT_B8G8R8X8_TYPELESS:
            return true;
        case DXGI_FORMAT_BC6H_TYPELESS:
            return true;
        case DXGI_FORMAT_BC7_TYPELESS:
            return true;
        default:
            return false;
        }
    }

    [[nodiscard]]
    static constexpr std::optional<DXGI_FORMAT> typelessFormat(DXGI_FORMAT format) noexcept {
        std::optional<DXGI_FORMAT> typelessFormat;

        switch(format)
        {
        case DXGI_FORMAT_UNKNOWN:
            break;
        case DXGI_FORMAT_R32G32B32A32_TYPELESS:
            [[fallthrough]];
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
            [[fallthrough]];
        case DXGI_FORMAT_R32G32B32A32_UINT:
            [[fallthrough]];
        case DXGI_FORMAT_R32G32B32A32_SINT:
            typelessFormat = DXGI_FORMAT_R32G32B32A32_TYPELESS;
            break;
        case DXGI_FORMAT_R32G32B32_TYPELESS:
            [[fallthrough]];
        case DXGI_FORMAT_R32G32B32_FLOAT:
            [[fallthrough]];
        case DXGI_FORMAT_R32G32B32_UINT:
            [[fallthrough]];
        case DXGI_FORMAT_R32G32B32_SINT:
            typelessFormat = DXGI_FORMAT_R32G32B32_TYPELESS;
            break;
        case DXGI_FORMAT_R16G16B16A16_TYPELESS:
            [[fallthrough]];
        case DXGI_FORMAT_R16G16B16A16_FLOAT:
            [[fallthrough]];
        case DXGI_FORMAT_R16G16B16A16_UNORM:
            [[fallthrough]];
        case DXGI_FORMAT_R16G16B16A16_UINT:
            [[fallthrough]];
        case DXGI_FORMAT_R16G16B16A16_SNORM:
            [[fallthrough]];
        case DXGI_FORMAT_R16G16B16A16_SINT:
            typelessFormat = DXGI_FORMAT_R16G16B16A16_TYPELESS;
            break;
        case DXGI_FORMAT_R32G32_TYPELESS:
            [[fallthrough]];
        case DXGI_FORMAT_R32G32_FLOAT:
            [[fallthrough]];
        case DXGI_FORMAT_R32G32_UINT:
            [[fallthrough]];
        case DXGI_FORMAT_R32G32_SINT:
            typelessFormat = DXGI_FORMAT_R32G32_TYPELESS;
            break;
        case DXGI_FORMAT_R32G8X24_TYPELESS:
            [[fallthrough]];
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
            [[fallthrough]];
        case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
            [[fallthrough]];
        case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
            typelessFormat = DXGI_FORMAT_R32G8X24_TYPELESS;
            break;
        case DXGI_FORMAT_R10G10B10A2_TYPELESS:
            [[fallthrough]];
        case DXGI_FORMAT_R10G10B10A2_UNORM:
            [[fallthrough]];
        case DXGI_FORMAT_R10G10B10A2_UINT:
            typelessFormat = DXGI_FORMAT_R10G10B10A2_TYPELESS;
            break;
        case DXGI_FORMAT_R11G11B10_FLOAT:
            break;
        case DXGI_FORMAT_R8G8B8A8_TYPELESS:
            [[fallthrough]];
        case DXGI_FORMAT_R8G8B8A8_UNORM:
            [[fallthrough]];
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
            [[fallthrough]];
        case DXGI_FORMAT_R8G8B8A8_UINT:
            [[fallthrough]];
        case DXGI_FORMAT_R8G8B8A8_SNORM:
            [[fallthrough]];
        case DXGI_FORMAT_R8G8B8A8_SINT:
            typelessFormat = DXGI_FORMAT_R8G8B8A8_TYPELESS;
            break;
        case DXGI_FORMAT_R16G16_TYPELESS:
            [[fallthrough]];
        case DXGI_FORMAT_R16G16_FLOAT:
            [[fallthrough]];
        case DXGI_FORMAT_R16G16_UNORM:
            [[fallthrough]];
        case DXGI_FORMAT_R16G16_UINT:
            [[fallthrough]];
        case DXGI_FORMAT_R16G16_SNORM:
            [[fallthrough]];
        case DXGI_FORMAT_R16G16_SINT:
            typelessFormat = DXGI_FORMAT_R16G16_TYPELESS;
            break;
        case DXGI_FORMAT_R32_TYPELESS:
            [[fallthrough]];
        case DXGI_FORMAT_D32_FLOAT:
            [[fallthrough]];
        case DXGI_FORMAT_R32_FLOAT:
            [[fallthrough]];
        case DXGI_FORMAT_R32_UINT:
            [[fallthrough]];
        case DXGI_FORMAT_R32_SINT:
            typelessFormat = DXGI_FORMAT_R32_TYPELESS;
            break;
        case DXGI_FORMAT_R24G8_TYPELESS:
            [[fallthrough]];
        case DXGI_FORMAT_D24_UNORM_S8_UINT:
            [[fallthrough]];
        case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
            [[fallthrough]];
        case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
            typelessFormat = DXGI_FORMAT_R24G8_TYPELESS;
            break;
        case DXGI_FORMAT_R8G8_TYPELESS:
            [[fallthrough]];
        case DXGI_FORMAT_R8G8_UNORM:
            [[fallthrough]];
        case DXGI_FORMAT_R8G8_UINT:
            [[fallthrough]];
        case DXGI_FORMAT_R8G8_SNORM:
            [[fallthrough]];
        case DXGI_FORMAT_R8G8_SINT:
            typelessFormat = DXGI_FORMAT_R8G8_TYPELESS;
            break;
        case DXGI_FORMAT_R16_TYPELESS:
            [[fallthrough]];
        case DXGI_FORMAT_R16_FLOAT:
            [[fallthrough]];
        case DXGI_FORMAT_D16_UNORM:
            [[fallthrough]];
        case DXGI_FORMAT_R16_UNORM:
            [[fallthrough]];
        case DXGI_FORMAT_R16_UINT:
            [[fallthrough]];
        case DXGI_FORMAT_R16_SNORM:
            [[fallthrough]];
        case DXGI_FORMAT_R16_SINT:
            typelessFormat = DXGI_FORMAT_R16_TYPELESS;
            break;
        case DXGI_FORMAT_R8_TYPELESS:
            [[fallthrough]];
        case DXGI_FORMAT_R8_UNORM:
            [[fallthrough]];
        case DXGI_FORMAT_R8_UINT:
            [[fallthrough]];
        case DXGI_FORMAT_R8_SNORM:
            [[fallthrough]];
        case DXGI_FORMAT_R8_SINT:
            typelessFormat = DXGI_FORMAT_R8_TYPELESS;
            break;
        case DXGI_FORMAT_A8_UNORM:
            break;
        case DXGI_FORMAT_R1_UNORM:
            break;
        case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
            break;
        case DXGI_FORMAT_R8G8_B8G8_UNORM:
            break;
        case DXGI_FORMAT_G8R8_G8B8_UNORM:
            break;
        case DXGI_FORMAT_BC1_TYPELESS:
            [[fallthrough]];
        case DXGI_FORMAT_BC1_UNORM:
            [[fallthrough]];
        case DXGI_FORMAT_BC1_UNORM_SRGB:
            typelessFormat = DXGI_FORMAT_BC1_TYPELESS;
            break;
        case DXGI_FORMAT_BC2_TYPELESS:
            [[fallthrough]];
        case DXGI_FORMAT_BC2_UNORM:
            [[fallthrough]];
        case DXGI_FORMAT_BC2_UNORM_SRGB:
            typelessFormat = DXGI_FORMAT_BC2_TYPELESS;
            break;
        case DXGI_FORMAT_BC3_TYPELESS:
            [[fallthrough]];
        case DXGI_FORMAT_BC3_UNORM:
            [[fallthrough]];
        case DXGI_FORMAT_BC3_UNORM_SRGB:
            typelessFormat = DXGI_FORMAT_BC3_TYPELESS;
            break;
        case DXGI_FORMAT_BC4_TYPELESS:
            [[fallthrough]];
        case DXGI_FORMAT_BC4_UNORM:
            [[fallthrough]];
        case DXGI_FORMAT_BC4_SNORM:
            typelessFormat = DXGI_FORMAT_BC4_TYPELESS;
            break;
        case DXGI_FORMAT_BC5_TYPELESS:
            [[fallthrough]];
        case DXGI_FORMAT_BC5_UNORM:
            [[fallthrough]];
        case DXGI_FORMAT_BC5_SNORM:
            typelessFormat = DXGI_FORMAT_BC5_TYPELESS;
            break;
        case DXGI_FORMAT_B5G6R5_UNORM:
            break;
        case DXGI_FORMAT_B5G5R5A1_UNORM:
            break;
        case DXGI_FORMAT_B8G8R8A8_UNORM:
            typelessFormat = DXGI_FORMAT_B8G8R8A8_TYPELESS;
            break;
        case DXGI_FORMAT_B8G8R8X8_UNORM:
            typelessFormat = DXGI_FORMAT_B8G8R8X8_TYPELESS;
            break;
        case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
            break;
        case DXGI_FORMAT_B8G8R8A8_TYPELESS:
            typelessFormat = DXGI_FORMAT_B8G8R8A8_TYPELESS;
            break;
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
            typelessFormat = DXGI_FORMAT_B8G8R8A8_TYPELESS;
            break;
        case DXGI_FORMAT_B8G8R8X8_TYPELESS:
            typelessFormat = DXGI_FORMAT_B8G8R8X8_TYPELESS;
            break;
        case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
            typelessFormat = DXGI_FORMAT_B8G8R8X8_TYPELESS;
            break;
        case DXGI_FORMAT_BC6H_TYPELESS:
            [[fallthrough]];
        case DXGI_FORMAT_BC6H_UF16:
            [[fallthrough]];
        case DXGI_FORMAT_BC6H_SF16:
            typelessFormat = DXGI_FORMAT_BC6H_TYPELESS;
            break;
        case DXGI_FORMAT_BC7_TYPELESS:
            [[fallthrough]];
        case DXGI_FORMAT_BC7_UNORM:
            [[fallthrough]];
        case DXGI_FORMAT_BC7_UNORM_SRGB:
            typelessFormat = DXGI_FORMAT_BC7_TYPELESS;
            break;
        case DXGI_FORMAT_AYUV:
            break;
        case DXGI_FORMAT_Y410:
            break;
        case DXGI_FORMAT_Y416:
            break;
        case DXGI_FORMAT_NV12:
            break;
        case DXGI_FORMAT_P010:
            break;
        case DXGI_FORMAT_P016:
            break;
        case DXGI_FORMAT_420_OPAQUE:
            break;
        case DXGI_FORMAT_YUY2:
            break;
        case DXGI_FORMAT_Y210:
            break;
        case DXGI_FORMAT_Y216:
            break;
        case DXGI_FORMAT_NV11:
            break;
        case DXGI_FORMAT_AI44:
            break;
        case DXGI_FORMAT_IA44:
            break;
        case DXGI_FORMAT_P8:
            break;
        case DXGI_FORMAT_A8P8:
            break;
        case DXGI_FORMAT_B4G4R4A4_UNORM:
            break;
        case DXGI_FORMAT_P208:
            break;
        case DXGI_FORMAT_V208:
            break;
        case DXGI_FORMAT_V408:
            break;
        case DXGI_FORMAT_FORCE_UINT:
            break;
        default:
            break;
        }

        return typelessFormat;
    }

    [[nodiscard]]
    constexpr DXGI_FORMAT nextBestFormatLossless(DXGI_FORMAT format) noexcept {
        switch(format)
        {
        case DXGI_FORMAT_UNKNOWN:
            return DXGI_FORMAT_UNKNOWN;
        case DXGI_FORMAT_R32G32B32A32_TYPELESS:
            return DXGI_FORMAT_UNKNOWN;
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
            return DXGI_FORMAT_UNKNOWN;
        case DXGI_FORMAT_R32G32B32A32_UINT:
            return DXGI_FORMAT_UNKNOWN;
        case DXGI_FORMAT_R32G32B32A32_SINT:
            return DXGI_FORMAT_UNKNOWN;
        case DXGI_FORMAT_R32G32B32_TYPELESS:
            return DXGI_FORMAT_R32G32B32A32_TYPELESS;
        case DXGI_FORMAT_R32G32B32_FLOAT:
            return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case DXGI_FORMAT_R32G32B32_UINT:
            return DXGI_FORMAT_R32G32B32A32_UINT;
        case DXGI_FORMAT_R32G32B32_SINT:
            return DXGI_FORMAT_R32G32B32A32_SINT;
        case DXGI_FORMAT_R16G16B16A16_TYPELESS:
            return DXGI_FORMAT_R32G32B32A32_TYPELESS;
        case DXGI_FORMAT_R16G16B16A16_FLOAT:
            return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case DXGI_FORMAT_R16G16B16A16_UNORM:
            return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case DXGI_FORMAT_R16G16B16A16_UINT:
            return DXGI_FORMAT_R32G32B32A32_UINT;
        case DXGI_FORMAT_R16G16B16A16_SNORM:
            return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case DXGI_FORMAT_R16G16B16A16_SINT:
            return DXGI_FORMAT_R32G32B32A32_SINT;
        case DXGI_FORMAT_R32G32_TYPELESS:
            return DXGI_FORMAT_R32G32B32_TYPELESS;
        case DXGI_FORMAT_R32G32_FLOAT:
            return DXGI_FORMAT_R32G32B32_FLOAT;
        case DXGI_FORMAT_R32G32_UINT:
            return DXGI_FORMAT_R32G32B32_UINT;
        case DXGI_FORMAT_R32G32_SINT:
            return DXGI_FORMAT_R32G32B32_SINT;
        case DXGI_FORMAT_R32G8X24_TYPELESS:
            return DXGI_FORMAT_UNKNOWN;
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
            return DXGI_FORMAT_UNKNOWN;
        case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
            return DXGI_FORMAT_UNKNOWN;
        case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
            return DXGI_FORMAT_UNKNOWN;
        case DXGI_FORMAT_R10G10B10A2_TYPELESS:
            return DXGI_FORMAT_R16G16B16A16_TYPELESS;
        case DXGI_FORMAT_R10G10B10A2_UNORM:
            return DXGI_FORMAT_R16G16B16A16_UNORM;
        case DXGI_FORMAT_R10G10B10A2_UINT:
            return DXGI_FORMAT_R16G16B16A16_UINT;
        case DXGI_FORMAT_R11G11B10_FLOAT:
            return DXGI_FORMAT_R16G16B16A16_FLOAT;
        case DXGI_FORMAT_R8G8B8A8_TYPELESS:
            return DXGI_FORMAT_R16G16B16A16_TYPELESS;
        case DXGI_FORMAT_R8G8B8A8_UNORM:
            return DXGI_FORMAT_R16G16B16A16_UNORM;
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
            return DXGI_FORMAT_R16G16B16A16_UNORM;
        case DXGI_FORMAT_R8G8B8A8_UINT:
            return DXGI_FORMAT_R16G16B16A16_UINT;
        case DXGI_FORMAT_R8G8B8A8_SNORM:
            return DXGI_FORMAT_R16G16B16A16_SNORM;
        case DXGI_FORMAT_R8G8B8A8_SINT:
            return DXGI_FORMAT_R16G16B16A16_SINT;
        case DXGI_FORMAT_R16G16_TYPELESS:
            return DXGI_FORMAT_R16G16B16A16_TYPELESS;
        case DXGI_FORMAT_R16G16_FLOAT:
            return DXGI_FORMAT_R32G32_FLOAT;
        case DXGI_FORMAT_R16G16_UNORM:
            return DXGI_FORMAT_R32G32_FLOAT;
        case DXGI_FORMAT_R16G16_UINT:
            return DXGI_FORMAT_R32G32_UINT;
        case DXGI_FORMAT_R16G16_SNORM:
            return DXGI_FORMAT_R32G32_FLOAT;
        case DXGI_FORMAT_R16G16_SINT:
            return DXGI_FORMAT_R32G32_SINT;
        case DXGI_FORMAT_R32_TYPELESS:
            return DXGI_FORMAT_R32G32_TYPELESS;
        case DXGI_FORMAT_D32_FLOAT:
            return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
        case DXGI_FORMAT_R32_FLOAT:
            return DXGI_FORMAT_R32G32_FLOAT;
        case DXGI_FORMAT_R32_UINT:
            return DXGI_FORMAT_R32G32_UINT;
        case DXGI_FORMAT_R32_SINT:
            return DXGI_FORMAT_R32G32_SINT;
        case DXGI_FORMAT_R24G8_TYPELESS:
            return DXGI_FORMAT_R32G8X24_TYPELESS;
        case DXGI_FORMAT_D24_UNORM_S8_UINT:
            return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
        case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
            return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
        case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
            return DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;
        case DXGI_FORMAT_R8G8_TYPELESS:
            return DXGI_FORMAT_R16G16_TYPELESS;
        case DXGI_FORMAT_R8G8_UNORM:
            return DXGI_FORMAT_R16G16_UNORM;
        case DXGI_FORMAT_R8G8_UINT:
            return DXGI_FORMAT_R16G16_UINT;
        case DXGI_FORMAT_R8G8_SNORM:
            return DXGI_FORMAT_R16G16_SNORM;
        case DXGI_FORMAT_R8G8_SINT:
            return DXGI_FORMAT_R16G16_SINT;
        case DXGI_FORMAT_R16_TYPELESS:
            return DXGI_FORMAT_R32_TYPELESS;
        case DXGI_FORMAT_R16_FLOAT:
            return DXGI_FORMAT_R32_FLOAT;
        case DXGI_FORMAT_D16_UNORM:
            return DXGI_FORMAT_D32_FLOAT;
        case DXGI_FORMAT_R16_UNORM:
            return DXGI_FORMAT_R32_FLOAT;
        case DXGI_FORMAT_R16_UINT:
            return DXGI_FORMAT_R32_UINT;
        case DXGI_FORMAT_R16_SNORM:
            return DXGI_FORMAT_R32_FLOAT;
        case DXGI_FORMAT_R16_SINT:
            return DXGI_FORMAT_R32_SINT;
        case DXGI_FORMAT_R8_TYPELESS:
            return DXGI_FORMAT_R16_TYPELESS;
        case DXGI_FORMAT_R8_UNORM:
            return DXGI_FORMAT_R16_UNORM;
        case DXGI_FORMAT_R8_UINT:
            return DXGI_FORMAT_R16_UINT;
        case DXGI_FORMAT_R8_SNORM:
            return DXGI_FORMAT_R16_SNORM;
        case DXGI_FORMAT_R8_SINT:
            return DXGI_FORMAT_R16_SINT;
        case DXGI_FORMAT_A8_UNORM:
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        case DXGI_FORMAT_R1_UNORM:
            return DXGI_FORMAT_R8_UNORM;
        case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
            return DXGI_FORMAT_R16G16B16A16_FLOAT;
        case DXGI_FORMAT_R8G8_B8G8_UNORM:
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        case DXGI_FORMAT_G8R8_G8B8_UNORM:
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        case DXGI_FORMAT_BC1_TYPELESS:
            return DXGI_FORMAT_R8G8B8A8_TYPELESS;
        case DXGI_FORMAT_BC1_UNORM:
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        case DXGI_FORMAT_BC1_UNORM_SRGB:
            return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        case DXGI_FORMAT_BC2_TYPELESS:
            return DXGI_FORMAT_R8G8B8A8_TYPELESS;
        case DXGI_FORMAT_BC2_UNORM:
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        case DXGI_FORMAT_BC2_UNORM_SRGB:
            return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        case DXGI_FORMAT_BC3_TYPELESS:
            return DXGI_FORMAT_R8G8B8A8_TYPELESS;
        case DXGI_FORMAT_BC3_UNORM:
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        case DXGI_FORMAT_BC3_UNORM_SRGB:
            return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        case DXGI_FORMAT_BC4_TYPELESS:
            return DXGI_FORMAT_R8_TYPELESS;
        case DXGI_FORMAT_BC4_UNORM:
            return DXGI_FORMAT_R8_UNORM;
        case DXGI_FORMAT_BC4_SNORM:
            return DXGI_FORMAT_R8_SNORM;
        case DXGI_FORMAT_BC5_TYPELESS:
            return DXGI_FORMAT_R8G8_TYPELESS;
        case DXGI_FORMAT_BC5_UNORM:
            return DXGI_FORMAT_R8G8_UNORM;
        case DXGI_FORMAT_BC5_SNORM:
            return DXGI_FORMAT_R8G8_SNORM;
        case DXGI_FORMAT_B5G6R5_UNORM:
            return DXGI_FORMAT_B8G8R8A8_UNORM;
        case DXGI_FORMAT_B5G5R5A1_UNORM:
            return DXGI_FORMAT_B8G8R8A8_UNORM;
        case DXGI_FORMAT_B8G8R8A8_UNORM:
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        case DXGI_FORMAT_B8G8R8X8_UNORM:
            return DXGI_FORMAT_B8G8R8A8_UNORM;
        case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
            return DXGI_FORMAT_R16G16B16A16_UNORM;
        case DXGI_FORMAT_B8G8R8A8_TYPELESS:
            return DXGI_FORMAT_R8G8B8A8_TYPELESS;
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
            return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        case DXGI_FORMAT_B8G8R8X8_TYPELESS:
            return DXGI_FORMAT_B8G8R8A8_TYPELESS;
        case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
            return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
        case DXGI_FORMAT_BC6H_TYPELESS:
            return DXGI_FORMAT_R16G16B16A16_TYPELESS;
        case DXGI_FORMAT_BC6H_UF16:
            return DXGI_FORMAT_R32G32B32_FLOAT;
        case DXGI_FORMAT_BC6H_SF16:
            return DXGI_FORMAT_R16G16B16A16_FLOAT;
        case DXGI_FORMAT_BC7_TYPELESS:
            return DXGI_FORMAT_R8G8B8A8_TYPELESS;
        case DXGI_FORMAT_BC7_UNORM:
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        case DXGI_FORMAT_BC7_UNORM_SRGB:
            return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        case DXGI_FORMAT_AYUV:
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        case DXGI_FORMAT_Y410:
            return DXGI_FORMAT_R10G10B10A2_UNORM;
        case DXGI_FORMAT_Y416:
            return DXGI_FORMAT_R16G16B16A16_UNORM;
        case DXGI_FORMAT_NV12:
            return DXGI_FORMAT_UNKNOWN;
        case DXGI_FORMAT_P010:
            return DXGI_FORMAT_R16_UNORM;
        case DXGI_FORMAT_P016:
            return DXGI_FORMAT_R16_UNORM;
        case DXGI_FORMAT_420_OPAQUE:
            return DXGI_FORMAT_UNKNOWN;
        case DXGI_FORMAT_YUY2:
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        case DXGI_FORMAT_Y210:
            return DXGI_FORMAT_R16G16B16A16_UNORM;
        case DXGI_FORMAT_Y216:
            return DXGI_FORMAT_R16G16B16A16_UNORM;
        case DXGI_FORMAT_NV11:
            return DXGI_FORMAT_UNKNOWN;
        case DXGI_FORMAT_AI44:
            return DXGI_FORMAT_UNKNOWN;
        case DXGI_FORMAT_IA44:
            return DXGI_FORMAT_UNKNOWN;
        case DXGI_FORMAT_P8:
            return DXGI_FORMAT_UNKNOWN;
        case DXGI_FORMAT_A8P8:
            return DXGI_FORMAT_UNKNOWN;
        case DXGI_FORMAT_B4G4R4A4_UNORM:
            return DXGI_FORMAT_B8G8R8A8_UNORM;
        case DXGI_FORMAT_P208:
            return DXGI_FORMAT_UNKNOWN;
        case DXGI_FORMAT_V208:
            return DXGI_FORMAT_UNKNOWN;
        case DXGI_FORMAT_V408:
            return DXGI_FORMAT_UNKNOWN;
        case DXGI_FORMAT_FORCE_UINT:
            return DXGI_FORMAT_UNKNOWN;
        default:
            return DXGI_FORMAT_UNKNOWN;
        }
    }
}

[[nodiscard]]
static constexpr std::string_view to_string(DXGI_FORMAT format) noexcept {
    switch(format)
    {
    case DXGI_FORMAT_UNKNOWN:
        return "DXGI_FORMAT_UNKNOWN";
    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
        return "DXGI_FORMAT_R32G32B32A32_TYPELESS";
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
        return "DXGI_FORMAT_R32G32B32A32_FLOAT";
    case DXGI_FORMAT_R32G32B32A32_UINT:
        return "DXGI_FORMAT_R32G32B32A32_UINT";
    case DXGI_FORMAT_R32G32B32A32_SINT:
        return "DXGI_FORMAT_R32G32B32A32_SINT";
    case DXGI_FORMAT_R32G32B32_TYPELESS:
        return "DXGI_FORMAT_R32G32B32_TYPELESS";
    case DXGI_FORMAT_R32G32B32_FLOAT:
        return "DXGI_FORMAT_R32G32B32_FLOAT";
    case DXGI_FORMAT_R32G32B32_UINT:
        return "DXGI_FORMAT_R32G32B32_UINT";
    case DXGI_FORMAT_R32G32B32_SINT:
        return "DXGI_FORMAT_R32G32B32_SINT";
    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
        return "DXGI_FORMAT_R16G16B16A16_TYPELESS";
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
        return "DXGI_FORMAT_R16G16B16A16_FLOAT";
    case DXGI_FORMAT_R16G16B16A16_UNORM:
        return "DXGI_FORMAT_R16G16B16A16_UNORM";
    case DXGI_FORMAT_R16G16B16A16_UINT:
        return "DXGI_FORMAT_R16G16B16A16_UINT";
    case DXGI_FORMAT_R16G16B16A16_SNORM:
        return "DXGI_FORMAT_R16G16B16A16_SNORM";
    case DXGI_FORMAT_R16G16B16A16_SINT:
        return "DXGI_FORMAT_R16G16B16A16_SINT";
    case DXGI_FORMAT_R32G32_TYPELESS:
        return "DXGI_FORMAT_R32G32_TYPELESS";
    case DXGI_FORMAT_R32G32_FLOAT:
        return "DXGI_FORMAT_R32G32_FLOAT";
    case DXGI_FORMAT_R32G32_UINT:
        return "DXGI_FORMAT_R32G32_UINT";
    case DXGI_FORMAT_R32G32_SINT:
        return "DXGI_FORMAT_R32G32_SINT";
    case DXGI_FORMAT_R32G8X24_TYPELESS:
        return "DXGI_FORMAT_R32G8X24_TYPELESS";
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        return "DXGI_FORMAT_D32_FLOAT_S8X24_UINT";
    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
        return "DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS";
    case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
        return "DXGI_FORMAT_X32_TYPELESS_G8X24_UINT";
    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
        return "DXGI_FORMAT_R10G10B10A2_TYPELESS";
    case DXGI_FORMAT_R10G10B10A2_UNORM:
        return "DXGI_FORMAT_R10G10B10A2_UNORM";
    case DXGI_FORMAT_R10G10B10A2_UINT:
        return "DXGI_FORMAT_R10G10B10A2_UINT";
    case DXGI_FORMAT_R11G11B10_FLOAT:
        return "DXGI_FORMAT_R11G11B10_FLOAT";
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
        return "DXGI_FORMAT_R8G8B8A8_TYPELESS";
    case DXGI_FORMAT_R8G8B8A8_UNORM:
        return "DXGI_FORMAT_R8G8B8A8_UNORM";
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        return "DXGI_FORMAT_R8G8B8A8_UNORM_SRGB";
    case DXGI_FORMAT_R8G8B8A8_UINT:
        return "DXGI_FORMAT_R8G8B8A8_UINT";
    case DXGI_FORMAT_R8G8B8A8_SNORM:
        return "DXGI_FORMAT_R8G8B8A8_SNORM";
    case DXGI_FORMAT_R8G8B8A8_SINT:
        return "DXGI_FORMAT_R8G8B8A8_SINT";
    case DXGI_FORMAT_R16G16_TYPELESS:
        return "DXGI_FORMAT_R16G16_TYPELESS";
    case DXGI_FORMAT_R16G16_FLOAT:
        return "DXGI_FORMAT_R16G16_FLOAT";
    case DXGI_FORMAT_R16G16_UNORM:
        return "DXGI_FORMAT_R16G16_UNORM";
    case DXGI_FORMAT_R16G16_UINT:
        return "DXGI_FORMAT_R16G16_UINT";
    case DXGI_FORMAT_R16G16_SNORM:
        return "DXGI_FORMAT_R16G16_SNORM";
    case DXGI_FORMAT_R16G16_SINT:
        return "DXGI_FORMAT_R16G16_SINT";
    case DXGI_FORMAT_R32_TYPELESS:
        return "DXGI_FORMAT_R32_TYPELESS";
    case DXGI_FORMAT_D32_FLOAT:
        return "DXGI_FORMAT_D32_FLOAT";
    case DXGI_FORMAT_R32_FLOAT:
        return "DXGI_FORMAT_R32_FLOAT";
    case DXGI_FORMAT_R32_UINT:
        return "DXGI_FORMAT_R32_UINT";
    case DXGI_FORMAT_R32_SINT:
        return "DXGI_FORMAT_R32_SINT";
    case DXGI_FORMAT_R24G8_TYPELESS:
        return "DXGI_FORMAT_R24G8_TYPELESS";
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
        return "DXGI_FORMAT_D24_UNORM_S8_UINT";
    case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
        return "DXGI_FORMAT_R24_UNORM_X8_TYPELESS";
    case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
        return "DXGI_FORMAT_X24_TYPELESS_G8_UINT";
    case DXGI_FORMAT_R8G8_TYPELESS:
        return "DXGI_FORMAT_R8G8_TYPELESS";
    case DXGI_FORMAT_R8G8_UNORM:
        return "DXGI_FORMAT_R8G8_UNORM";
    case DXGI_FORMAT_R8G8_UINT:
        return "DXGI_FORMAT_R8G8_UINT";
    case DXGI_FORMAT_R8G8_SNORM:
        return "DXGI_FORMAT_R8G8_SNORM";
    case DXGI_FORMAT_R8G8_SINT:
        return "DXGI_FORMAT_R8G8_SINT";
    case DXGI_FORMAT_R16_TYPELESS:
        return "DXGI_FORMAT_R16_TYPELESS";
    case DXGI_FORMAT_R16_FLOAT:
        return "DXGI_FORMAT_R16_FLOAT";
    case DXGI_FORMAT_D16_UNORM:
        return "DXGI_FORMAT_D16_UNORM";
    case DXGI_FORMAT_R16_UNORM:
        return "DXGI_FORMAT_R16_UNORM";
    case DXGI_FORMAT_R16_UINT:
        return "DXGI_FORMAT_R16_UINT";
    case DXGI_FORMAT_R16_SNORM:
        return "DXGI_FORMAT_R16_SNORM";
    case DXGI_FORMAT_R16_SINT:
        return "DXGI_FORMAT_R16_SINT";
    case DXGI_FORMAT_R8_TYPELESS:
        return "DXGI_FORMAT_R8_TYPELESS";
    case DXGI_FORMAT_R8_UNORM:
        return "DXGI_FORMAT_R8_UNORM";
    case DXGI_FORMAT_R8_UINT:
        return "DXGI_FORMAT_R8_UINT";
    case DXGI_FORMAT_R8_SNORM:
        return "DXGI_FORMAT_R8_SNORM";
    case DXGI_FORMAT_R8_SINT:
        return "DXGI_FORMAT_R8_SINT";
    case DXGI_FORMAT_A8_UNORM:
        return "DXGI_FORMAT_A8_UNORM";
    case DXGI_FORMAT_R1_UNORM:
        return "DXGI_FORMAT_R1_UNORM";
    case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
        return "DXGI_FORMAT_R9G9B9E5_SHAREDEXP";
    case DXGI_FORMAT_R8G8_B8G8_UNORM:
        return "DXGI_FORMAT_R8G8_B8G8_UNORM";
    case DXGI_FORMAT_G8R8_G8B8_UNORM:
        return "DXGI_FORMAT_G8R8_G8B8_UNORM";
    case DXGI_FORMAT_BC1_TYPELESS:
        return "DXGI_FORMAT_BC1_TYPELESS";
    case DXGI_FORMAT_BC1_UNORM:
        return "DXGI_FORMAT_BC1_UNORM";
    case DXGI_FORMAT_BC1_UNORM_SRGB:
        return "DXGI_FORMAT_BC1_UNORM_SRGB";
    case DXGI_FORMAT_BC2_TYPELESS:
        return "DXGI_FORMAT_BC2_TYPELESS";
    case DXGI_FORMAT_BC2_UNORM:
        return "DXGI_FORMAT_BC2_UNORM";
    case DXGI_FORMAT_BC2_UNORM_SRGB:
        return "DXGI_FORMAT_BC2_UNORM_SRGB";
    case DXGI_FORMAT_BC3_TYPELESS:
        return "DXGI_FORMAT_BC3_TYPELESS";
    case DXGI_FORMAT_BC3_UNORM:
        return "DXGI_FORMAT_BC3_UNORM";
    case DXGI_FORMAT_BC3_UNORM_SRGB:
        return "DXGI_FORMAT_BC3_UNORM_SRGB";
    case DXGI_FORMAT_BC4_TYPELESS:
        return "DXGI_FORMAT_BC4_TYPELESS";
    case DXGI_FORMAT_BC4_UNORM:
        return "DXGI_FORMAT_BC4_UNORM";
    case DXGI_FORMAT_BC4_SNORM:
        return "DXGI_FORMAT_BC4_SNORM";
    case DXGI_FORMAT_BC5_TYPELESS:
        return "DXGI_FORMAT_BC5_TYPELESS";
    case DXGI_FORMAT_BC5_UNORM:
        return "DXGI_FORMAT_BC5_UNORM";
    case DXGI_FORMAT_BC5_SNORM:
        return "DXGI_FORMAT_BC5_SNORM";
    case DXGI_FORMAT_B5G6R5_UNORM:
        return "DXGI_FORMAT_B5G6R5_UNORM";
    case DXGI_FORMAT_B5G5R5A1_UNORM:
        return "DXGI_FORMAT_B5G5R5A1_UNORM";
    case DXGI_FORMAT_B8G8R8A8_UNORM:
        return "DXGI_FORMAT_B8G8R8A8_UNORM";
    case DXGI_FORMAT_B8G8R8X8_UNORM:
        return "DXGI_FORMAT_B8G8R8X8_UNORM";
    case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
        return "DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM";
    case DXGI_FORMAT_B8G8R8A8_TYPELESS:
        return "DXGI_FORMAT_B8G8R8A8_TYPELESS";
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        return "DXGI_FORMAT_B8G8R8A8_UNORM_SRGB";
    case DXGI_FORMAT_B8G8R8X8_TYPELESS:
        return "DXGI_FORMAT_B8G8R8X8_TYPELESS";
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        return "DXGI_FORMAT_B8G8R8X8_UNORM_SRGB";
    case DXGI_FORMAT_BC6H_TYPELESS:
        return "DXGI_FORMAT_BC6H_TYPELESS";
    case DXGI_FORMAT_BC6H_UF16:
        return "DXGI_FORMAT_BC6H_UF16";
    case DXGI_FORMAT_BC6H_SF16:
        return "DXGI_FORMAT_BC6H_SF16";
    case DXGI_FORMAT_BC7_TYPELESS:
        return "DXGI_FORMAT_BC7_TYPELESS";
    case DXGI_FORMAT_BC7_UNORM:
        return "DXGI_FORMAT_BC7_UNORM";
    case DXGI_FORMAT_BC7_UNORM_SRGB:
        return "DXGI_FORMAT_BC7_UNORM_SRGB";
    case DXGI_FORMAT_AYUV:
        return "DXGI_FORMAT_AYUV";
    case DXGI_FORMAT_Y410:
        return "DXGI_FORMAT_Y410";
    case DXGI_FORMAT_Y416:
        return "DXGI_FORMAT_Y416";
    case DXGI_FORMAT_NV12:
        return "DXGI_FORMAT_NV12";
    case DXGI_FORMAT_P010:
        return "DXGI_FORMAT_P010";
    case DXGI_FORMAT_P016:
        return "DXGI_FORMAT_P016";
    case DXGI_FORMAT_420_OPAQUE:
        return "DXGI_FORMAT_420_OPAQUE";
    case DXGI_FORMAT_YUY2:
        return "DXGI_FORMAT_YUY2";
    case DXGI_FORMAT_Y210:
        return "DXGI_FORMAT_Y210";
    case DXGI_FORMAT_Y216:
        return "DXGI_FORMAT_Y216";
    case DXGI_FORMAT_NV11:
        return "DXGI_FORMAT_NV11";
    case DXGI_FORMAT_AI44:
        return "DXGI_FORMAT_AI44";
    case DXGI_FORMAT_IA44:
        return "DXGI_FORMAT_IA44";
    case DXGI_FORMAT_P8:
        return "DXGI_FORMAT_P8";
    case DXGI_FORMAT_A8P8:
        return "DXGI_FORMAT_A8P8";
    case DXGI_FORMAT_B4G4R4A4_UNORM:
        return "DXGI_FORMAT_B4G4R4A4_UNORM";
    case DXGI_FORMAT_P208:
        return "DXGI_FORMAT_P208";
    case DXGI_FORMAT_V208:
        return "DXGI_FORMAT_V208";
    case DXGI_FORMAT_V408:
        return "DXGI_FORMAT_V408";
    case DXGI_FORMAT_FORCE_UINT:
        return "DXGI_FORMAT_FORCE_UINT";
    default:
        return "DXGI_FORMAT_UNKNOWN";
    }
}

inline std::ostream &operator<<(std::ostream &out, DXGI_FORMAT format) {
    out << to_string(format);
    return out;
}