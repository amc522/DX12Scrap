#pragma once

#include <gpufmt/format.h>

#include <optional>

#if __has_include(<vk_core.h>)
#include <vk_core.h>
#else
typedef enum VkFormat {
    VK_FORMAT_UNDEFINED = 0,
    VK_FORMAT_R4G4_UNORM_PACK8 = 1,
    VK_FORMAT_R4G4B4A4_UNORM_PACK16 = 2,
    VK_FORMAT_B4G4R4A4_UNORM_PACK16 = 3,
    VK_FORMAT_R5G6B5_UNORM_PACK16 = 4,
    VK_FORMAT_B5G6R5_UNORM_PACK16 = 5,
    VK_FORMAT_R5G5B5A1_UNORM_PACK16 = 6,
    VK_FORMAT_B5G5R5A1_UNORM_PACK16 = 7,
    VK_FORMAT_A1R5G5B5_UNORM_PACK16 = 8,
    VK_FORMAT_R8_UNORM = 9,
    VK_FORMAT_R8_SNORM = 10,
    VK_FORMAT_R8_USCALED = 11,
    VK_FORMAT_R8_SSCALED = 12,
    VK_FORMAT_R8_UINT = 13,
    VK_FORMAT_R8_SINT = 14,
    VK_FORMAT_R8_SRGB = 15,
    VK_FORMAT_R8G8_UNORM = 16,
    VK_FORMAT_R8G8_SNORM = 17,
    VK_FORMAT_R8G8_USCALED = 18,
    VK_FORMAT_R8G8_SSCALED = 19,
    VK_FORMAT_R8G8_UINT = 20,
    VK_FORMAT_R8G8_SINT = 21,
    VK_FORMAT_R8G8_SRGB = 22,
    VK_FORMAT_R8G8B8_UNORM = 23,
    VK_FORMAT_R8G8B8_SNORM = 24,
    VK_FORMAT_R8G8B8_USCALED = 25,
    VK_FORMAT_R8G8B8_SSCALED = 26,
    VK_FORMAT_R8G8B8_UINT = 27,
    VK_FORMAT_R8G8B8_SINT = 28,
    VK_FORMAT_R8G8B8_SRGB = 29,
    VK_FORMAT_B8G8R8_UNORM = 30,
    VK_FORMAT_B8G8R8_SNORM = 31,
    VK_FORMAT_B8G8R8_USCALED = 32,
    VK_FORMAT_B8G8R8_SSCALED = 33,
    VK_FORMAT_B8G8R8_UINT = 34,
    VK_FORMAT_B8G8R8_SINT = 35,
    VK_FORMAT_B8G8R8_SRGB = 36,
    VK_FORMAT_R8G8B8A8_UNORM = 37,
    VK_FORMAT_R8G8B8A8_SNORM = 38,
    VK_FORMAT_R8G8B8A8_USCALED = 39,
    VK_FORMAT_R8G8B8A8_SSCALED = 40,
    VK_FORMAT_R8G8B8A8_UINT = 41,
    VK_FORMAT_R8G8B8A8_SINT = 42,
    VK_FORMAT_R8G8B8A8_SRGB = 43,
    VK_FORMAT_B8G8R8A8_UNORM = 44,
    VK_FORMAT_B8G8R8A8_SNORM = 45,
    VK_FORMAT_B8G8R8A8_USCALED = 46,
    VK_FORMAT_B8G8R8A8_SSCALED = 47,
    VK_FORMAT_B8G8R8A8_UINT = 48,
    VK_FORMAT_B8G8R8A8_SINT = 49,
    VK_FORMAT_B8G8R8A8_SRGB = 50,
    VK_FORMAT_A8B8G8R8_UNORM_PACK32 = 51,
    VK_FORMAT_A8B8G8R8_SNORM_PACK32 = 52,
    VK_FORMAT_A8B8G8R8_USCALED_PACK32 = 53,
    VK_FORMAT_A8B8G8R8_SSCALED_PACK32 = 54,
    VK_FORMAT_A8B8G8R8_UINT_PACK32 = 55,
    VK_FORMAT_A8B8G8R8_SINT_PACK32 = 56,
    VK_FORMAT_A8B8G8R8_SRGB_PACK32 = 57,
    VK_FORMAT_A2R10G10B10_UNORM_PACK32 = 58,
    VK_FORMAT_A2R10G10B10_SNORM_PACK32 = 59,
    VK_FORMAT_A2R10G10B10_USCALED_PACK32 = 60,
    VK_FORMAT_A2R10G10B10_SSCALED_PACK32 = 61,
    VK_FORMAT_A2R10G10B10_UINT_PACK32 = 62,
    VK_FORMAT_A2R10G10B10_SINT_PACK32 = 63,
    VK_FORMAT_A2B10G10R10_UNORM_PACK32 = 64,
    VK_FORMAT_A2B10G10R10_SNORM_PACK32 = 65,
    VK_FORMAT_A2B10G10R10_USCALED_PACK32 = 66,
    VK_FORMAT_A2B10G10R10_SSCALED_PACK32 = 67,
    VK_FORMAT_A2B10G10R10_UINT_PACK32 = 68,
    VK_FORMAT_A2B10G10R10_SINT_PACK32 = 69,
    VK_FORMAT_R16_UNORM = 70,
    VK_FORMAT_R16_SNORM = 71,
    VK_FORMAT_R16_USCALED = 72,
    VK_FORMAT_R16_SSCALED = 73,
    VK_FORMAT_R16_UINT = 74,
    VK_FORMAT_R16_SINT = 75,
    VK_FORMAT_R16_SFLOAT = 76,
    VK_FORMAT_R16G16_UNORM = 77,
    VK_FORMAT_R16G16_SNORM = 78,
    VK_FORMAT_R16G16_USCALED = 79,
    VK_FORMAT_R16G16_SSCALED = 80,
    VK_FORMAT_R16G16_UINT = 81,
    VK_FORMAT_R16G16_SINT = 82,
    VK_FORMAT_R16G16_SFLOAT = 83,
    VK_FORMAT_R16G16B16_UNORM = 84,
    VK_FORMAT_R16G16B16_SNORM = 85,
    VK_FORMAT_R16G16B16_USCALED = 86,
    VK_FORMAT_R16G16B16_SSCALED = 87,
    VK_FORMAT_R16G16B16_UINT = 88,
    VK_FORMAT_R16G16B16_SINT = 89,
    VK_FORMAT_R16G16B16_SFLOAT = 90,
    VK_FORMAT_R16G16B16A16_UNORM = 91,
    VK_FORMAT_R16G16B16A16_SNORM = 92,
    VK_FORMAT_R16G16B16A16_USCALED = 93,
    VK_FORMAT_R16G16B16A16_SSCALED = 94,
    VK_FORMAT_R16G16B16A16_UINT = 95,
    VK_FORMAT_R16G16B16A16_SINT = 96,
    VK_FORMAT_R16G16B16A16_SFLOAT = 97,
    VK_FORMAT_R32_UINT = 98,
    VK_FORMAT_R32_SINT = 99,
    VK_FORMAT_R32_SFLOAT = 100,
    VK_FORMAT_R32G32_UINT = 101,
    VK_FORMAT_R32G32_SINT = 102,
    VK_FORMAT_R32G32_SFLOAT = 103,
    VK_FORMAT_R32G32B32_UINT = 104,
    VK_FORMAT_R32G32B32_SINT = 105,
    VK_FORMAT_R32G32B32_SFLOAT = 106,
    VK_FORMAT_R32G32B32A32_UINT = 107,
    VK_FORMAT_R32G32B32A32_SINT = 108,
    VK_FORMAT_R32G32B32A32_SFLOAT = 109,
    VK_FORMAT_R64_UINT = 110,
    VK_FORMAT_R64_SINT = 111,
    VK_FORMAT_R64_SFLOAT = 112,
    VK_FORMAT_R64G64_UINT = 113,
    VK_FORMAT_R64G64_SINT = 114,
    VK_FORMAT_R64G64_SFLOAT = 115,
    VK_FORMAT_R64G64B64_UINT = 116,
    VK_FORMAT_R64G64B64_SINT = 117,
    VK_FORMAT_R64G64B64_SFLOAT = 118,
    VK_FORMAT_R64G64B64A64_UINT = 119,
    VK_FORMAT_R64G64B64A64_SINT = 120,
    VK_FORMAT_R64G64B64A64_SFLOAT = 121,
    VK_FORMAT_B10G11R11_UFLOAT_PACK32 = 122,
    VK_FORMAT_E5B9G9R9_UFLOAT_PACK32 = 123,
    VK_FORMAT_D16_UNORM = 124,
    VK_FORMAT_X8_D24_UNORM_PACK32 = 125,
    VK_FORMAT_D32_SFLOAT = 126,
    VK_FORMAT_S8_UINT = 127,
    VK_FORMAT_D16_UNORM_S8_UINT = 128,
    VK_FORMAT_D24_UNORM_S8_UINT = 129,
    VK_FORMAT_D32_SFLOAT_S8_UINT = 130,
    VK_FORMAT_BC1_RGB_UNORM_BLOCK = 131,
    VK_FORMAT_BC1_RGB_SRGB_BLOCK = 132,
    VK_FORMAT_BC1_RGBA_UNORM_BLOCK = 133,
    VK_FORMAT_BC1_RGBA_SRGB_BLOCK = 134,
    VK_FORMAT_BC2_UNORM_BLOCK = 135,
    VK_FORMAT_BC2_SRGB_BLOCK = 136,
    VK_FORMAT_BC3_UNORM_BLOCK = 137,
    VK_FORMAT_BC3_SRGB_BLOCK = 138,
    VK_FORMAT_BC4_UNORM_BLOCK = 139,
    VK_FORMAT_BC4_SNORM_BLOCK = 140,
    VK_FORMAT_BC5_UNORM_BLOCK = 141,
    VK_FORMAT_BC5_SNORM_BLOCK = 142,
    VK_FORMAT_BC6H_UFLOAT_BLOCK = 143,
    VK_FORMAT_BC6H_SFLOAT_BLOCK = 144,
    VK_FORMAT_BC7_UNORM_BLOCK = 145,
    VK_FORMAT_BC7_SRGB_BLOCK = 146,
    VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK = 147,
    VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK = 148,
    VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK = 149,
    VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK = 150,
    VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK = 151,
    VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK = 152,
    VK_FORMAT_EAC_R11_UNORM_BLOCK = 153,
    VK_FORMAT_EAC_R11_SNORM_BLOCK = 154,
    VK_FORMAT_EAC_R11G11_UNORM_BLOCK = 155,
    VK_FORMAT_EAC_R11G11_SNORM_BLOCK = 156,
    VK_FORMAT_ASTC_4x4_UNORM_BLOCK = 157,
    VK_FORMAT_ASTC_4x4_SRGB_BLOCK = 158,
    VK_FORMAT_ASTC_5x4_UNORM_BLOCK = 159,
    VK_FORMAT_ASTC_5x4_SRGB_BLOCK = 160,
    VK_FORMAT_ASTC_5x5_UNORM_BLOCK = 161,
    VK_FORMAT_ASTC_5x5_SRGB_BLOCK = 162,
    VK_FORMAT_ASTC_6x5_UNORM_BLOCK = 163,
    VK_FORMAT_ASTC_6x5_SRGB_BLOCK = 164,
    VK_FORMAT_ASTC_6x6_UNORM_BLOCK = 165,
    VK_FORMAT_ASTC_6x6_SRGB_BLOCK = 166,
    VK_FORMAT_ASTC_8x5_UNORM_BLOCK = 167,
    VK_FORMAT_ASTC_8x5_SRGB_BLOCK = 168,
    VK_FORMAT_ASTC_8x6_UNORM_BLOCK = 169,
    VK_FORMAT_ASTC_8x6_SRGB_BLOCK = 170,
    VK_FORMAT_ASTC_8x8_UNORM_BLOCK = 171,
    VK_FORMAT_ASTC_8x8_SRGB_BLOCK = 172,
    VK_FORMAT_ASTC_10x5_UNORM_BLOCK = 173,
    VK_FORMAT_ASTC_10x5_SRGB_BLOCK = 174,
    VK_FORMAT_ASTC_10x6_UNORM_BLOCK = 175,
    VK_FORMAT_ASTC_10x6_SRGB_BLOCK = 176,
    VK_FORMAT_ASTC_10x8_UNORM_BLOCK = 177,
    VK_FORMAT_ASTC_10x8_SRGB_BLOCK = 178,
    VK_FORMAT_ASTC_10x10_UNORM_BLOCK = 179,
    VK_FORMAT_ASTC_10x10_SRGB_BLOCK = 180,
    VK_FORMAT_ASTC_12x10_UNORM_BLOCK = 181,
    VK_FORMAT_ASTC_12x10_SRGB_BLOCK = 182,
    VK_FORMAT_ASTC_12x12_UNORM_BLOCK = 183,
    VK_FORMAT_ASTC_12x12_SRGB_BLOCK = 184,
    VK_FORMAT_G8B8G8R8_422_UNORM = 1000156000,
    VK_FORMAT_B8G8R8G8_422_UNORM = 1000156001,
    VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM = 1000156002,
    VK_FORMAT_G8_B8R8_2PLANE_420_UNORM = 1000156003,
    VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM = 1000156004,
    VK_FORMAT_G8_B8R8_2PLANE_422_UNORM = 1000156005,
    VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM = 1000156006,
    VK_FORMAT_R10X6_UNORM_PACK16 = 1000156007,
    VK_FORMAT_R10X6G10X6_UNORM_2PACK16 = 1000156008,
    VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16 = 1000156009,
    VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16 = 1000156010,
    VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16 = 1000156011,
    VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16 = 1000156012,
    VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16 = 1000156013,
    VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16 = 1000156014,
    VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16 = 1000156015,
    VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16 = 1000156016,
    VK_FORMAT_R12X4_UNORM_PACK16 = 1000156017,
    VK_FORMAT_R12X4G12X4_UNORM_2PACK16 = 1000156018,
    VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16 = 1000156019,
    VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16 = 1000156020,
    VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16 = 1000156021,
    VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16 = 1000156022,
    VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16 = 1000156023,
    VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16 = 1000156024,
    VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16 = 1000156025,
    VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16 = 1000156026,
    VK_FORMAT_G16B16G16R16_422_UNORM = 1000156027,
    VK_FORMAT_B16G16R16G16_422_UNORM = 1000156028,
    VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM = 1000156029,
    VK_FORMAT_G16_B16R16_2PLANE_420_UNORM = 1000156030,
    VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM = 1000156031,
    VK_FORMAT_G16_B16R16_2PLANE_422_UNORM = 1000156032,
    VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM = 1000156033,
    VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG = 1000054000,
    VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG = 1000054001,
    VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG = 1000054002,
    VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG = 1000054003,
    VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG = 1000054004,
    VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG = 1000054005,
    VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG = 1000054006,
    VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG = 1000054007,
    VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK_EXT = 1000066000,
    VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK_EXT = 1000066001,
    VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK_EXT = 1000066002,
    VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK_EXT = 1000066003,
    VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK_EXT = 1000066004,
    VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK_EXT = 1000066005,
    VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK_EXT = 1000066006,
    VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK_EXT = 1000066007,
    VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK_EXT = 1000066008,
    VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK_EXT = 1000066009,
    VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK_EXT = 1000066010,
    VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK_EXT = 1000066011,
    VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK_EXT = 1000066012,
    VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK_EXT = 1000066013,
    VK_FORMAT_G8B8G8R8_422_UNORM_KHR = VK_FORMAT_G8B8G8R8_422_UNORM,
    VK_FORMAT_B8G8R8G8_422_UNORM_KHR = VK_FORMAT_B8G8R8G8_422_UNORM,
    VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM_KHR = VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM,
    VK_FORMAT_G8_B8R8_2PLANE_420_UNORM_KHR = VK_FORMAT_G8_B8R8_2PLANE_420_UNORM,
    VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM_KHR = VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM,
    VK_FORMAT_G8_B8R8_2PLANE_422_UNORM_KHR = VK_FORMAT_G8_B8R8_2PLANE_422_UNORM,
    VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM_KHR = VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM,
    VK_FORMAT_R10X6_UNORM_PACK16_KHR = VK_FORMAT_R10X6_UNORM_PACK16,
    VK_FORMAT_R10X6G10X6_UNORM_2PACK16_KHR = VK_FORMAT_R10X6G10X6_UNORM_2PACK16,
    VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16_KHR = VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16,
    VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16_KHR = VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16,
    VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16_KHR = VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16,
    VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16_KHR = VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16,
    VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16_KHR = VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16,
    VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16_KHR = VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16,
    VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16_KHR = VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16,
    VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16_KHR = VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16,
    VK_FORMAT_R12X4_UNORM_PACK16_KHR = VK_FORMAT_R12X4_UNORM_PACK16,
    VK_FORMAT_R12X4G12X4_UNORM_2PACK16_KHR = VK_FORMAT_R12X4G12X4_UNORM_2PACK16,
    VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16_KHR = VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16,
    VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16_KHR = VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16,
    VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16_KHR = VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16,
    VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16_KHR = VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16,
    VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16_KHR = VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16,
    VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16_KHR = VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16,
    VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16_KHR = VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16,
    VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16_KHR = VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16,
    VK_FORMAT_G16B16G16R16_422_UNORM_KHR = VK_FORMAT_G16B16G16R16_422_UNORM,
    VK_FORMAT_B16G16R16G16_422_UNORM_KHR = VK_FORMAT_B16G16R16G16_422_UNORM,
    VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM_KHR = VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM,
    VK_FORMAT_G16_B16R16_2PLANE_420_UNORM_KHR = VK_FORMAT_G16_B16R16_2PLANE_420_UNORM,
    VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM_KHR = VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM,
    VK_FORMAT_G16_B16R16_2PLANE_422_UNORM_KHR = VK_FORMAT_G16_B16R16_2PLANE_422_UNORM,
    VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM_KHR = VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM,
    VK_FORMAT_BEGIN_RANGE = VK_FORMAT_UNDEFINED,
    VK_FORMAT_END_RANGE = VK_FORMAT_ASTC_12x12_SRGB_BLOCK,
    VK_FORMAT_RANGE_SIZE = (VK_FORMAT_ASTC_12x12_SRGB_BLOCK - VK_FORMAT_UNDEFINED + 1),
    VK_FORMAT_MAX_ENUM = 0x7FFFFFFF
} VkFormat;
#endif

namespace gpufmt::vulkan {
    struct FormatConversion {
        std::optional<VkFormat> exact;
        std::optional<VkFormat> closest;

        [[nodiscard]]
        constexpr operator bool() const noexcept {
            return exact || closest;
        }
    };

    [[nodiscard]]
    constexpr FormatConversion translateFormat(Format format) noexcept {
        FormatConversion conversion;

        switch(format)
        {
        case gpufmt::Format::UNDEFINED:
            conversion.exact = VK_FORMAT_UNDEFINED;
            break;
        case gpufmt::Format::R4G4_UNORM_PACK8:
            conversion.exact = VK_FORMAT_R4G4_UNORM_PACK8;
            break;
        case gpufmt::Format::R4G4B4A4_UNORM_PACK16:
            conversion.exact = VK_FORMAT_R4G4B4A4_UNORM_PACK16;
            break;
        case gpufmt::Format::B4G4R4A4_UNORM_PACK16:
            conversion.exact = VK_FORMAT_B4G4R4A4_UNORM_PACK16;
            break;
        case gpufmt::Format::A4R4G4B4_UNORM_PACK16:
            conversion.closest = VK_FORMAT_B4G4R4A4_UNORM_PACK16;
            break;
        case gpufmt::Format::R5G6B5_UNORM_PACK16:
            conversion.exact = VK_FORMAT_R5G6B5_UNORM_PACK16;
            break;
        case gpufmt::Format::B5G6R5_UNORM_PACK16:
            conversion.exact = VK_FORMAT_B5G6R5_UNORM_PACK16;
            break;
        case gpufmt::Format::R5G5B5A1_UNORM_PACK16:
            conversion.exact = VK_FORMAT_R5G5B5A1_UNORM_PACK16;
            break;
        case gpufmt::Format::B5G5R5A1_UNORM_PACK16:
            conversion.exact = VK_FORMAT_B5G5R5A1_UNORM_PACK16;
            break;
        case gpufmt::Format::A1R5G5B5_UNORM_PACK16:
            conversion.exact = VK_FORMAT_A1R5G5B5_UNORM_PACK16;
            break;
        case gpufmt::Format::R8_UNORM:
            conversion.exact = VK_FORMAT_R8_UNORM;
            break;
        case gpufmt::Format::R8_SNORM:
            conversion.exact = VK_FORMAT_R8_SNORM;
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::R8_USCALED:
            conversion.exact = VK_FORMAT_R8_USCALED;
            break;
        case gpufmt::Format::R8_SSCALED:
            conversion.exact = VK_FORMAT_R8_SSCALED;
            break;
#endif
        case gpufmt::Format::R8_UINT:
            conversion.exact = VK_FORMAT_R8_UINT;
            break;
        case gpufmt::Format::R8_SINT:
            conversion.exact = VK_FORMAT_R8_SINT;
            break;
        case gpufmt::Format::R8_SRGB:
            conversion.exact = VK_FORMAT_R8_SRGB;
            break;
        case gpufmt::Format::A8_UNORM:
            conversion.closest = VK_FORMAT_R8G8B8A8_UNORM;
            break;
        case gpufmt::Format::R8G8_UNORM:
            conversion.exact = VK_FORMAT_R8G8_UNORM;
            break;
        case gpufmt::Format::R8G8_SNORM:
            conversion.exact = VK_FORMAT_R8G8_SNORM;
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::R8G8_USCALED:
            conversion.exact = VK_FORMAT_R8G8_USCALED;
            break;
        case gpufmt::Format::R8G8_SSCALED:
            conversion.exact = VK_FORMAT_R8G8_SSCALED;
            break;
#endif
        case gpufmt::Format::R8G8_UINT:
            conversion.exact = VK_FORMAT_R8G8_UINT;
            break;
        case gpufmt::Format::R8G8_SINT:
            conversion.exact = VK_FORMAT_R8G8_SINT;
            break;
        case gpufmt::Format::R8G8_SRGB:
            conversion.exact = VK_FORMAT_R8G8_SRGB;
            break;
        case gpufmt::Format::R8G8B8_UNORM:
            conversion.exact = VK_FORMAT_R8G8B8_UNORM;
            break;
        case gpufmt::Format::R8G8B8_SNORM:
            conversion.exact = VK_FORMAT_R8G8B8_SNORM;
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::R8G8B8_USCALED:
            conversion.exact = VK_FORMAT_R8G8B8_USCALED;
            break;
        case gpufmt::Format::R8G8B8_SSCALED:
            conversion.exact = VK_FORMAT_R8G8B8_SSCALED;
            break;
#endif
        case gpufmt::Format::R8G8B8_UINT:
            conversion.exact = VK_FORMAT_R8G8B8_UINT;
            break;
        case gpufmt::Format::R8G8B8_SINT:
            conversion.exact = VK_FORMAT_R8G8B8_SINT;
            break;
        case gpufmt::Format::R8G8B8_SRGB:
            conversion.exact = VK_FORMAT_R8G8B8_SRGB;
            break;
        case gpufmt::Format::B8G8R8_UNORM:
            conversion.exact = VK_FORMAT_B8G8R8_UNORM;
            break;
        case gpufmt::Format::B8G8R8_SNORM:
            conversion.exact = VK_FORMAT_B8G8R8_SNORM;
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::B8G8R8_USCALED:
            conversion.exact = VK_FORMAT_B8G8R8_USCALED;
            break;
        case gpufmt::Format::B8G8R8_SSCALED:
            conversion.exact = VK_FORMAT_B8G8R8_SSCALED;
            break;
#endif
        case gpufmt::Format::B8G8R8_UINT:
            conversion.exact = VK_FORMAT_B8G8R8_UINT;
            break;
        case gpufmt::Format::B8G8R8_SINT:
            conversion.exact = VK_FORMAT_B8G8R8_SINT;
            break;
        case gpufmt::Format::B8G8R8_SRGB:
            conversion.exact = VK_FORMAT_B8G8R8_SRGB;
            break;
        case gpufmt::Format::R8G8B8A8_UNORM:
            conversion.exact = VK_FORMAT_R8G8B8A8_UNORM;
            break;
        case gpufmt::Format::R8G8B8A8_SNORM:
            conversion.exact = VK_FORMAT_R8G8B8A8_SNORM;
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::R8G8B8A8_USCALED:
            conversion.exact = VK_FORMAT_R8G8B8A8_USCALED;
            break;
        case gpufmt::Format::R8G8B8A8_SSCALED:
            conversion.exact = VK_FORMAT_R8G8B8A8_SSCALED;
            break;
#endif
        case gpufmt::Format::R8G8B8A8_UINT:
            conversion.exact = VK_FORMAT_R8G8B8A8_UINT;
            break;
        case gpufmt::Format::R8G8B8A8_SINT:
            conversion.exact = VK_FORMAT_R8G8B8A8_SINT;
            break;
        case gpufmt::Format::R8G8B8A8_SRGB:
            conversion.exact = VK_FORMAT_R8G8B8A8_SRGB;
            break;
        case gpufmt::Format::B8G8R8A8_UNORM:
            conversion.exact = VK_FORMAT_B8G8R8A8_UNORM;
            break;
        case gpufmt::Format::B8G8R8A8_SNORM:
            conversion.exact = VK_FORMAT_B8G8R8A8_SNORM;
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::B8G8R8A8_USCALED:
            conversion.exact = VK_FORMAT_B8G8R8A8_USCALED;
            break;
        case gpufmt::Format::B8G8R8A8_SSCALED:
            conversion.exact = VK_FORMAT_B8G8R8A8_SSCALED;
            break;
#endif
        case gpufmt::Format::B8G8R8A8_UINT:
            conversion.exact = VK_FORMAT_B8G8R8A8_UINT;
            break;
        case gpufmt::Format::B8G8R8A8_SINT:
            conversion.exact = VK_FORMAT_B8G8R8A8_SINT;
            break;
        case gpufmt::Format::B8G8R8A8_SRGB:
            conversion.exact = VK_FORMAT_B8G8R8A8_SRGB;
            break;
        case gpufmt::Format::B8G8R8X8_UNORM:
            conversion.closest = VK_FORMAT_B8G8R8A8_UNORM;
            break;
        case gpufmt::Format::B8G8R8X8_SRGB:
            conversion.closest = VK_FORMAT_B8G8R8A8_SRGB;
            break;
        case gpufmt::Format::A8B8G8R8_UNORM_PACK32:
            conversion.exact = VK_FORMAT_A8B8G8R8_UNORM_PACK32;
            break;
        case gpufmt::Format::A8B8G8R8_SNORM_PACK32:
            conversion.exact = VK_FORMAT_A8B8G8R8_SNORM_PACK32;
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::A8B8G8R8_USCALED_PACK32:
            conversion.exact = VK_FORMAT_A8B8G8R8_USCALED_PACK32;
            break;
        case gpufmt::Format::A8B8G8R8_SSCALED_PACK32:
            conversion.exact = VK_FORMAT_A8B8G8R8_SSCALED_PACK32;
            break;
#endif
        case gpufmt::Format::A8B8G8R8_UINT_PACK32:
            conversion.exact = VK_FORMAT_A8B8G8R8_UINT_PACK32;
            break;
        case gpufmt::Format::A8B8G8R8_SINT_PACK32:
            conversion.exact = VK_FORMAT_A8B8G8R8_SINT_PACK32;
            break;
        case gpufmt::Format::A8B8G8R8_SRGB_PACK32:
            conversion.exact = VK_FORMAT_A8B8G8R8_SRGB_PACK32;
            break;
        case gpufmt::Format::A2R10G10B10_UNORM_PACK32:
            conversion.exact = VK_FORMAT_A2R10G10B10_UNORM_PACK32;
            break;
        case gpufmt::Format::A2R10G10B10_SNORM_PACK32:
            conversion.exact = VK_FORMAT_A2R10G10B10_SNORM_PACK32;
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::A2R10G10B10_USCALED_PACK32:
            conversion.exact = VK_FORMAT_A2R10G10B10_USCALED_PACK32;
            break;
        case gpufmt::Format::A2R10G10B10_SSCALED_PACK32:
            conversion.exact = VK_FORMAT_A2R10G10B10_SSCALED_PACK32;
            break;
#endif
        case gpufmt::Format::A2R10G10B10_UINT_PACK32:
            conversion.exact = VK_FORMAT_A2R10G10B10_UINT_PACK32;
            break;
        case gpufmt::Format::A2R10G10B10_SINT_PACK32:
            conversion.exact = VK_FORMAT_A2R10G10B10_SINT_PACK32;
            break;
        case gpufmt::Format::A2B10G10R10_UNORM_PACK32:
            conversion.exact = VK_FORMAT_A2B10G10R10_UNORM_PACK32;
            break;
        case gpufmt::Format::A2B10G10R10_SNORM_PACK32:
            conversion.exact = VK_FORMAT_A2B10G10R10_SNORM_PACK32;
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::A2B10G10R10_USCALED_PACK32:
            conversion.exact = VK_FORMAT_A2B10G10R10_USCALED_PACK32;
            break;
        case gpufmt::Format::A2B10G10R10_SSCALED_PACK32:
            conversion.exact = VK_FORMAT_A2B10G10R10_SSCALED_PACK32;
            break;
#endif
        case gpufmt::Format::A2B10G10R10_UINT_PACK32:
            conversion.exact = VK_FORMAT_A2B10G10R10_UINT_PACK32;
            break;
        case gpufmt::Format::A2B10G10R10_SINT_PACK32:
            conversion.exact = VK_FORMAT_A2B10G10R10_SINT_PACK32;
            break;
        case gpufmt::Format::R16_UNORM:
            conversion.exact = VK_FORMAT_R16_UNORM;
            break;
        case gpufmt::Format::R16_SNORM:
            conversion.exact = VK_FORMAT_R16_SNORM;
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::R16_USCALED:
            conversion.exact = VK_FORMAT_R16_USCALED;
            break;
        case gpufmt::Format::R16_SSCALED:
            conversion.exact = VK_FORMAT_R16_SSCALED;
            break;
#endif
        case gpufmt::Format::R16_UINT:
            conversion.exact = VK_FORMAT_R16_UINT;
            break;
        case gpufmt::Format::R16_SINT:
            conversion.exact = VK_FORMAT_R16_SINT;
            break;
        case gpufmt::Format::R16_SFLOAT:
            conversion.exact = VK_FORMAT_R16_SFLOAT;
            break;
        case gpufmt::Format::R16G16_UNORM:
            conversion.exact = VK_FORMAT_R16G16_UNORM;
            break;
        case gpufmt::Format::R16G16_SNORM:
            conversion.exact = VK_FORMAT_R16G16_SNORM;
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::R16G16_USCALED:
            conversion.exact = VK_FORMAT_R16G16_USCALED;
            break;
        case gpufmt::Format::R16G16_SSCALED:
            conversion.exact = VK_FORMAT_R16G16_SSCALED;
            break;
#endif
        case gpufmt::Format::R16G16_UINT:
            conversion.exact = VK_FORMAT_R16G16_UINT;
            break;
        case gpufmt::Format::R16G16_SINT:
            conversion.exact = VK_FORMAT_R16G16_SINT;
            break;
        case gpufmt::Format::R16G16_SFLOAT:
            conversion.exact = VK_FORMAT_R16G16_SFLOAT;
            break;
        case gpufmt::Format::R16G16B16_UNORM:
            conversion.exact = VK_FORMAT_R16G16B16_UNORM;
            break;
        case gpufmt::Format::R16G16B16_SNORM:
            conversion.exact = VK_FORMAT_R16G16B16_SNORM;
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::R16G16B16_USCALED:
            conversion.exact = VK_FORMAT_R16G16B16_USCALED;
            break;
        case gpufmt::Format::R16G16B16_SSCALED:
            conversion.exact = VK_FORMAT_R16G16B16_SSCALED;
            break;
#endif
        case gpufmt::Format::R16G16B16_UINT:
            conversion.exact = VK_FORMAT_R16G16B16_UINT;
            break;
        case gpufmt::Format::R16G16B16_SINT:
            conversion.exact = VK_FORMAT_R16G16B16_SINT;
            break;
        case gpufmt::Format::R16G16B16_SFLOAT:
            conversion.exact = VK_FORMAT_R16G16B16_SFLOAT;
            break;
        case gpufmt::Format::R16G16B16A16_UNORM:
            conversion.exact = VK_FORMAT_R16G16B16A16_UNORM;
            break;
        case gpufmt::Format::R16G16B16A16_SNORM:
            conversion.exact = VK_FORMAT_R16G16B16A16_SNORM;
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::R16G16B16A16_USCALED:
            conversion.exact = VK_FORMAT_R16G16B16A16_USCALED;
            break;
        case gpufmt::Format::R16G16B16A16_SSCALED:
            conversion.exact = VK_FORMAT_R16G16B16A16_SSCALED;
            break;
#endif
        case gpufmt::Format::R16G16B16A16_UINT:
            conversion.exact = VK_FORMAT_R16G16B16A16_UINT;
            break;
        case gpufmt::Format::R16G16B16A16_SINT:
            conversion.exact = VK_FORMAT_R16G16B16A16_SINT;
            break;
        case gpufmt::Format::R16G16B16A16_SFLOAT:
            conversion.exact = VK_FORMAT_R16G16B16A16_SFLOAT;
            break;
        case gpufmt::Format::R32_UINT:
            conversion.exact = VK_FORMAT_R32_UINT;
            break;
        case gpufmt::Format::R32_SINT:
            conversion.exact = VK_FORMAT_R32_SINT;
            break;
        case gpufmt::Format::R32_SFLOAT:
            conversion.exact = VK_FORMAT_R32_SFLOAT;
            break;
        case gpufmt::Format::R32G32_UINT:
            conversion.exact = VK_FORMAT_R32G32_UINT;
            break;
        case gpufmt::Format::R32G32_SINT:
            conversion.exact = VK_FORMAT_R32G32_SINT;
            break;
        case gpufmt::Format::R32G32_SFLOAT:
            conversion.exact = VK_FORMAT_R32G32_SFLOAT;
            break;
        case gpufmt::Format::R32G32B32_UINT:
            conversion.exact = VK_FORMAT_R32G32B32_UINT;
            break;
        case gpufmt::Format::R32G32B32_SINT:
            conversion.exact = VK_FORMAT_R32G32B32_SINT;
            break;
        case gpufmt::Format::R32G32B32_SFLOAT:
            conversion.exact = VK_FORMAT_R32G32B32_SFLOAT;
            break;
        case gpufmt::Format::R32G32B32A32_UINT:
            conversion.exact = VK_FORMAT_R32G32B32A32_UINT;
            break;
        case gpufmt::Format::R32G32B32A32_SINT:
            conversion.exact = VK_FORMAT_R32G32B32A32_SINT;
            break;
        case gpufmt::Format::R32G32B32A32_SFLOAT:
            conversion.exact = VK_FORMAT_R32G32B32A32_SFLOAT;
            break;
#ifndef GF_EXCLUDE_64BIT_FORMATS
        case gpufmt::Format::R64_UINT:
            conversion.exact = VK_FORMAT_R64_UINT;
            break;
        case gpufmt::Format::R64_SINT:
            conversion.exact = VK_FORMAT_R64_SINT;
            break;
        case gpufmt::Format::R64_SFLOAT:
            conversion.exact = VK_FORMAT_R64_SFLOAT;
            break;
        case gpufmt::Format::R64G64_UINT:
            conversion.exact = VK_FORMAT_R64G64_UINT;
            break;
        case gpufmt::Format::R64G64_SINT:
            conversion.exact = VK_FORMAT_R64G64_SINT;
            break;
        case gpufmt::Format::R64G64_SFLOAT:
            conversion.exact = VK_FORMAT_R64G64_SFLOAT;
            break;
        case gpufmt::Format::R64G64B64_UINT:
            conversion.exact = VK_FORMAT_R64G64B64_UINT;
            break;
        case gpufmt::Format::R64G64B64_SINT:
            conversion.exact = VK_FORMAT_R64G64B64_SINT;
            break;
        case gpufmt::Format::R64G64B64_SFLOAT:
            conversion.exact = VK_FORMAT_R64G64B64_SFLOAT;
            break;
        case gpufmt::Format::R64G64B64A64_UINT:
            conversion.exact = VK_FORMAT_R64G64B64A64_UINT;
            break;
        case gpufmt::Format::R64G64B64A64_SINT:
            conversion.exact = VK_FORMAT_R64G64B64A64_SINT;
            break;
        case gpufmt::Format::R64G64B64A64_SFLOAT:
            conversion.exact = VK_FORMAT_R64G64B64A64_SFLOAT;
            break;
#endif // GF_EXCLUDE_64BIT_FORMATS
        case gpufmt::Format::B10G11R11_UFLOAT_PACK32:
            conversion.exact = VK_FORMAT_B10G11R11_UFLOAT_PACK32;
            break;
        case gpufmt::Format::E5B9G9R9_UFLOAT_PACK32:
            conversion.exact = VK_FORMAT_E5B9G9R9_UFLOAT_PACK32;
            break;
        case gpufmt::Format::D16_UNORM:
            conversion.exact = VK_FORMAT_D16_UNORM;
            break;
        case gpufmt::Format::X8_D24_UNORM_PACK32:
            conversion.exact = VK_FORMAT_X8_D24_UNORM_PACK32;
            break;
        case gpufmt::Format::D32_SFLOAT:
            conversion.exact = VK_FORMAT_D32_SFLOAT;
            break;
        case gpufmt::Format::S8_UINT:
            conversion.exact = VK_FORMAT_S8_UINT;
            break;
        case gpufmt::Format::D16_UNORM_S8_UINT:
            conversion.exact = VK_FORMAT_D16_UNORM_S8_UINT;
            break;
        case gpufmt::Format::D24_UNORM_S8_UINT:
            conversion.exact = VK_FORMAT_D24_UNORM_S8_UINT;
            break;
        case gpufmt::Format::D32_SFLOAT_S8_UINT:
            conversion.exact = VK_FORMAT_D32_SFLOAT_S8_UINT;
            break;
#ifndef GF_EXCLUDE_COMPRESSED_FORMATS
#ifndef GF_EXCLUDE_BC_COMPRESSED_FORMATS
        case gpufmt::Format::BC1_RGB_UNORM_BLOCK:
            conversion.exact = VK_FORMAT_BC1_RGB_UNORM_BLOCK;
            break;
        case gpufmt::Format::BC1_RGB_SRGB_BLOCK:
            conversion.exact = VK_FORMAT_BC1_RGB_SRGB_BLOCK;
            break;
        case gpufmt::Format::BC1_RGBA_UNORM_BLOCK:
            conversion.exact = VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
            break;
        case gpufmt::Format::BC1_RGBA_SRGB_BLOCK:
            conversion.exact = VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
            break;
        case gpufmt::Format::BC2_UNORM_BLOCK:
            conversion.exact = VK_FORMAT_BC2_UNORM_BLOCK;
            break;
        case gpufmt::Format::BC2_SRGB_BLOCK:
            conversion.exact = VK_FORMAT_BC2_SRGB_BLOCK;
            break;
        case gpufmt::Format::BC3_UNORM_BLOCK:
            conversion.exact = VK_FORMAT_BC3_UNORM_BLOCK;
            break;
        case gpufmt::Format::BC3_SRGB_BLOCK:
            conversion.exact = VK_FORMAT_BC3_SRGB_BLOCK;
            break;
        case gpufmt::Format::BC4_UNORM_BLOCK:
            conversion.exact = VK_FORMAT_BC4_UNORM_BLOCK;
            break;
        case gpufmt::Format::BC4_SNORM_BLOCK:
            conversion.exact = VK_FORMAT_BC4_SNORM_BLOCK;
            break;
        case gpufmt::Format::BC5_UNORM_BLOCK:
            conversion.exact = VK_FORMAT_BC5_UNORM_BLOCK;
            break;
        case gpufmt::Format::BC5_SNORM_BLOCK:
            conversion.exact = VK_FORMAT_BC5_SNORM_BLOCK;
            break;
        case gpufmt::Format::BC6H_UFLOAT_BLOCK:
            conversion.exact = VK_FORMAT_BC6H_UFLOAT_BLOCK;
            break;
        case gpufmt::Format::BC6H_SFLOAT_BLOCK:
            conversion.exact = VK_FORMAT_BC6H_SFLOAT_BLOCK;
            break;
        case gpufmt::Format::BC7_UNORM_BLOCK:
            conversion.exact = VK_FORMAT_BC7_UNORM_BLOCK;
            break;
        case gpufmt::Format::BC7_SRGB_BLOCK:
            conversion.exact = VK_FORMAT_BC7_SRGB_BLOCK;
            break;
#endif //GF_EXCLUDE_BC_COMPRESSED_FORMATS
#ifndef GF_EXCLUDE_ETC_COMPRESSED_FORMATS
        case gpufmt::Format::ETC2_R8G8B8_UNORM_BLOCK:
            conversion.exact = VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK;
            break;
        case gpufmt::Format::ETC2_R8G8B8_SRGB_BLOCK:
            conversion.exact = VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK;
            break;
        case gpufmt::Format::ETC2_R8G8B8A1_UNORM_BLOCK:
            conversion.exact = VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK;
            break;
        case gpufmt::Format::ETC2_R8G8B8A1_SRGB_BLOCK:
            conversion.exact = VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK;
            break;
        case gpufmt::Format::ETC2_R8G8B8A8_UNORM_BLOCK:
            conversion.exact = VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK;
            break;
        case gpufmt::Format::ETC2_R8G8B8A8_SRGB_BLOCK:
            conversion.exact = VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK;
            break;
        case gpufmt::Format::EAC_R11_UNORM_BLOCK:
            conversion.exact = VK_FORMAT_EAC_R11_UNORM_BLOCK;
            break;
        case gpufmt::Format::EAC_R11_SNORM_BLOCK:
            conversion.exact = VK_FORMAT_EAC_R11_SNORM_BLOCK;
            break;
        case gpufmt::Format::EAC_R11G11_UNORM_BLOCK:
            conversion.exact = VK_FORMAT_EAC_R11G11_UNORM_BLOCK;
            break;
        case gpufmt::Format::EAC_R11G11_SNORM_BLOCK:
            conversion.exact = VK_FORMAT_EAC_R11G11_SNORM_BLOCK;
            break;
#endif //GF_EXCLUDE_ETC_COMPRESSED_FORMATS
#ifndef GF_EXCLUDE_ASTC_COMPRESSED_FORMATS
        case gpufmt::Format::ASTC_4x4_UNORM_BLOCK:
            conversion.exact = VK_FORMAT_ASTC_4x4_UNORM_BLOCK;
            break;
        case gpufmt::Format::ASTC_4x4_SRGB_BLOCK:
            conversion.exact = VK_FORMAT_ASTC_4x4_SRGB_BLOCK;
            break;
        case gpufmt::Format::ASTC_5x4_UNORM_BLOCK:
            conversion.exact = VK_FORMAT_ASTC_5x4_UNORM_BLOCK;
            break;
        case gpufmt::Format::ASTC_5x4_SRGB_BLOCK:
            conversion.exact = VK_FORMAT_ASTC_5x4_SRGB_BLOCK;
            break;
        case gpufmt::Format::ASTC_5x5_UNORM_BLOCK:
            conversion.exact = VK_FORMAT_ASTC_5x5_UNORM_BLOCK;
            break;
        case gpufmt::Format::ASTC_5x5_SRGB_BLOCK:
            conversion.exact = VK_FORMAT_ASTC_5x5_SRGB_BLOCK;
            break;
        case gpufmt::Format::ASTC_6x5_UNORM_BLOCK:
            conversion.exact = VK_FORMAT_ASTC_6x5_UNORM_BLOCK;
            break;
        case gpufmt::Format::ASTC_6x5_SRGB_BLOCK:
            conversion.exact = VK_FORMAT_ASTC_6x5_SRGB_BLOCK;
            break;
        case gpufmt::Format::ASTC_6x6_UNORM_BLOCK:
            conversion.exact = VK_FORMAT_ASTC_6x6_UNORM_BLOCK;
            break;
        case gpufmt::Format::ASTC_6x6_SRGB_BLOCK:
            conversion.exact = VK_FORMAT_ASTC_6x6_SRGB_BLOCK;
            break;
        case gpufmt::Format::ASTC_8x5_UNORM_BLOCK:
            conversion.exact = VK_FORMAT_ASTC_8x5_UNORM_BLOCK;
            break;
        case gpufmt::Format::ASTC_8x5_SRGB_BLOCK:
            conversion.exact = VK_FORMAT_ASTC_8x5_SRGB_BLOCK;
            break;
        case gpufmt::Format::ASTC_8x6_UNORM_BLOCK:
            conversion.exact = VK_FORMAT_ASTC_8x6_UNORM_BLOCK;
            break;
        case gpufmt::Format::ASTC_8x6_SRGB_BLOCK:
            conversion.exact = VK_FORMAT_ASTC_8x6_SRGB_BLOCK;
            break;
        case gpufmt::Format::ASTC_8x8_UNORM_BLOCK:
            conversion.exact = VK_FORMAT_ASTC_8x8_UNORM_BLOCK;
            break;
        case gpufmt::Format::ASTC_8x8_SRGB_BLOCK:
            conversion.exact = VK_FORMAT_ASTC_8x8_SRGB_BLOCK;
            break;
        case gpufmt::Format::ASTC_10x5_UNORM_BLOCK:
            conversion.exact = VK_FORMAT_ASTC_10x5_UNORM_BLOCK;
            break;
        case gpufmt::Format::ASTC_10x5_SRGB_BLOCK:
            conversion.exact = VK_FORMAT_ASTC_10x5_SRGB_BLOCK;
            break;
        case gpufmt::Format::ASTC_10x6_UNORM_BLOCK:
            conversion.exact = VK_FORMAT_ASTC_10x6_UNORM_BLOCK;
            break;
        case gpufmt::Format::ASTC_10x6_SRGB_BLOCK:
            conversion.exact = VK_FORMAT_ASTC_10x6_SRGB_BLOCK;
            break;
        case gpufmt::Format::ASTC_10x8_UNORM_BLOCK:
            conversion.exact = VK_FORMAT_ASTC_10x8_UNORM_BLOCK;
            break;
        case gpufmt::Format::ASTC_10x8_SRGB_BLOCK:
            conversion.exact = VK_FORMAT_ASTC_10x8_SRGB_BLOCK;
            break;
        case gpufmt::Format::ASTC_10x10_UNORM_BLOCK:
            conversion.exact = VK_FORMAT_ASTC_10x10_UNORM_BLOCK;
            break;
        case gpufmt::Format::ASTC_10x10_SRGB_BLOCK:
            conversion.exact = VK_FORMAT_ASTC_10x10_SRGB_BLOCK;
            break;
        case gpufmt::Format::ASTC_12x10_UNORM_BLOCK:
            conversion.exact = VK_FORMAT_ASTC_12x10_UNORM_BLOCK;
            break;
        case gpufmt::Format::ASTC_12x10_SRGB_BLOCK:
            conversion.exact = VK_FORMAT_ASTC_12x10_SRGB_BLOCK;
            break;
        case gpufmt::Format::ASTC_12x12_UNORM_BLOCK:
            conversion.exact = VK_FORMAT_ASTC_12x12_UNORM_BLOCK;
            break;
        case gpufmt::Format::ASTC_12x12_SRGB_BLOCK:
            conversion.exact = VK_FORMAT_ASTC_12x12_SRGB_BLOCK;
            break;
#endif //GF_EXCLUDE_ASTC_COMPRESSED_FORMATS
#ifndef GF_EXCLUDE_PVRTC_COMPRESSED_FORMATS
        case gpufmt::Format::PVRTC1_2BPP_UNORM_BLOCK_IMG:
            conversion.exact = VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG;
            break;
        case gpufmt::Format::PVRTC1_4BPP_UNORM_BLOCK_IMG:
            conversion.exact = VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG;
            break;
        case gpufmt::Format::PVRTC2_2BPP_UNORM_BLOCK_IMG:
            conversion.exact = VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG;
            break;
        case gpufmt::Format::PVRTC2_4BPP_UNORM_BLOCK_IMG:
            conversion.exact = VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG;
            break;
        case gpufmt::Format::PVRTC1_2BPP_SRGB_BLOCK_IMG:
            conversion.exact = VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG;
            break;
        case gpufmt::Format::PVRTC1_4BPP_SRGB_BLOCK_IMG:
            conversion.exact = VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG;
            break;
        case gpufmt::Format::PVRTC2_2BPP_SRGB_BLOCK_IMG:
            conversion.exact = VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG;
            break;
        case gpufmt::Format::PVRTC2_4BPP_SRGB_BLOCK_IMG:
            conversion.exact = VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG;
            break;
#endif //GF_EXCLUDE_PVRTC_COMPRESSED_FORMATS
#endif //GF_EXCLUDE_COMPRESSED_FORMATS
        default:
            break;
        }

        return conversion;
    }

    [[nodiscard]]
    static constexpr std::optional<gpufmt::Format> translateFormat(VkFormat vkFormat) noexcept {
        std::optional<gpufmt::Format> format;

        switch(vkFormat)
        {
        case VK_FORMAT_UNDEFINED:
            format = gpufmt::Format::UNDEFINED;
            break;
        case VK_FORMAT_R4G4_UNORM_PACK8:
            format = gpufmt::Format::R4G4_UNORM_PACK8;
            break;
        case VK_FORMAT_R4G4B4A4_UNORM_PACK16:
            format = gpufmt::Format::R4G4B4A4_UNORM_PACK16;
            break;
        case VK_FORMAT_B4G4R4A4_UNORM_PACK16:
            format = gpufmt::Format::B4G4R4A4_UNORM_PACK16;
            break;
        case VK_FORMAT_R5G6B5_UNORM_PACK16:
            format = gpufmt::Format::R5G6B5_UNORM_PACK16;
            break;
        case VK_FORMAT_B5G6R5_UNORM_PACK16:
            format = gpufmt::Format::B5G6R5_UNORM_PACK16;
            break;
        case VK_FORMAT_R5G5B5A1_UNORM_PACK16:
            format = gpufmt::Format::R5G5B5A1_UNORM_PACK16;
            break;
        case VK_FORMAT_B5G5R5A1_UNORM_PACK16:
            format = gpufmt::Format::B5G5R5A1_UNORM_PACK16;
            break;
        case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
            format = gpufmt::Format::A1R5G5B5_UNORM_PACK16;
            break;
        case VK_FORMAT_R8_UNORM:
            format = gpufmt::Format::R8_UNORM;
            break;
        case VK_FORMAT_R8_SNORM:
            format = gpufmt::Format::R8_SNORM;
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case VK_FORMAT_R8_USCALED:
            format = gpufmt::Format::R8_USCALED;
            break;
        case VK_FORMAT_R8_SSCALED:
            format = gpufmt::Format::R8_SSCALED;
            break;
#endif
        case VK_FORMAT_R8_UINT:
            format = gpufmt::Format::R8_UINT;
            break;
        case VK_FORMAT_R8_SINT:
            format = gpufmt::Format::R8_SINT;
            break;
        case VK_FORMAT_R8_SRGB:
            format = gpufmt::Format::R8_SRGB;
            break;
        case VK_FORMAT_R8G8_UNORM:
            format = gpufmt::Format::R8G8_UNORM;
            break;
        case VK_FORMAT_R8G8_SNORM:
            format = gpufmt::Format::R8G8_SNORM;
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case VK_FORMAT_R8G8_USCALED:
            format = gpufmt::Format::R8G8_USCALED;
            break;
        case VK_FORMAT_R8G8_SSCALED:
            format = gpufmt::Format::R8G8_SSCALED;
            break;
#endif
        case VK_FORMAT_R8G8_UINT:
            format = gpufmt::Format::R8G8_UINT;
            break;
        case VK_FORMAT_R8G8_SINT:
            format = gpufmt::Format::R8G8_SINT;
            break;
        case VK_FORMAT_R8G8_SRGB:
            format = gpufmt::Format::R8G8_SRGB;
            break;
        case VK_FORMAT_R8G8B8_UNORM:
            format = gpufmt::Format::R8G8B8_UNORM;
            break;
        case VK_FORMAT_R8G8B8_SNORM:
            format = gpufmt::Format::R8G8B8_SNORM;
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case VK_FORMAT_R8G8B8_USCALED:
            format = gpufmt::Format::R8G8B8_USCALED;
            break;
        case VK_FORMAT_R8G8B8_SSCALED:
            format = gpufmt::Format::R8G8B8_SSCALED;
            break;
#endif
        case VK_FORMAT_R8G8B8_UINT:
            format = gpufmt::Format::R8G8B8_UINT;
            break;
        case VK_FORMAT_R8G8B8_SINT:
            format = gpufmt::Format::R8G8B8_SINT;
            break;
        case VK_FORMAT_R8G8B8_SRGB:
            format = gpufmt::Format::R8G8B8_SRGB;
            break;
        case VK_FORMAT_B8G8R8_UNORM:
            format = gpufmt::Format::B8G8R8_UNORM;
            break;
        case VK_FORMAT_B8G8R8_SNORM:
            format = gpufmt::Format::B8G8R8_SNORM;
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case VK_FORMAT_B8G8R8_USCALED:
            format = gpufmt::Format::B8G8R8_USCALED;
            break;
        case VK_FORMAT_B8G8R8_SSCALED:
            format = gpufmt::Format::B8G8R8_SSCALED;
            break;
#endif
        case VK_FORMAT_B8G8R8_UINT:
            format = gpufmt::Format::B8G8R8_UINT;
            break;
        case VK_FORMAT_B8G8R8_SINT:
            format = gpufmt::Format::B8G8R8_SINT;
            break;
        case VK_FORMAT_B8G8R8_SRGB:
            format = gpufmt::Format::B8G8R8_SRGB;
            break;
        case VK_FORMAT_R8G8B8A8_UNORM:
            format = gpufmt::Format::R8G8B8A8_UNORM;
            break;
        case VK_FORMAT_R8G8B8A8_SNORM:
            format = gpufmt::Format::R8G8B8A8_SNORM;
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case VK_FORMAT_R8G8B8A8_USCALED:
            format = gpufmt::Format::R8G8B8A8_USCALED;
            break;
        case VK_FORMAT_R8G8B8A8_SSCALED:
            format = gpufmt::Format::R8G8B8A8_SSCALED;
            break;
#endif
        case VK_FORMAT_R8G8B8A8_UINT:
            format = gpufmt::Format::R8G8B8A8_UINT;
            break;
        case VK_FORMAT_R8G8B8A8_SINT:
            format = gpufmt::Format::R8G8B8A8_SINT;
            break;
        case VK_FORMAT_R8G8B8A8_SRGB:
            format = gpufmt::Format::R8G8B8A8_SRGB;
            break;
        case VK_FORMAT_B8G8R8A8_UNORM:
            format = gpufmt::Format::B8G8R8A8_UNORM;
            break;
        case VK_FORMAT_B8G8R8A8_SNORM:
            format = gpufmt::Format::B8G8R8A8_SNORM;
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case VK_FORMAT_B8G8R8A8_USCALED:
            format = gpufmt::Format::B8G8R8A8_USCALED;
            break;
        case VK_FORMAT_B8G8R8A8_SSCALED:
            format = gpufmt::Format::B8G8R8A8_SSCALED;
            break;
#endif
        case VK_FORMAT_B8G8R8A8_UINT:
            format = gpufmt::Format::B8G8R8A8_UINT;
            break;
        case VK_FORMAT_B8G8R8A8_SINT:
            format = gpufmt::Format::B8G8R8A8_SINT;
            break;
        case VK_FORMAT_B8G8R8A8_SRGB:
            format = gpufmt::Format::B8G8R8A8_SRGB;
            break;
        case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
            format = gpufmt::Format::A8B8G8R8_UNORM_PACK32;
            break;
        case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
            format = gpufmt::Format::A8B8G8R8_SNORM_PACK32;
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case VK_FORMAT_A8B8G8R8_USCALED_PACK32:
            format = gpufmt::Format::A8B8G8R8_USCALED_PACK32;
            break;
        case VK_FORMAT_A8B8G8R8_SSCALED_PACK32:
            format = gpufmt::Format::A8B8G8R8_SSCALED_PACK32;
            break;
#endif
        case VK_FORMAT_A8B8G8R8_UINT_PACK32:
            format = gpufmt::Format::A8B8G8R8_UINT_PACK32;
            break;
        case VK_FORMAT_A8B8G8R8_SINT_PACK32:
            format = gpufmt::Format::A8B8G8R8_SINT_PACK32;
            break;
        case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
            format = gpufmt::Format::A8B8G8R8_SRGB_PACK32;
            break;
        case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
            format = gpufmt::Format::A2R10G10B10_UNORM_PACK32;
            break;
        case VK_FORMAT_A2R10G10B10_SNORM_PACK32:
            format = gpufmt::Format::A2R10G10B10_SNORM_PACK32;
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case VK_FORMAT_A2R10G10B10_USCALED_PACK32:
            format = gpufmt::Format::A2R10G10B10_USCALED_PACK32;
            break;
        case VK_FORMAT_A2R10G10B10_SSCALED_PACK32:
            format = gpufmt::Format::A2R10G10B10_SSCALED_PACK32;
            break;
#endif
        case VK_FORMAT_A2R10G10B10_UINT_PACK32:
            format = gpufmt::Format::A2R10G10B10_UINT_PACK32;
            break;
        case VK_FORMAT_A2R10G10B10_SINT_PACK32:
            format = gpufmt::Format::A2R10G10B10_SINT_PACK32;
            break;
        case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
            format = gpufmt::Format::A2B10G10R10_UNORM_PACK32;
            break;
        case VK_FORMAT_A2B10G10R10_SNORM_PACK32:
            format = gpufmt::Format::A2B10G10R10_SNORM_PACK32;
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case VK_FORMAT_A2B10G10R10_USCALED_PACK32:
            format = gpufmt::Format::A2B10G10R10_USCALED_PACK32;
            break;
        case VK_FORMAT_A2B10G10R10_SSCALED_PACK32:
            format = gpufmt::Format::A2B10G10R10_SSCALED_PACK32;
            break;
#endif
        case VK_FORMAT_A2B10G10R10_UINT_PACK32:
            format = gpufmt::Format::A2B10G10R10_UINT_PACK32;
            break;
        case VK_FORMAT_A2B10G10R10_SINT_PACK32:
            format = gpufmt::Format::A2B10G10R10_SINT_PACK32;
            break;
        case VK_FORMAT_R16_UNORM:
            format = gpufmt::Format::R16_UNORM;
            break;
        case VK_FORMAT_R16_SNORM:
            format = gpufmt::Format::R16_SNORM;
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case VK_FORMAT_R16_USCALED:
            format = gpufmt::Format::R16_USCALED;
            break;
        case VK_FORMAT_R16_SSCALED:
            format = gpufmt::Format::R16_SSCALED;
            break;
#endif
        case VK_FORMAT_R16_UINT:
            format = gpufmt::Format::R16_UINT;
            break;
        case VK_FORMAT_R16_SINT:
            format = gpufmt::Format::R16_SINT;
            break;
        case VK_FORMAT_R16_SFLOAT:
            format = gpufmt::Format::R16_SFLOAT;
            break;
        case VK_FORMAT_R16G16_UNORM:
            format = gpufmt::Format::R16G16_UNORM;
            break;
        case VK_FORMAT_R16G16_SNORM:
            format = gpufmt::Format::R16G16_SNORM;
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case VK_FORMAT_R16G16_USCALED:
            format = gpufmt::Format::R16G16_USCALED;
            break;
        case VK_FORMAT_R16G16_SSCALED:
            format = gpufmt::Format::R16G16_SSCALED;
            break;
#endif
        case VK_FORMAT_R16G16_UINT:
            format = gpufmt::Format::R16G16_UINT;
            break;
        case VK_FORMAT_R16G16_SINT:
            format = gpufmt::Format::R16G16_SINT;
            break;
        case VK_FORMAT_R16G16_SFLOAT:
            format = gpufmt::Format::R16G16_SFLOAT;
            break;
        case VK_FORMAT_R16G16B16_UNORM:
            format = gpufmt::Format::R16G16B16_UNORM;
            break;
        case VK_FORMAT_R16G16B16_SNORM:
            format = gpufmt::Format::R16G16B16_SNORM;
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case VK_FORMAT_R16G16B16_USCALED:
            format = gpufmt::Format::R16G16B16_USCALED;
            break;
        case VK_FORMAT_R16G16B16_SSCALED:
            format = gpufmt::Format::R16G16B16_SSCALED;
            break;
#endif
        case VK_FORMAT_R16G16B16_UINT:
            format = gpufmt::Format::R16G16B16_UINT;
            break;
        case VK_FORMAT_R16G16B16_SINT:
            format = gpufmt::Format::R16G16B16_SINT;
            break;
        case VK_FORMAT_R16G16B16_SFLOAT:
            format = gpufmt::Format::R16G16B16_SFLOAT;
            break;
        case VK_FORMAT_R16G16B16A16_UNORM:
            format = gpufmt::Format::R16G16B16A16_UNORM;
            break;
        case VK_FORMAT_R16G16B16A16_SNORM:
            format = gpufmt::Format::R16G16B16A16_SNORM;
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case VK_FORMAT_R16G16B16A16_USCALED:
            format = gpufmt::Format::R16G16B16A16_USCALED;
            break;
        case VK_FORMAT_R16G16B16A16_SSCALED:
            format = gpufmt::Format::R16G16B16A16_SSCALED;
            break;
#endif
        case VK_FORMAT_R16G16B16A16_UINT:
            format = gpufmt::Format::R16G16B16A16_UINT;
            break;
        case VK_FORMAT_R16G16B16A16_SINT:
            format = gpufmt::Format::R16G16B16A16_SINT;
            break;
        case VK_FORMAT_R16G16B16A16_SFLOAT:
            format = gpufmt::Format::R16G16B16A16_SFLOAT;
            break;
        case VK_FORMAT_R32_UINT:
            format = gpufmt::Format::R32_UINT;
            break;
        case VK_FORMAT_R32_SINT:
            format = gpufmt::Format::R32_SINT;
            break;
        case VK_FORMAT_R32_SFLOAT:
            format = gpufmt::Format::R32_SFLOAT;
            break;
        case VK_FORMAT_R32G32_UINT:
            format = gpufmt::Format::R32G32_UINT;
            break;
        case VK_FORMAT_R32G32_SINT:
            format = gpufmt::Format::R32G32_SINT;
            break;
        case VK_FORMAT_R32G32_SFLOAT:
            format = gpufmt::Format::R32G32_SFLOAT;
            break;
        case VK_FORMAT_R32G32B32_UINT:
            format = gpufmt::Format::R32G32B32_UINT;
            break;
        case VK_FORMAT_R32G32B32_SINT:
            format = gpufmt::Format::R32G32B32_SINT;
            break;
        case VK_FORMAT_R32G32B32_SFLOAT:
            format = gpufmt::Format::R32G32B32_SFLOAT;
            break;
        case VK_FORMAT_R32G32B32A32_UINT:
            format = gpufmt::Format::R32G32B32A32_UINT;
            break;
        case VK_FORMAT_R32G32B32A32_SINT:
            format = gpufmt::Format::R32G32B32A32_SINT;
            break;
        case VK_FORMAT_R32G32B32A32_SFLOAT:
            format = gpufmt::Format::R32G32B32A32_SFLOAT;
            break;
#ifndef GF_EXCLUDE_64BIT_FORMATS
        case VK_FORMAT_R64_UINT:
            format = gpufmt::Format::R64_UINT;
            break;
        case VK_FORMAT_R64_SINT:
            format = gpufmt::Format::R64_SINT;
            break;
        case VK_FORMAT_R64_SFLOAT:
            format = gpufmt::Format::R64_SFLOAT;
            break;
        case VK_FORMAT_R64G64_UINT:
            format = gpufmt::Format::R64G64_UINT;
            break;
        case VK_FORMAT_R64G64_SINT:
            format = gpufmt::Format::R64G64_SINT;
            break;
        case VK_FORMAT_R64G64_SFLOAT:
            format = gpufmt::Format::R64G64_SFLOAT;
            break;
        case VK_FORMAT_R64G64B64_UINT:
            format = gpufmt::Format::R64G64B64_UINT;
            break;
        case VK_FORMAT_R64G64B64_SINT:
            format = gpufmt::Format::R64G64B64_SINT;
            break;
        case VK_FORMAT_R64G64B64_SFLOAT:
            format = gpufmt::Format::R64G64B64_SFLOAT;
            break;
        case VK_FORMAT_R64G64B64A64_UINT:
            format = gpufmt::Format::R64G64B64A64_UINT;
            break;
        case VK_FORMAT_R64G64B64A64_SINT:
            format = gpufmt::Format::R64G64B64A64_SINT;
            break;
        case VK_FORMAT_R64G64B64A64_SFLOAT:
            format = gpufmt::Format::R64G64B64A64_SFLOAT;
            break;
#endif // GF_EXCLUDE_64BIT_FORMATS
        case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
            format = gpufmt::Format::B10G11R11_UFLOAT_PACK32;
            break;
        case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:
            format = gpufmt::Format::E5B9G9R9_UFLOAT_PACK32;
            break;
        case VK_FORMAT_D16_UNORM:
            format = gpufmt::Format::D16_UNORM;
            break;
        case VK_FORMAT_X8_D24_UNORM_PACK32:
            format = gpufmt::Format::X8_D24_UNORM_PACK32;
            break;
        case VK_FORMAT_D32_SFLOAT:
            format = gpufmt::Format::D32_SFLOAT;
            break;
        case VK_FORMAT_S8_UINT:
            format = gpufmt::Format::S8_UINT;
            break;
        case VK_FORMAT_D16_UNORM_S8_UINT:
            format = gpufmt::Format::D16_UNORM_S8_UINT;
            break;
        case VK_FORMAT_D24_UNORM_S8_UINT:
            format = gpufmt::Format::D24_UNORM_S8_UINT;
            break;
        case VK_FORMAT_D32_SFLOAT_S8_UINT:
            format = gpufmt::Format::D32_SFLOAT_S8_UINT;
            break;
#ifndef GF_EXCLUDE_COMPRESSED_FORMATS
#ifndef GF_EXCLUDE_BC_COMPRESSED_FORMATS
        case VK_FORMAT_BC1_RGB_UNORM_BLOCK:
            format = gpufmt::Format::BC1_RGB_UNORM_BLOCK;
            break;
        case VK_FORMAT_BC1_RGB_SRGB_BLOCK:
            format = gpufmt::Format::BC1_RGB_SRGB_BLOCK;
            break;
        case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
            format = gpufmt::Format::BC1_RGBA_UNORM_BLOCK;
            break;
        case VK_FORMAT_BC1_RGBA_SRGB_BLOCK:
            format = gpufmt::Format::BC1_RGBA_SRGB_BLOCK;
            break;
        case VK_FORMAT_BC2_UNORM_BLOCK:
            format = gpufmt::Format::BC2_UNORM_BLOCK;
            break;
        case VK_FORMAT_BC2_SRGB_BLOCK:
            format = gpufmt::Format::BC2_SRGB_BLOCK;
            break;
        case VK_FORMAT_BC3_UNORM_BLOCK:
            format = gpufmt::Format::BC3_UNORM_BLOCK;
            break;
        case VK_FORMAT_BC3_SRGB_BLOCK:
            format = gpufmt::Format::BC3_SRGB_BLOCK;
            break;
        case VK_FORMAT_BC4_UNORM_BLOCK:
            format = gpufmt::Format::BC4_UNORM_BLOCK;
            break;
        case VK_FORMAT_BC4_SNORM_BLOCK:
            format = gpufmt::Format::BC4_SNORM_BLOCK;
            break;
        case VK_FORMAT_BC5_UNORM_BLOCK:
            format = gpufmt::Format::BC5_UNORM_BLOCK;
            break;
        case VK_FORMAT_BC5_SNORM_BLOCK:
            format = gpufmt::Format::BC5_SNORM_BLOCK;
            break;
        case VK_FORMAT_BC6H_UFLOAT_BLOCK:
            format = gpufmt::Format::BC6H_UFLOAT_BLOCK;
            break;
        case VK_FORMAT_BC6H_SFLOAT_BLOCK:
            format = gpufmt::Format::BC6H_SFLOAT_BLOCK;
            break;
        case VK_FORMAT_BC7_UNORM_BLOCK:
            format = gpufmt::Format::BC7_UNORM_BLOCK;
            break;
        case VK_FORMAT_BC7_SRGB_BLOCK:
            format = gpufmt::Format::BC7_SRGB_BLOCK;
            break;
#endif //GF_EXCLUDE_BC_COMPRESSED_FORMATS
#ifndef GF_EXCLUDE_ETC_COMPRESSED_FORMATS
        case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK:
            format = gpufmt::Format::ETC2_R8G8B8_UNORM_BLOCK;
            break;
        case VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK:
            format = gpufmt::Format::ETC2_R8G8B8_SRGB_BLOCK;
            break;
        case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK:
            format = gpufmt::Format::ETC2_R8G8B8A1_UNORM_BLOCK;
            break;
        case VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK:
            format = gpufmt::Format::ETC2_R8G8B8A1_SRGB_BLOCK;
            break;
        case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK:
            format = gpufmt::Format::ETC2_R8G8B8A8_UNORM_BLOCK;
            break;
        case VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK:
            format = gpufmt::Format::ETC2_R8G8B8A8_SRGB_BLOCK;
            break;
        case VK_FORMAT_EAC_R11_UNORM_BLOCK:
            format = gpufmt::Format::EAC_R11_UNORM_BLOCK;
            break;
        case VK_FORMAT_EAC_R11_SNORM_BLOCK:
            format = gpufmt::Format::EAC_R11_SNORM_BLOCK;
            break;
        case VK_FORMAT_EAC_R11G11_UNORM_BLOCK:
            format = gpufmt::Format::EAC_R11G11_UNORM_BLOCK;
            break;
        case VK_FORMAT_EAC_R11G11_SNORM_BLOCK:
            format = gpufmt::Format::EAC_R11G11_SNORM_BLOCK;
            break;
#endif //GF_EXCLUDE_ETC_COMPRESSED_FORMATS
#ifndef GF_EXCLUDE_ASTC_COMPRESSED_FORMATS
        case VK_FORMAT_ASTC_4x4_UNORM_BLOCK:
            format = gpufmt::Format::ASTC_4x4_UNORM_BLOCK;
            break;
        case VK_FORMAT_ASTC_4x4_SRGB_BLOCK:
            format = gpufmt::Format::ASTC_4x4_SRGB_BLOCK;
            break;
        case VK_FORMAT_ASTC_5x4_UNORM_BLOCK:
            format = gpufmt::Format::ASTC_5x4_UNORM_BLOCK;
            break;
        case VK_FORMAT_ASTC_5x4_SRGB_BLOCK:
            format = gpufmt::Format::ASTC_5x4_SRGB_BLOCK;
            break;
        case VK_FORMAT_ASTC_5x5_UNORM_BLOCK:
            format = gpufmt::Format::ASTC_5x5_UNORM_BLOCK;
            break;
        case VK_FORMAT_ASTC_5x5_SRGB_BLOCK:
            format = gpufmt::Format::ASTC_5x5_SRGB_BLOCK;
            break;
        case VK_FORMAT_ASTC_6x5_UNORM_BLOCK:
            format = gpufmt::Format::ASTC_6x5_UNORM_BLOCK;
            break;
        case VK_FORMAT_ASTC_6x5_SRGB_BLOCK:
            format = gpufmt::Format::ASTC_6x5_SRGB_BLOCK;
            break;
        case VK_FORMAT_ASTC_6x6_UNORM_BLOCK:
            format = gpufmt::Format::ASTC_6x6_UNORM_BLOCK;
            break;
        case VK_FORMAT_ASTC_6x6_SRGB_BLOCK:
            format = gpufmt::Format::ASTC_6x6_SRGB_BLOCK;
            break;
        case VK_FORMAT_ASTC_8x5_UNORM_BLOCK:
            format = gpufmt::Format::ASTC_8x5_UNORM_BLOCK;
            break;
        case VK_FORMAT_ASTC_8x5_SRGB_BLOCK:
            format = gpufmt::Format::ASTC_8x5_SRGB_BLOCK;
            break;
        case VK_FORMAT_ASTC_8x6_UNORM_BLOCK:
            format = gpufmt::Format::ASTC_8x6_UNORM_BLOCK;
            break;
        case VK_FORMAT_ASTC_8x6_SRGB_BLOCK:
            format = gpufmt::Format::ASTC_8x6_SRGB_BLOCK;
            break;
        case VK_FORMAT_ASTC_8x8_UNORM_BLOCK:
            format = gpufmt::Format::ASTC_8x8_UNORM_BLOCK;
            break;
        case VK_FORMAT_ASTC_8x8_SRGB_BLOCK:
            format = gpufmt::Format::ASTC_8x8_SRGB_BLOCK;
            break;
        case VK_FORMAT_ASTC_10x5_UNORM_BLOCK:
            format = gpufmt::Format::ASTC_10x5_UNORM_BLOCK;
            break;
        case VK_FORMAT_ASTC_10x5_SRGB_BLOCK:
            format = gpufmt::Format::ASTC_10x5_SRGB_BLOCK;
            break;
        case VK_FORMAT_ASTC_10x6_UNORM_BLOCK:
            format = gpufmt::Format::ASTC_10x6_UNORM_BLOCK;
            break;
        case VK_FORMAT_ASTC_10x6_SRGB_BLOCK:
            format = gpufmt::Format::ASTC_10x6_SRGB_BLOCK;
            break;
        case VK_FORMAT_ASTC_10x8_UNORM_BLOCK:
            format = gpufmt::Format::ASTC_10x8_UNORM_BLOCK;
            break;
        case VK_FORMAT_ASTC_10x8_SRGB_BLOCK:
            format = gpufmt::Format::ASTC_10x8_SRGB_BLOCK;
            break;
        case VK_FORMAT_ASTC_10x10_UNORM_BLOCK:
            format = gpufmt::Format::ASTC_10x10_UNORM_BLOCK;
            break;
        case VK_FORMAT_ASTC_10x10_SRGB_BLOCK:
            format = gpufmt::Format::ASTC_10x10_SRGB_BLOCK;
            break;
        case VK_FORMAT_ASTC_12x10_UNORM_BLOCK:
            format = gpufmt::Format::ASTC_12x10_UNORM_BLOCK;
            break;
        case VK_FORMAT_ASTC_12x10_SRGB_BLOCK:
            format = gpufmt::Format::ASTC_12x10_SRGB_BLOCK;
            break;
        case VK_FORMAT_ASTC_12x12_UNORM_BLOCK:
            format = gpufmt::Format::ASTC_12x12_UNORM_BLOCK;
            break;
        case VK_FORMAT_ASTC_12x12_SRGB_BLOCK:
            format = gpufmt::Format::ASTC_12x12_SRGB_BLOCK;
            break;
#endif //GF_EXCLUDE_ASTC_COMPRESSED_FORMATS
#endif //GF_EXCLUDE_COMPRESSED_FORMATS
        case VK_FORMAT_G8B8G8R8_422_UNORM:
            // no supported
            break;
        case VK_FORMAT_B8G8R8G8_422_UNORM:
            // no supported
            break;
        case VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM:
            // no supported
            break;
        case VK_FORMAT_G8_B8R8_2PLANE_420_UNORM:
            // no supported
            break;
        case VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM:
            // no supported
            break;
        case VK_FORMAT_G8_B8R8_2PLANE_422_UNORM:
            // no supported
            break;
        case VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM:
            // no supported
            break;
        case VK_FORMAT_R10X6_UNORM_PACK16:
            // no supported
            break;
        case VK_FORMAT_R10X6G10X6_UNORM_2PACK16:
            // no supported
            break;
        case VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16:
            // no supported
            break;
        case VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16:
            // no supported
            break;
        case VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16:
            // no supported
            break;
        case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16:
            // no supported
            break;
        case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16:
            // no supported
            break;
        case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16:
            // no supported
            break;
        case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16:
            // no supported
            break;
        case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16:
            // no supported
            break;
        case VK_FORMAT_R12X4_UNORM_PACK16:
            // no supported
            break;
        case VK_FORMAT_R12X4G12X4_UNORM_2PACK16:
            // no supported
            break;
        case VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16:
            // no supported
            break;
        case VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16:
            // no supported
            break;
        case VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16:
            // no supported
            break;
        case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16:
            // no supported
            break;
        case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16:
            // no supported
            break;
        case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16:
            // no supported
            break;
        case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16:
            // no supported
            break;
        case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16:
            // no supported
            break;
        case VK_FORMAT_G16B16G16R16_422_UNORM:
            // no supported
            break;
        case VK_FORMAT_B16G16R16G16_422_UNORM:
            // no supported
            break;
        case VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM:
            // no supported
            break;
        case VK_FORMAT_G16_B16R16_2PLANE_420_UNORM:
            // no supported
            break;
        case VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM:
            // no supported
            break;
        case VK_FORMAT_G16_B16R16_2PLANE_422_UNORM:
            // no supported
            break;
        case VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM:
            // no supported
            break;
#if !defined(GF_EXCLUDE_COMPRESSED_FORMATS) && !defined(GF_EXCLUDE_PVRTC_COMPRESSED_FORMATS)
        case VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG:
            format = gpufmt::Format::PVRTC1_2BPP_UNORM_BLOCK_IMG;
            break;
        case VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG:
            format = gpufmt::Format::PVRTC1_4BPP_UNORM_BLOCK_IMG;
            break;
        case VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG:
            format = gpufmt::Format::PVRTC2_2BPP_UNORM_BLOCK_IMG;
            break;
        case VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG:
            format = gpufmt::Format::PVRTC2_4BPP_UNORM_BLOCK_IMG;
            break;
        case VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG:
            format = gpufmt::Format::PVRTC1_2BPP_SRGB_BLOCK_IMG;
            break;
        case VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG:
            format = gpufmt::Format::PVRTC1_4BPP_SRGB_BLOCK_IMG;
            break;
        case VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG:
            format = gpufmt::Format::PVRTC2_2BPP_SRGB_BLOCK_IMG;
            break;
        case VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG:
            format = gpufmt::Format::PVRTC2_4BPP_SRGB_BLOCK_IMG;
            break;
#endif //!defined(GF_EXCLUDE_COMPRESSED_FORMATS) && !defined(GF_EXCLUDE_PVRTC_COMPRESSED_FORMATS)
        case VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK_EXT:
            // no supported
            break;
        case VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK_EXT:
            // no supported
            break;
        case VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK_EXT:
            // no supported
            break;
        case VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK_EXT:
            // no supported
            break;
        case VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK_EXT:
            // no supported
            break;
        case VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK_EXT:
            // no supported
            break;
        case VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK_EXT:
            // no supported
            break;
        case VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK_EXT:
            // no supported
            break;
        case VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK_EXT:
            // no supported
            break;
        case VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK_EXT:
            // no supported
            break;
        case VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK_EXT:
            // no supported
            break;
        case VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK_EXT:
            // no supported
            break;
        case VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK_EXT:
            // no supported
            break;
        case VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK_EXT:
            // no supported
            break;
        default:
            break;
        }

        return format;
    }
}

static constexpr std::string_view to_string(VkFormat format) noexcept {
    switch(format)
    {
    case VK_FORMAT_UNDEFINED:
        return "VK_FORMAT_UNDEFINED";
    case VK_FORMAT_R4G4_UNORM_PACK8:
        return "VK_FORMAT_R4G4_UNORM_PACK8";
    case VK_FORMAT_R4G4B4A4_UNORM_PACK16:
        return "VK_FORMAT_R4G4B4A4_UNORM_PACK16";
    case VK_FORMAT_B4G4R4A4_UNORM_PACK16:
        return "VK_FORMAT_B4G4R4A4_UNORM_PACK16";
    case VK_FORMAT_R5G6B5_UNORM_PACK16:
        return "VK_FORMAT_R5G6B5_UNORM_PACK16";
    case VK_FORMAT_B5G6R5_UNORM_PACK16:
        return "VK_FORMAT_B5G6R5_UNORM_PACK16";
    case VK_FORMAT_R5G5B5A1_UNORM_PACK16:
        return "VK_FORMAT_R5G5B5A1_UNORM_PACK16";
    case VK_FORMAT_B5G5R5A1_UNORM_PACK16:
        return "VK_FORMAT_B5G5R5A1_UNORM_PACK16";
    case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
        return "VK_FORMAT_A1R5G5B5_UNORM_PACK16";
    case VK_FORMAT_R8_UNORM:
        return "VK_FORMAT_R8_UNORM";
    case VK_FORMAT_R8_SNORM:
        return "VK_FORMAT_R8_SNORM";
#ifndef GF_EXCLUDE_SCALED_FORMATS
    case VK_FORMAT_R8_USCALED:
        return "VK_FORMAT_R8_USCALED";
    case VK_FORMAT_R8_SSCALED:
        return "VK_FORMAT_R8_SSCALED";
#endif
    case VK_FORMAT_R8_UINT:
        return "VK_FORMAT_R8_UINT";
    case VK_FORMAT_R8_SINT:
        return "VK_FORMAT_R8_SINT";
    case VK_FORMAT_R8_SRGB:
        return "VK_FORMAT_R8_SRGB";
    case VK_FORMAT_R8G8_UNORM:
        return "VK_FORMAT_R8G8_UNORM";
    case VK_FORMAT_R8G8_SNORM:
        return "VK_FORMAT_R8G8_SNORM";
#ifndef GF_EXCLUDE_SCALED_FORMATS
    case VK_FORMAT_R8G8_USCALED:
        return "VK_FORMAT_R8G8_USCALED";
    case VK_FORMAT_R8G8_SSCALED:
        return "VK_FORMAT_R8G8_SSCALED";
#endif
    case VK_FORMAT_R8G8_UINT:
        return "VK_FORMAT_R8G8_UINT";
    case VK_FORMAT_R8G8_SINT:
        return "VK_FORMAT_R8G8_SINT";
    case VK_FORMAT_R8G8_SRGB:
        return "VK_FORMAT_R8G8_SRGB";
    case VK_FORMAT_R8G8B8_UNORM:
        return "VK_FORMAT_R8G8B8_UNORM";
    case VK_FORMAT_R8G8B8_SNORM:
        return "VK_FORMAT_R8G8B8_SNORM";
#ifndef GF_EXCLUDE_SCALED_FORMATS
    case VK_FORMAT_R8G8B8_USCALED:
        return "VK_FORMAT_R8G8B8_USCALED";
    case VK_FORMAT_R8G8B8_SSCALED:
        return "VK_FORMAT_R8G8B8_SSCALED";
#endif
    case VK_FORMAT_R8G8B8_UINT:
        return "VK_FORMAT_R8G8B8_UINT";
    case VK_FORMAT_R8G8B8_SINT:
        return "VK_FORMAT_R8G8B8_SINT";
    case VK_FORMAT_R8G8B8_SRGB:
        return "VK_FORMAT_R8G8B8_SRGB";
    case VK_FORMAT_B8G8R8_UNORM:
        return "VK_FORMAT_B8G8R8_UNORM";
    case VK_FORMAT_B8G8R8_SNORM:
        return "VK_FORMAT_B8G8R8_SNORM";
#ifndef GF_EXCLUDE_SCALED_FORMATS
    case VK_FORMAT_B8G8R8_USCALED:
        return "VK_FORMAT_B8G8R8_USCALED";
    case VK_FORMAT_B8G8R8_SSCALED:
        return "VK_FORMAT_B8G8R8_SSCALED";
#endif
    case VK_FORMAT_B8G8R8_UINT:
        return "VK_FORMAT_B8G8R8_UINT";
    case VK_FORMAT_B8G8R8_SINT:
        return "VK_FORMAT_B8G8R8_SINT";
    case VK_FORMAT_B8G8R8_SRGB:
        return "VK_FORMAT_B8G8R8_SRGB";
    case VK_FORMAT_R8G8B8A8_UNORM:
        return "VK_FORMAT_R8G8B8A8_UNORM";
    case VK_FORMAT_R8G8B8A8_SNORM:
        return "VK_FORMAT_R8G8B8A8_SNORM";
#ifndef GF_EXCLUDE_SCALED_FORMATS
    case VK_FORMAT_R8G8B8A8_USCALED:
        return "VK_FORMAT_R8G8B8A8_USCALED";
    case VK_FORMAT_R8G8B8A8_SSCALED:
        return "VK_FORMAT_R8G8B8A8_SSCALED";
#endif
    case VK_FORMAT_R8G8B8A8_UINT:
        return "VK_FORMAT_R8G8B8A8_UINT";
    case VK_FORMAT_R8G8B8A8_SINT:
        return "VK_FORMAT_R8G8B8A8_SINT";
    case VK_FORMAT_R8G8B8A8_SRGB:
        return "VK_FORMAT_R8G8B8A8_SRGB";
    case VK_FORMAT_B8G8R8A8_UNORM:
        return "VK_FORMAT_B8G8R8A8_UNORM";
    case VK_FORMAT_B8G8R8A8_SNORM:
        return "VK_FORMAT_B8G8R8A8_SNORM";
#ifndef GF_EXCLUDE_SCALED_FORMATS
    case VK_FORMAT_B8G8R8A8_USCALED:
        return "VK_FORMAT_B8G8R8A8_USCALED";
    case VK_FORMAT_B8G8R8A8_SSCALED:
        return "VK_FORMAT_B8G8R8A8_SSCALED";
#endif
    case VK_FORMAT_B8G8R8A8_UINT:
        return "VK_FORMAT_B8G8R8A8_UINT";
    case VK_FORMAT_B8G8R8A8_SINT:
        return "VK_FORMAT_B8G8R8A8_SINT";
    case VK_FORMAT_B8G8R8A8_SRGB:
        return "VK_FORMAT_B8G8R8A8_SRGB";
    case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
        return "VK_FORMAT_A8B8G8R8_UNORM_PACK32";
    case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
        return "VK_FORMAT_A8B8G8R8_SNORM_PACK32";
#ifndef GF_EXCLUDE_SCALED_FORMATS
    case VK_FORMAT_A8B8G8R8_USCALED_PACK32:
        return "VK_FORMAT_A8B8G8R8_USCALED_PACK32";
    case VK_FORMAT_A8B8G8R8_SSCALED_PACK32:
        return "VK_FORMAT_A8B8G8R8_SSCALED_PACK32";
#endif
    case VK_FORMAT_A8B8G8R8_UINT_PACK32:
        return "VK_FORMAT_A8B8G8R8_UINT_PACK32";
    case VK_FORMAT_A8B8G8R8_SINT_PACK32:
        return "VK_FORMAT_A8B8G8R8_SINT_PACK32";
    case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
        return "VK_FORMAT_A8B8G8R8_SRGB_PACK32";
    case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
        return "VK_FORMAT_A2R10G10B10_UNORM_PACK32";
    case VK_FORMAT_A2R10G10B10_SNORM_PACK32:
        return "VK_FORMAT_A2R10G10B10_SNORM_PACK32";
#ifndef GF_EXCLUDE_SCALED_FORMATS
    case VK_FORMAT_A2R10G10B10_USCALED_PACK32:
        return "VK_FORMAT_A2R10G10B10_USCALED_PACK32";
    case VK_FORMAT_A2R10G10B10_SSCALED_PACK32:
        return "VK_FORMAT_A2R10G10B10_SSCALED_PACK32";
#endif
    case VK_FORMAT_A2R10G10B10_UINT_PACK32:
        return "VK_FORMAT_A2R10G10B10_UINT_PACK32";
    case VK_FORMAT_A2R10G10B10_SINT_PACK32:
        return "VK_FORMAT_A2R10G10B10_SINT_PACK32";
    case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
        return "VK_FORMAT_A2B10G10R10_UNORM_PACK32";
    case VK_FORMAT_A2B10G10R10_SNORM_PACK32:
        return "VK_FORMAT_A2B10G10R10_SNORM_PACK32";
#ifndef GF_EXCLUDE_SCALED_FORMATS
    case VK_FORMAT_A2B10G10R10_USCALED_PACK32:
        return "VK_FORMAT_A2B10G10R10_USCALED_PACK32";
    case VK_FORMAT_A2B10G10R10_SSCALED_PACK32:
        return "VK_FORMAT_A2B10G10R10_SSCALED_PACK32";
#endif
    case VK_FORMAT_A2B10G10R10_UINT_PACK32:
        return "VK_FORMAT_A2B10G10R10_UINT_PACK32";
    case VK_FORMAT_A2B10G10R10_SINT_PACK32:
        return "VK_FORMAT_A2B10G10R10_SINT_PACK32";
    case VK_FORMAT_R16_UNORM:
        return "VK_FORMAT_R16_UNORM";
    case VK_FORMAT_R16_SNORM:
        return "VK_FORMAT_R16_SNORM";
#ifndef GF_EXCLUDE_SCALED_FORMATS
    case VK_FORMAT_R16_USCALED:
        return "VK_FORMAT_R16_USCALED";
    case VK_FORMAT_R16_SSCALED:
        return "VK_FORMAT_R16_SSCALED";
#endif
    case VK_FORMAT_R16_UINT:
        return "VK_FORMAT_R16_UINT";
    case VK_FORMAT_R16_SINT:
        return "VK_FORMAT_R16_SINT";
    case VK_FORMAT_R16_SFLOAT:
        return "VK_FORMAT_R16_SFLOAT";
    case VK_FORMAT_R16G16_UNORM:
        return "VK_FORMAT_R16G16_UNORM";
    case VK_FORMAT_R16G16_SNORM:
        return "VK_FORMAT_R16G16_SNORM";
#ifndef GF_EXCLUDE_SCALED_FORMATS
    case VK_FORMAT_R16G16_USCALED:
        return "VK_FORMAT_R16G16_USCALED";
    case VK_FORMAT_R16G16_SSCALED:
        return "VK_FORMAT_R16G16_SSCALED";
#endif
    case VK_FORMAT_R16G16_UINT:
        return "VK_FORMAT_R16G16_UINT";
    case VK_FORMAT_R16G16_SINT:
        return "VK_FORMAT_R16G16_SINT";
    case VK_FORMAT_R16G16_SFLOAT:
        return "VK_FORMAT_R16G16_SFLOAT";
    case VK_FORMAT_R16G16B16_UNORM:
        return "VK_FORMAT_R16G16B16_UNORM";
    case VK_FORMAT_R16G16B16_SNORM:
        return "VK_FORMAT_R16G16B16_SNORM";
#ifndef GF_EXCLUDE_SCALED_FORMATS
    case VK_FORMAT_R16G16B16_USCALED:
        return "VK_FORMAT_R16G16B16_USCALED";
    case VK_FORMAT_R16G16B16_SSCALED:
        return "VK_FORMAT_R16G16B16_SSCALED";
#endif
    case VK_FORMAT_R16G16B16_UINT:
        return "VK_FORMAT_R16G16B16_UINT";
    case VK_FORMAT_R16G16B16_SINT:
        return "VK_FORMAT_R16G16B16_SINT";
    case VK_FORMAT_R16G16B16_SFLOAT:
        return "VK_FORMAT_R16G16B16_SFLOAT";
    case VK_FORMAT_R16G16B16A16_UNORM:
        return "VK_FORMAT_R16G16B16A16_UNORM";
    case VK_FORMAT_R16G16B16A16_SNORM:
        return "VK_FORMAT_R16G16B16A16_SNORM";
#ifndef GF_EXCLUDE_SCALED_FORMATS
    case VK_FORMAT_R16G16B16A16_USCALED:
        return "VK_FORMAT_R16G16B16A16_USCALED";
    case VK_FORMAT_R16G16B16A16_SSCALED:
        return "VK_FORMAT_R16G16B16A16_SSCALED";
#endif
    case VK_FORMAT_R16G16B16A16_UINT:
        return "VK_FORMAT_R16G16B16A16_UINT";
    case VK_FORMAT_R16G16B16A16_SINT:
        return "VK_FORMAT_R16G16B16A16_SINT";
    case VK_FORMAT_R16G16B16A16_SFLOAT:
        return "VK_FORMAT_R16G16B16A16_SFLOAT";
    case VK_FORMAT_R32_UINT:
        return "VK_FORMAT_R32_UINT";
    case VK_FORMAT_R32_SINT:
        return "VK_FORMAT_R32_SINT";
    case VK_FORMAT_R32_SFLOAT:
        return "VK_FORMAT_R32_SFLOAT";
    case VK_FORMAT_R32G32_UINT:
        return "VK_FORMAT_R32G32_UINT";
    case VK_FORMAT_R32G32_SINT:
        return "VK_FORMAT_R32G32_SINT";
    case VK_FORMAT_R32G32_SFLOAT:
        return "VK_FORMAT_R32G32_SFLOAT";
    case VK_FORMAT_R32G32B32_UINT:
        return "VK_FORMAT_R32G32B32_UINT";
    case VK_FORMAT_R32G32B32_SINT:
        return "VK_FORMAT_R32G32B32_SINT";
    case VK_FORMAT_R32G32B32_SFLOAT:
        return "VK_FORMAT_R32G32B32_SFLOAT";
    case VK_FORMAT_R32G32B32A32_UINT:
        return "VK_FORMAT_R32G32B32A32_UINT";
    case VK_FORMAT_R32G32B32A32_SINT:
        return "VK_FORMAT_R32G32B32A32_SINT";
    case VK_FORMAT_R32G32B32A32_SFLOAT:
        return "VK_FORMAT_R32G32B32A32_SFLOAT";
#ifndef GF_EXCLUDE_64BIT_FORMATS
    case VK_FORMAT_R64_UINT:
        return "VK_FORMAT_R64_UINT";
    case VK_FORMAT_R64_SINT:
        return "VK_FORMAT_R64_SINT";
    case VK_FORMAT_R64_SFLOAT:
        return "VK_FORMAT_R64_SFLOAT";
    case VK_FORMAT_R64G64_UINT:
        return "VK_FORMAT_R64G64_UINT";
    case VK_FORMAT_R64G64_SINT:
        return "VK_FORMAT_R64G64_SINT";
    case VK_FORMAT_R64G64_SFLOAT:
        return "VK_FORMAT_R64G64_SFLOAT";
    case VK_FORMAT_R64G64B64_UINT:
        return "VK_FORMAT_R64G64B64_UINT";
    case VK_FORMAT_R64G64B64_SINT:
        return "VK_FORMAT_R64G64B64_SINT";
    case VK_FORMAT_R64G64B64_SFLOAT:
        return "VK_FORMAT_R64G64B64_SFLOAT";
    case VK_FORMAT_R64G64B64A64_UINT:
        return "VK_FORMAT_R64G64B64A64_UINT";
    case VK_FORMAT_R64G64B64A64_SINT:
        return "VK_FORMAT_R64G64B64A64_SINT";
    case VK_FORMAT_R64G64B64A64_SFLOAT:
        return "VK_FORMAT_R64G64B64A64_SFLOAT";
#endif // GF_EXCLUDE_64BIT_FORMATS
    case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
        return "VK_FORMAT_B10G11R11_UFLOAT_PACK32";
    case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:
        return "VK_FORMAT_E5B9G9R9_UFLOAT_PACK32";
    case VK_FORMAT_D16_UNORM:
        return "VK_FORMAT_D16_UNORM";
    case VK_FORMAT_X8_D24_UNORM_PACK32:
        return "VK_FORMAT_X8_D24_UNORM_PACK32";
    case VK_FORMAT_D32_SFLOAT:
        return "VK_FORMAT_D32_SFLOAT";
    case VK_FORMAT_S8_UINT:
        return "VK_FORMAT_S8_UINT";
    case VK_FORMAT_D16_UNORM_S8_UINT:
        return "VK_FORMAT_D16_UNORM_S8_UINT";
    case VK_FORMAT_D24_UNORM_S8_UINT:
        return "VK_FORMAT_D24_UNORM_S8_UINT";
    case VK_FORMAT_D32_SFLOAT_S8_UINT:
        return "VK_FORMAT_D32_SFLOAT_S8_UINT";
    case VK_FORMAT_BC1_RGB_UNORM_BLOCK:
        return "VK_FORMAT_BC1_RGB_UNORM_BLOCK";
    case VK_FORMAT_BC1_RGB_SRGB_BLOCK:
        return "VK_FORMAT_BC1_RGB_SRGB_BLOCK";
    case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
        return "VK_FORMAT_BC1_RGBA_UNORM_BLOCK";
    case VK_FORMAT_BC1_RGBA_SRGB_BLOCK:
        return "VK_FORMAT_BC1_RGBA_SRGB_BLOCK";
    case VK_FORMAT_BC2_UNORM_BLOCK:
        return "VK_FORMAT_BC2_UNORM_BLOCK";
    case VK_FORMAT_BC2_SRGB_BLOCK:
        return "VK_FORMAT_BC2_SRGB_BLOCK";
    case VK_FORMAT_BC3_UNORM_BLOCK:
        return "VK_FORMAT_BC3_UNORM_BLOCK";
    case VK_FORMAT_BC3_SRGB_BLOCK:
        return "VK_FORMAT_BC3_SRGB_BLOCK";
    case VK_FORMAT_BC4_UNORM_BLOCK:
        return "VK_FORMAT_BC4_UNORM_BLOCK";
    case VK_FORMAT_BC4_SNORM_BLOCK:
        return "VK_FORMAT_BC4_SNORM_BLOCK";
    case VK_FORMAT_BC5_UNORM_BLOCK:
        return "VK_FORMAT_BC5_UNORM_BLOCK";
    case VK_FORMAT_BC5_SNORM_BLOCK:
        return "VK_FORMAT_BC5_SNORM_BLOCK";
    case VK_FORMAT_BC6H_UFLOAT_BLOCK:
        return "VK_FORMAT_BC6H_UFLOAT_BLOCK";
    case VK_FORMAT_BC6H_SFLOAT_BLOCK:
        return "VK_FORMAT_BC6H_SFLOAT_BLOCK";
    case VK_FORMAT_BC7_UNORM_BLOCK:
        return "VK_FORMAT_BC7_UNORM_BLOCK";
    case VK_FORMAT_BC7_SRGB_BLOCK:
        return "VK_FORMAT_BC7_SRGB_BLOCK";
    case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK:
        return "VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK";
    case VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK:
        return "VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK";
    case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK:
        return "VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK";
    case VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK:
        return "VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK";
    case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK:
        return "VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK";
    case VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK:
        return "VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK";
    case VK_FORMAT_EAC_R11_UNORM_BLOCK:
        return "VK_FORMAT_EAC_R11_UNORM_BLOCK";
    case VK_FORMAT_EAC_R11_SNORM_BLOCK:
        return "VK_FORMAT_EAC_R11_SNORM_BLOCK";
    case VK_FORMAT_EAC_R11G11_UNORM_BLOCK:
        return "VK_FORMAT_EAC_R11G11_UNORM_BLOCK";
    case VK_FORMAT_EAC_R11G11_SNORM_BLOCK:
        return "VK_FORMAT_EAC_R11G11_SNORM_BLOCK";
    case VK_FORMAT_ASTC_4x4_UNORM_BLOCK:
        return "VK_FORMAT_ASTC_4x4_UNORM_BLOCK";
    case VK_FORMAT_ASTC_4x4_SRGB_BLOCK:
        return "VK_FORMAT_ASTC_4x4_SRGB_BLOCK";
    case VK_FORMAT_ASTC_5x4_UNORM_BLOCK:
        return "VK_FORMAT_ASTC_5x4_UNORM_BLOCK";
    case VK_FORMAT_ASTC_5x4_SRGB_BLOCK:
        return "VK_FORMAT_ASTC_5x4_SRGB_BLOCK";
    case VK_FORMAT_ASTC_5x5_UNORM_BLOCK:
        return "VK_FORMAT_ASTC_5x5_UNORM_BLOCK";
    case VK_FORMAT_ASTC_5x5_SRGB_BLOCK:
        return "VK_FORMAT_ASTC_5x5_SRGB_BLOCK";
    case VK_FORMAT_ASTC_6x5_UNORM_BLOCK:
        return "VK_FORMAT_ASTC_6x5_UNORM_BLOCK";
    case VK_FORMAT_ASTC_6x5_SRGB_BLOCK:
        return "VK_FORMAT_ASTC_6x5_SRGB_BLOCK";
    case VK_FORMAT_ASTC_6x6_UNORM_BLOCK:
        return "VK_FORMAT_ASTC_6x6_UNORM_BLOCK";
    case VK_FORMAT_ASTC_6x6_SRGB_BLOCK:
        return "VK_FORMAT_ASTC_6x6_SRGB_BLOCK";
    case VK_FORMAT_ASTC_8x5_UNORM_BLOCK:
        return "VK_FORMAT_ASTC_8x5_UNORM_BLOCK";
    case VK_FORMAT_ASTC_8x5_SRGB_BLOCK:
        return "VK_FORMAT_ASTC_8x5_SRGB_BLOCK";
    case VK_FORMAT_ASTC_8x6_UNORM_BLOCK:
        return "VK_FORMAT_ASTC_8x6_UNORM_BLOCK";
    case VK_FORMAT_ASTC_8x6_SRGB_BLOCK:
        return "VK_FORMAT_ASTC_8x6_SRGB_BLOCK";
    case VK_FORMAT_ASTC_8x8_UNORM_BLOCK:
        return "VK_FORMAT_ASTC_8x8_UNORM_BLOCK";
    case VK_FORMAT_ASTC_8x8_SRGB_BLOCK:
        return "VK_FORMAT_ASTC_8x8_SRGB_BLOCK";
    case VK_FORMAT_ASTC_10x5_UNORM_BLOCK:
        return "VK_FORMAT_ASTC_10x5_UNORM_BLOCK";
    case VK_FORMAT_ASTC_10x5_SRGB_BLOCK:
        return "VK_FORMAT_ASTC_10x5_SRGB_BLOCK";
    case VK_FORMAT_ASTC_10x6_UNORM_BLOCK:
        return "VK_FORMAT_ASTC_10x6_UNORM_BLOCK";
    case VK_FORMAT_ASTC_10x6_SRGB_BLOCK:
        return "VK_FORMAT_ASTC_10x6_SRGB_BLOCK";
    case VK_FORMAT_ASTC_10x8_UNORM_BLOCK:
        return "VK_FORMAT_ASTC_10x8_UNORM_BLOCK";
    case VK_FORMAT_ASTC_10x8_SRGB_BLOCK:
        return "VK_FORMAT_ASTC_10x8_SRGB_BLOCK";
    case VK_FORMAT_ASTC_10x10_UNORM_BLOCK:
        return "VK_FORMAT_ASTC_10x10_UNORM_BLOCK";
    case VK_FORMAT_ASTC_10x10_SRGB_BLOCK:
        return "VK_FORMAT_ASTC_10x10_SRGB_BLOCK";
    case VK_FORMAT_ASTC_12x10_UNORM_BLOCK:
        return "VK_FORMAT_ASTC_12x10_UNORM_BLOCK";
    case VK_FORMAT_ASTC_12x10_SRGB_BLOCK:
        return "VK_FORMAT_ASTC_12x10_SRGB_BLOCK";
    case VK_FORMAT_ASTC_12x12_UNORM_BLOCK:
        return "VK_FORMAT_ASTC_12x12_UNORM_BLOCK";
    case VK_FORMAT_ASTC_12x12_SRGB_BLOCK:
        return "VK_FORMAT_ASTC_12x12_SRGB_BLOCK";
    case VK_FORMAT_G8B8G8R8_422_UNORM:
        return "VK_FORMAT_G8B8G8R8_422_UNORM";
    case VK_FORMAT_B8G8R8G8_422_UNORM:
        return "VK_FORMAT_B8G8R8G8_422_UNORM";
    case VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM:
        return "VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM";
    case VK_FORMAT_G8_B8R8_2PLANE_420_UNORM:
        return "VK_FORMAT_G8_B8R8_2PLANE_420_UNORM";
    case VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM:
        return "VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM";
    case VK_FORMAT_G8_B8R8_2PLANE_422_UNORM:
        return "VK_FORMAT_G8_B8R8_2PLANE_422_UNORM";
    case VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM:
        return "VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM";
    case VK_FORMAT_R10X6_UNORM_PACK16:
        return "VK_FORMAT_R10X6_UNORM_PACK16";
    case VK_FORMAT_R10X6G10X6_UNORM_2PACK16:
        return "VK_FORMAT_R10X6G10X6_UNORM_2PACK16";
    case VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16:
        return "VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16";
    case VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16:
        return "VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16";
    case VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16:
        return "VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16";
    case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16:
        return "VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16";
    case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16:
        return "VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16";
    case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16:
        return "VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16";
    case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16:
        return "VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16";
    case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16:
        return "VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16";
    case VK_FORMAT_R12X4_UNORM_PACK16:
        return "VK_FORMAT_R12X4_UNORM_PACK16";
    case VK_FORMAT_R12X4G12X4_UNORM_2PACK16:
        return "VK_FORMAT_R12X4G12X4_UNORM_2PACK16";
    case VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16:
        return "VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16";
    case VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16:
        return "VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16";
    case VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16:
        return "VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16";
    case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16:
        return "VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16";
    case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16:
        return "VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16";
    case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16:
        return "VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16";
    case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16:
        return "VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16";
    case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16:
        return "VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16";
    case VK_FORMAT_G16B16G16R16_422_UNORM:
        return "VK_FORMAT_G16B16G16R16_422_UNORM";
    case VK_FORMAT_B16G16R16G16_422_UNORM:
        return "VK_FORMAT_B16G16R16G16_422_UNORM";
    case VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM:
        return "VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM";
    case VK_FORMAT_G16_B16R16_2PLANE_420_UNORM:
        return "VK_FORMAT_G16_B16R16_2PLANE_420_UNORM";
    case VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM:
        return "VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM";
    case VK_FORMAT_G16_B16R16_2PLANE_422_UNORM:
        return "VK_FORMAT_G16_B16R16_2PLANE_422_UNORM";
    case VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM:
        return "VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM";
    case VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG:
        return "VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG";
    case VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG:
        return "VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG";
    case VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG:
        return "VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG";
    case VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG:
        return "VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG";
    case VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG:
        return "VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG";
    case VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG:
        return "VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG";
    case VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG:
        return "VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG";
    case VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG:
        return "VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG";
    case VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK_EXT:
        return "VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK_EXT";
    case VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK_EXT:
        return "VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK_EXT";
    case VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK_EXT:
        return "VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK_EXT";
    case VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK_EXT:
        return "VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK_EXT";
    case VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK_EXT:
        return "VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK_EXT";
    case VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK_EXT:
        return "VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK_EXT";
    case VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK_EXT:
        return "VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK_EXT";
    case VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK_EXT:
        return "VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK_EXT";
    case VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK_EXT:
        return "VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK_EXT";
    case VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK_EXT:
        return "VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK_EXT";
    case VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK_EXT:
        return "VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK_EXT";
    case VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK_EXT:
        return "VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK_EXT";
    case VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK_EXT:
        return "VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK_EXT";
    case VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK_EXT:
        return "VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK_EXT";
    default:
        return "VK_FORMAT_UNDEFINED";
    }
}

inline std::ostream &operator<<(std::ostream &out, VkFormat format) {
    out << to_string(format);
    return out;
}