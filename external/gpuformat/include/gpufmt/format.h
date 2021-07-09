#pragma once

// All of the formats are based on the Vulkan specifications. For details, visit
// https://www.khronos.org/registry/vulkan/specs/1.1/html/chap32.html#VkFormat
//
//
// Format component layout
//-------------------------
// Channel|Bits_NumericFormat
// Channel|Bits_NumericFormat_PACK|Bits
//
//
// Numeric formats
//-----------------
// UNORM: The components are unsigned normalized values in the range[0, 1]. value / (2^n - 1)
// SNORM: The components are signed normalized values in the range [-1,1]. (value / (2^n - 1)) * 2 - 1
// USCALED: The components are unsigned integer values that get converted to floating-point in the range [0,2^n - 1]
// SSCALED: The components are signed integer values that get converted to floating-point in the range [-2^(n-1),2^(n-1) - 1]
// UINT: The components are unsigned integer values in the range[0, 2^n - 1]
// SINT: The components are signed integer values in the range [-2^(n-1),2^(n-1) - 1]
// UFLOAT: The components are unsigned floating-point numbers (used by packed, shared exponent, and some compressed formats)
// SFLOAT: The components are signed floating-point numbers
// SRGB: The R, G, and B components are unsigned normalized values that represent values using sRGB nonlinear encoding, while
//       the A component (if one exists) is a regular unsigned normalized value
//
//
// Packing
//---------
// There are two ways formats are packed into bits; those with _PACKxx and those without.
//
// Formats with _PACK postfix
//-----------------------------
// Formats postfixed with a PACK specifier are packed as bits with the components
// from left to right packed MSB to LSB. Each components value is packed from LSB to MSB.
// 
// PACK examples:
//
// R4G4B4A4_UNORM_PACK16
// MSB                                           LSB
// |15|14|13|12|11|10|09|08|07|06|05|04|03|02|01|00| Bits
// |R3|R2|R1|R0|G3|G2|G1|G0|B3|B2|B1|B0|A3|A2|A1|A0| Texel data
//
// R5G5B5A1_UNORM_PACK16
// MSB                                           LSB
// |15|14|13|12|11|10|09|08|07|06|05|04|03|02|01|00| Bits
// |R4|R3|R2|R1|R0|G4|G3|G2|G1|G0|B4|B3|B2|B1|B0|A0| Texel data
//
//
// All other formats
//-------------------
// All other formats (those without a _PACK postfix) assume a memory byte ordering. That is, assume
// the components are an array of component data type (byte, short, int, float, double)
//
// examples:
//
// R8G8B8A8_UNORM
// MSB                                                                                           LSB
// |31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16|15|14|13|12|11|10|09|08|07|06|05|04|03|02|01|00| Bits
// | Byte 3                | Byte 2                | Byte 1                | Byte 0                | Bytes
// |A7|A6|A5|A4|A3|A2|A1|A0|B7|B6|B5|B4|B3|B2|B1|B0|G7|G6|G5|G4|G3|G2|G1|G0|R7|R6|R5|R4|R3|R2|R1|R0| Texel data
//
// B8G8R8A8_UNORM
// MSB                                                                                           LSB
// |31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16|15|14|13|12|11|10|09|08|07|06|05|04|03|02|01|00| Bits
// | Byte 3                | Byte 2                | Byte 1                | Byte 0                | Bytes
// |A7|A6|A5|A4|A3|A2|A1|A0|R7|R6|R5|R4|R3|R2|R1|R0|G7|G6|G5|G4|G3|G2|G1|G0|B7|B6|B5|B4|B3|B2|B1|B0| Texel data

// R16G16_UNORM
// MSB                                                                                           LSB
// |31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16|15|14|13|12|11|10|09|08|07|06|05|04|03|02|01|00| Bits
// | Byte 3                | Byte 2                | Byte 1                | Byte 0                | Bytes
// | Short 1                                       | Short 0                                       | Shorts
// |Bf|Be|Bd|Bc|Bb|Ba|B9|B8|B7|B6|B5|B4|B3|B2|B1|B0|Rf|Re|Rd|Rc|Rb|Ra|R9|R8|R7|R6|R5|R4|R3|R2|R1|R0| Texel data

#include <gpufmt/config.h>

namespace gpufmt {
    enum class Format {
        UNDEFINED,
        R4G4_UNORM_PACK8,
        R4G4B4A4_UNORM_PACK16,
        B4G4R4A4_UNORM_PACK16,
        A4R4G4B4_UNORM_PACK16,
        R5G6B5_UNORM_PACK16,
        B5G6R5_UNORM_PACK16,
        R5G5B5A1_UNORM_PACK16,
        B5G5R5A1_UNORM_PACK16,
        A1R5G5B5_UNORM_PACK16,
        R8_UNORM,
        R8_SNORM,
#ifndef GF_EXCLUDE_SCALED_FORMATS
        R8_USCALED,
        R8_SSCALED,
#endif
        R8_UINT,
        R8_SINT,
        R8_SRGB,
        A8_UNORM,
        R8G8_UNORM,
        R8G8_SNORM,
#ifndef GF_EXCLUDE_SCALED_FORMATS
        R8G8_USCALED,
        R8G8_SSCALED,
#endif
        R8G8_UINT,
        R8G8_SINT,
        R8G8_SRGB,
        R8G8B8_UNORM,
        R8G8B8_SNORM,
#ifndef GF_EXCLUDE_SCALED_FORMATS
        R8G8B8_USCALED,
        R8G8B8_SSCALED,
#endif
        R8G8B8_UINT,
        R8G8B8_SINT,
        R8G8B8_SRGB,
        B8G8R8_UNORM,
        B8G8R8_SNORM,
#ifndef GF_EXCLUDE_SCALED_FORMATS
        B8G8R8_USCALED,
        B8G8R8_SSCALED,
#endif
        B8G8R8_UINT,
        B8G8R8_SINT,
        B8G8R8_SRGB,
        R8G8B8A8_UNORM,
        R8G8B8A8_SNORM,
#ifndef GF_EXCLUDE_SCALED_FORMATS
        R8G8B8A8_USCALED,
        R8G8B8A8_SSCALED,
#endif
        R8G8B8A8_UINT,
        R8G8B8A8_SINT,
        R8G8B8A8_SRGB,
        B8G8R8A8_UNORM,
        B8G8R8A8_SNORM,
#ifndef GF_EXCLUDE_SCALED_FORMATS
        B8G8R8A8_USCALED,
        B8G8R8A8_SSCALED,
#endif
        B8G8R8A8_UINT,
        B8G8R8A8_SINT,
        B8G8R8A8_SRGB,
        B8G8R8X8_UNORM,
        B8G8R8X8_SRGB,
        A8B8G8R8_UNORM_PACK32,
        A8B8G8R8_SNORM_PACK32,
#ifndef GF_EXCLUDE_SCALED_FORMATS
        A8B8G8R8_USCALED_PACK32,
        A8B8G8R8_SSCALED_PACK32,
#endif
        A8B8G8R8_UINT_PACK32,
        A8B8G8R8_SINT_PACK32,
        A8B8G8R8_SRGB_PACK32,
        A2R10G10B10_UNORM_PACK32,
        A2R10G10B10_SNORM_PACK32,
#ifndef GF_EXCLUDE_SCALED_FORMATS
        A2R10G10B10_USCALED_PACK32,
        A2R10G10B10_SSCALED_PACK32,
#endif
        A2R10G10B10_UINT_PACK32,
        A2R10G10B10_SINT_PACK32,
        A2B10G10R10_UNORM_PACK32,
        A2B10G10R10_SNORM_PACK32,
#ifndef GF_EXCLUDE_SCALED_FORMATS
        A2B10G10R10_USCALED_PACK32,
        A2B10G10R10_SSCALED_PACK32,
#endif
        A2B10G10R10_UINT_PACK32,
        A2B10G10R10_SINT_PACK32,
        R16_UNORM,
        R16_SNORM,
#ifndef GF_EXCLUDE_SCALED_FORMATS
        R16_USCALED,
        R16_SSCALED,
#endif
        R16_UINT,
        R16_SINT,
        R16_SFLOAT,
        R16G16_UNORM,
        R16G16_SNORM,
#ifndef GF_EXCLUDE_SCALED_FORMATS
        R16G16_USCALED,
        R16G16_SSCALED,
#endif
        R16G16_UINT,
        R16G16_SINT,
        R16G16_SFLOAT,
        R16G16B16_UNORM,
        R16G16B16_SNORM,
#ifndef GF_EXCLUDE_SCALED_FORMATS
        R16G16B16_USCALED,
        R16G16B16_SSCALED,
#endif
        R16G16B16_UINT,
        R16G16B16_SINT,
        R16G16B16_SFLOAT,
        R16G16B16A16_UNORM,
        R16G16B16A16_SNORM,
#ifndef GF_EXCLUDE_SCALED_FORMATS
        R16G16B16A16_USCALED,
        R16G16B16A16_SSCALED,
#endif
        R16G16B16A16_UINT,
        R16G16B16A16_SINT,
        R16G16B16A16_SFLOAT,
        R32_UINT,
        R32_SINT,
        R32_SFLOAT,
        R32G32_UINT,
        R32G32_SINT,
        R32G32_SFLOAT,
        R32G32B32_UINT,
        R32G32B32_SINT,
        R32G32B32_SFLOAT,
        R32G32B32A32_UINT,
        R32G32B32A32_SINT,
        R32G32B32A32_SFLOAT,
#ifndef GF_EXCLUDE_64BIT_FORMATS
        R64_UINT,
        R64_SINT,
        R64_SFLOAT,
        R64G64_UINT,
        R64G64_SINT,
        R64G64_SFLOAT,
        R64G64B64_UINT,
        R64G64B64_SINT,
        R64G64B64_SFLOAT,
        R64G64B64A64_UINT,
        R64G64B64A64_SINT,
        R64G64B64A64_SFLOAT,
#endif
        B10G11R11_UFLOAT_PACK32,
        E5B9G9R9_UFLOAT_PACK32,
        D16_UNORM,
        X8_D24_UNORM_PACK32,
        D32_SFLOAT,
        S8_UINT,
        D16_UNORM_S8_UINT,
        D24_UNORM_S8_UINT,
        D32_SFLOAT_S8_UINT,
#ifndef GF_EXCLUDE_COMPRESSED_FORMATS
#ifndef GF_EXCLUDE_BC_COMPRESSED_FORMATS
        BC1_RGB_UNORM_BLOCK,
        BC1_RGB_SRGB_BLOCK,
        BC1_RGBA_UNORM_BLOCK,
        BC1_RGBA_SRGB_BLOCK,
        BC2_UNORM_BLOCK,
        BC2_SRGB_BLOCK,
        BC3_UNORM_BLOCK,
        BC3_SRGB_BLOCK,
        BC4_UNORM_BLOCK,
        BC4_SNORM_BLOCK,
        BC5_UNORM_BLOCK,
        BC5_SNORM_BLOCK,
        BC6H_UFLOAT_BLOCK,
        BC6H_SFLOAT_BLOCK,
        BC7_UNORM_BLOCK,
        BC7_SRGB_BLOCK,
#endif //GF_EXCLUDE_BC_COMPRESSED_FORMATS
#ifndef GF_EXCLUDE_ETC_COMPRESSED_FORMATS
        ETC2_R8G8B8_UNORM_BLOCK,
        ETC2_R8G8B8_SRGB_BLOCK,
        ETC2_R8G8B8A1_UNORM_BLOCK,
        ETC2_R8G8B8A1_SRGB_BLOCK,
        ETC2_R8G8B8A8_UNORM_BLOCK,
        ETC2_R8G8B8A8_SRGB_BLOCK,
        EAC_R11_UNORM_BLOCK,
        EAC_R11_SNORM_BLOCK,
        EAC_R11G11_UNORM_BLOCK,
        EAC_R11G11_SNORM_BLOCK,
#endif //GF_EXCLUDE_ETC_COMPRESSED_FORMATS
#ifndef GF_EXCLUDE_ASTC_COMPRESSED_FORMATS
        ASTC_4x4_UNORM_BLOCK,
        ASTC_4x4_SRGB_BLOCK,
        ASTC_5x4_UNORM_BLOCK,
        ASTC_5x4_SRGB_BLOCK,
        ASTC_5x5_UNORM_BLOCK,
        ASTC_5x5_SRGB_BLOCK,
        ASTC_6x5_UNORM_BLOCK,
        ASTC_6x5_SRGB_BLOCK,
        ASTC_6x6_UNORM_BLOCK,
        ASTC_6x6_SRGB_BLOCK,
        ASTC_8x5_UNORM_BLOCK,
        ASTC_8x5_SRGB_BLOCK,
        ASTC_8x6_UNORM_BLOCK,
        ASTC_8x6_SRGB_BLOCK,
        ASTC_8x8_UNORM_BLOCK,
        ASTC_8x8_SRGB_BLOCK,
        ASTC_10x5_UNORM_BLOCK,
        ASTC_10x5_SRGB_BLOCK,
        ASTC_10x6_UNORM_BLOCK,
        ASTC_10x6_SRGB_BLOCK,
        ASTC_10x8_UNORM_BLOCK,
        ASTC_10x8_SRGB_BLOCK,
        ASTC_10x10_UNORM_BLOCK,
        ASTC_10x10_SRGB_BLOCK,
        ASTC_12x10_UNORM_BLOCK,
        ASTC_12x10_SRGB_BLOCK,
        ASTC_12x12_UNORM_BLOCK,
        ASTC_12x12_SRGB_BLOCK,
#endif //GF_EXCLUDE_ASTC_COMPRESSED_FORMATS
#ifndef GF_EXCLUDE_PVRTC_COMPRESSED_FORMATS
        PVRTC1_2BPP_UNORM_BLOCK_IMG,
        PVRTC1_4BPP_UNORM_BLOCK_IMG,
        PVRTC2_2BPP_UNORM_BLOCK_IMG,
        PVRTC2_4BPP_UNORM_BLOCK_IMG,
        PVRTC1_2BPP_SRGB_BLOCK_IMG,
        PVRTC1_4BPP_SRGB_BLOCK_IMG,
        PVRTC2_2BPP_SRGB_BLOCK_IMG,
        PVRTC2_4BPP_SRGB_BLOCK_IMG,
#endif //GF_EXCLUDE_PVRTC_COMPRESSED_FORMATS
#endif //GF_EXCLUDE_COMPRESSED_FORMATS
        Count,
        First = UNDEFINED,
        Last = Count - 1,
    };
}