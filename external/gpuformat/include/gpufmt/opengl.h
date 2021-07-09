#pragma once

#include <gpufmt/format.h>
#include <gpufmt/utility.h>

#include <array>
#include <optional>
#include <string_view>
#include <variant>

#ifndef GL_OES_compressed_ETC1_RGB8_texture
#define GL_OES_compressed_ETC1_RGB8_texture 1
#define GL_ETC1_RGB8_OES                  0x8D64
#endif /* GL_OES_compressed_ETC1_RGB8_texture */

#ifndef GL_IMG_texture_compression_pvrtc
#define GL_IMG_texture_compression_pvrtc 1
#define GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG 0x8C00
#define GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG 0x8C01
#define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG 0x8C02
#define GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG 0x8C03
#endif /* GL_IMG_texture_compression_pvrtc */

#ifndef GL_IMG_texture_compression_pvrtc2
#define GL_IMG_texture_compression_pvrtc2 1
#define GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG 0x9137
#define GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG 0x9138
#endif /* GL_IMG_texture_compression_pvrtc2 */

#ifndef GL_EXT_pvrtc_sRGB
#define GL_EXT_pvrtc_sRGB 1
#define GL_COMPRESSED_SRGB_PVRTC_2BPPV1_EXT 0x8A54
#define GL_COMPRESSED_SRGB_PVRTC_4BPPV1_EXT 0x8A55
#define GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV1_EXT 0x8A56
#define GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV1_EXT 0x8A57
#define GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV2_IMG 0x93F0
#define GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV2_IMG 0x93F1
#endif /* GL_EXT_pvrtc_sRGB */

namespace gpufmt::gl {
    struct Format {
        GLenum internalFormat;
        GLenum format = GL_NONE;
        GLenum type = GL_NONE;
        std::array<GLenum, 4> swizzleMask = { GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA };
    };

    struct FormatConversion {
        std::optional<Format> exact;
        std::optional<Format> closest;

        [[nodiscard]]
        constexpr operator bool() const noexcept {
            return exact || closest;
        }
    };

    [[nodiscard]]
    static constexpr FormatConversion translateFormat(gpufmt::Format format) noexcept {
        FormatConversion conversion;

        switch(format) {
        case gpufmt::Format::UNDEFINED:
            // undefined
            break;
        case gpufmt::Format::R4G4_UNORM_PACK8:
            conversion.closest = gpufmt::gl::Format{ GL_RGB4, GL_RGB, GL_UNSIGNED_SHORT, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA } };
            break;
        case gpufmt::Format::R4G4B4A4_UNORM_PACK16:
            conversion.exact = gpufmt::gl::Format{ GL_RGBA4, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA } };
            break;
        case gpufmt::Format::B4G4R4A4_UNORM_PACK16:
            conversion.exact = gpufmt::gl::Format{ GL_RGBA4, GL_BGRA, GL_UNSIGNED_SHORT_4_4_4_4, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA } };
            break;
        case gpufmt::Format::A4R4G4B4_UNORM_PACK16:
            conversion.exact = gpufmt::gl::Format{ GL_RGBA4, GL_BGRA, GL_UNSIGNED_SHORT_4_4_4_4_REV, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R5G6B5_UNORM_PACK16:
            conversion.exact = gpufmt::gl::Format{ GL_RGB565, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA } };
            break;
        case gpufmt::Format::B5G6R5_UNORM_PACK16:
            conversion.exact = gpufmt::gl::Format{ GL_RGB565, GL_BGR, GL_UNSIGNED_SHORT_5_6_5, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA } };
            break;
        case gpufmt::Format::R5G5B5A1_UNORM_PACK16:
            conversion.exact = gpufmt::gl::Format{ GL_RGB5_A1, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA } };
            break;
        case gpufmt::Format::B5G5R5A1_UNORM_PACK16:
            conversion.exact = gpufmt::gl::Format{ GL_RGB5_A1, GL_BGRA, GL_UNSIGNED_SHORT_5_5_5_1, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA } };
            break;
        case gpufmt::Format::A1R5G5B5_UNORM_PACK16:
            conversion.exact = gpufmt::gl::Format{ GL_RGB5_A1, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA } };
            break;
        case gpufmt::Format::R8_UNORM:
            conversion.exact = gpufmt::gl::Format{ GL_R8, GL_RED, GL_UNSIGNED_BYTE, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA } };
            break;
        case gpufmt::Format::R8_SNORM:
            conversion.exact = gpufmt::gl::Format{ GL_R8_SNORM, GL_RED, GL_BYTE, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA } };
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::R8_USCALED:
            conversion.closest = gpufmt::gl::Format{ GL_R8UI, GL_RED, GL_UNSIGNED_BYTE, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R8_SSCALED:
            conversion.closest = gpufmt::gl::Format{ GL_R8I, GL_RED, GL_BYTE, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
#endif
        case gpufmt::Format::R8_UINT:
            conversion.exact = gpufmt::gl::Format{ GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R8_SINT:
            conversion.exact = gpufmt::gl::Format{ GL_R8I, GL_RED_INTEGER, GL_BYTE, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R8_SRGB:
            conversion.exact = gpufmt::gl::Format{ GL_SR8_EXT, GL_RED, GL_UNSIGNED_BYTE, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::A8_UNORM:
            conversion.exact = gpufmt::gl::Format{ GL_R8, GL_RED, GL_UNSIGNED_BYTE, {GL_ZERO, GL_ZERO, GL_ZERO, GL_RED} };
            break;
        case gpufmt::Format::R8G8_UNORM:
            conversion.exact = gpufmt::gl::Format{ GL_RG8, GL_RG, GL_UNSIGNED_BYTE, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R8G8_SNORM:
            conversion.exact = gpufmt::gl::Format{ GL_RG8_SNORM, GL_RG, GL_BYTE, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::R8G8_USCALED:
            conversion.closest = gpufmt::gl::Format{ GL_RG8UI, GL_RG, GL_UNSIGNED_BYTE, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R8G8_SSCALED:
            conversion.closest = gpufmt::gl::Format{ GL_RG8I, GL_RG, GL_BYTE, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
#endif
        case gpufmt::Format::R8G8_UINT:
            conversion.exact = gpufmt::gl::Format{ GL_RG8UI, GL_RG_INTEGER, GL_UNSIGNED_BYTE, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R8G8_SINT:
            conversion.exact = gpufmt::gl::Format{ GL_RG8I, GL_RG_INTEGER, GL_BYTE, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R8G8_SRGB:
            conversion.closest = gpufmt::gl::Format{ GL_RG8, GL_RG, GL_UNSIGNED_BYTE, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R8G8B8_UNORM:
            conversion.exact = gpufmt::gl::Format{ GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R8G8B8_SNORM:
            conversion.exact = gpufmt::gl::Format{ GL_RGB8_SNORM, GL_RGB, GL_BYTE, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::R8G8B8_USCALED:
            conversion.closest = gpufmt::gl::Format{ GL_RGB8UI, GL_RGB, GL_UNSIGNED_BYTE, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R8G8B8_SSCALED:
            conversion.closest = gpufmt::gl::Format{ GL_RGB8I, GL_RGB, GL_BYTE, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
#endif
        case gpufmt::Format::R8G8B8_UINT:
            conversion.exact = gpufmt::gl::Format{ GL_RGB8UI, GL_RGB_INTEGER, GL_UNSIGNED_BYTE, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R8G8B8_SINT:
            conversion.exact = gpufmt::gl::Format{ GL_RGB8I, GL_RGB_INTEGER, GL_BYTE, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R8G8B8_SRGB:
            conversion.exact = gpufmt::gl::Format{ GL_SRGB8, GL_RGB, GL_UNSIGNED_BYTE, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::B8G8R8_UNORM:
            conversion.exact = gpufmt::gl::Format{ GL_RGB8, GL_BGR, GL_UNSIGNED_BYTE, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::B8G8R8_SNORM:
            conversion.exact = gpufmt::gl::Format{ GL_RGB8_SNORM, GL_BGR, GL_BYTE, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::B8G8R8_USCALED:
            conversion.closest = gpufmt::gl::Format{ GL_RGB8UI, GL_BGR, GL_UNSIGNED_BYTE, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::B8G8R8_SSCALED:
            conversion.closest = gpufmt::gl::Format{ GL_RGB8I, GL_BGR, GL_BYTE, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
#endif
        case gpufmt::Format::B8G8R8_UINT:
            conversion.exact = gpufmt::gl::Format{ GL_RGB8UI, GL_BGR_INTEGER, GL_UNSIGNED_BYTE, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::B8G8R8_SINT:
            conversion.exact = gpufmt::gl::Format{ GL_RGB8I, GL_BGR_INTEGER, GL_BYTE, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::B8G8R8_SRGB:
            conversion.exact = gpufmt::gl::Format{ GL_SRGB8, GL_BGR, GL_UNSIGNED_BYTE, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R8G8B8A8_UNORM:
            conversion.exact = gpufmt::gl::Format{ GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R8G8B8A8_SNORM:
            conversion.exact = gpufmt::gl::Format{ GL_RGBA8_SNORM, GL_RGBA, GL_BYTE, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::R8G8B8A8_USCALED:
            conversion.closest = gpufmt::gl::Format{ GL_RGBA8UI, GL_RGBA, GL_UNSIGNED_BYTE, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R8G8B8A8_SSCALED:
            conversion.closest = gpufmt::gl::Format{ GL_RGBA8I, GL_RGBA, GL_BYTE, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
#endif
        case gpufmt::Format::R8G8B8A8_UINT:
            conversion.exact = gpufmt::gl::Format{ GL_RGBA8UI, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R8G8B8A8_SINT:
            conversion.exact = gpufmt::gl::Format{ GL_RGBA8I, GL_RGBA_INTEGER, GL_BYTE, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R8G8B8A8_SRGB:
            conversion.exact = gpufmt::gl::Format{ GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::B8G8R8A8_UNORM:
            conversion.exact = gpufmt::gl::Format{ GL_RGBA8, GL_BGRA, GL_UNSIGNED_BYTE, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::B8G8R8A8_SNORM:
            conversion.exact = gpufmt::gl::Format{ GL_RGBA8_SNORM, GL_BGRA, GL_BYTE, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::B8G8R8A8_USCALED:
            conversion.closest = gpufmt::gl::Format{ GL_RGBA8UI, GL_BGRA, GL_UNSIGNED_BYTE, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::B8G8R8A8_SSCALED:
            conversion.closest = gpufmt::gl::Format{ GL_RGBA8I, GL_BGRA, GL_BYTE, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
#endif
        case gpufmt::Format::B8G8R8A8_UINT:
            conversion.exact = gpufmt::gl::Format{ GL_RGBA8UI, GL_BGRA_INTEGER, GL_UNSIGNED_BYTE, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::B8G8R8A8_SINT:
            conversion.exact = gpufmt::gl::Format{ GL_RGBA8I, GL_BGRA_INTEGER, GL_BYTE, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::B8G8R8A8_SRGB:
            conversion.exact = gpufmt::gl::Format{ GL_SRGB8_ALPHA8, GL_BGRA, GL_UNSIGNED_BYTE, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::B8G8R8X8_UNORM:
            conversion.exact = gpufmt::gl::Format{ GL_RGBA8, GL_BGRA, GL_UNSIGNED_BYTE, {GL_RED, GL_GREEN, GL_BLUE, GL_ONE} };
            break;
        case gpufmt::Format::B8G8R8X8_SRGB:
            conversion.exact = gpufmt::gl::Format{ GL_SRGB8_ALPHA8, GL_BGRA, GL_UNSIGNED_BYTE, {GL_RED, GL_GREEN, GL_BLUE, GL_ONE} };
            break;
        case gpufmt::Format::A8B8G8R8_UNORM_PACK32:
            conversion.exact = gpufmt::gl::Format{ GL_RGBA8, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::A8B8G8R8_SNORM_PACK32:
            conversion.exact = gpufmt::gl::Format{ GL_RGBA8_SNORM, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::A8B8G8R8_USCALED_PACK32:
            conversion.closest = gpufmt::gl::Format{ GL_RGBA8UI, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::A8B8G8R8_SSCALED_PACK32:
            conversion.closest = gpufmt::gl::Format{ GL_RGBA8I, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
#endif
        case gpufmt::Format::A8B8G8R8_UINT_PACK32:
            conversion.exact = gpufmt::gl::Format{ GL_RGBA8UI, GL_RGBA_INTEGER, GL_UNSIGNED_INT_8_8_8_8_REV, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::A8B8G8R8_SINT_PACK32:
            conversion.exact = gpufmt::gl::Format{ GL_RGBA8I, GL_RGBA_INTEGER, GL_UNSIGNED_INT_8_8_8_8_REV, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::A8B8G8R8_SRGB_PACK32:
            conversion.exact = gpufmt::gl::Format{ GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::A2R10G10B10_UNORM_PACK32:
            conversion.exact = gpufmt::gl::Format{ GL_RGB10_A2, GL_BGRA, GL_UNSIGNED_INT_2_10_10_10_REV, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::A2R10G10B10_SNORM_PACK32:
            conversion.exact = gpufmt::gl::Format{ GL_RGB10_A2, GL_BGRA, GL_INT_2_10_10_10_REV, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::A2R10G10B10_USCALED_PACK32:
            conversion.closest = gpufmt::gl::Format{ GL_RGB10_A2UI, GL_BGRA, GL_UNSIGNED_INT_2_10_10_10_REV, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::A2R10G10B10_SSCALED_PACK32:
            conversion.closest = gpufmt::gl::Format{ GL_RGB10_A2UI, GL_BGRA, GL_INT_2_10_10_10_REV, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
#endif
        case gpufmt::Format::A2R10G10B10_UINT_PACK32:
            conversion.exact = gpufmt::gl::Format{ GL_RGB10_A2UI, GL_BGRA_INTEGER, GL_UNSIGNED_INT_2_10_10_10_REV, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::A2R10G10B10_SINT_PACK32:
            conversion.exact = gpufmt::gl::Format{ GL_RGB10_A2UI, GL_BGRA_INTEGER, GL_INT_2_10_10_10_REV, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::A2B10G10R10_UNORM_PACK32:
            conversion.exact = gpufmt::gl::Format{ GL_RGB10_A2, GL_RGBA, GL_UNSIGNED_INT_2_10_10_10_REV, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::A2B10G10R10_SNORM_PACK32:
            conversion.exact = gpufmt::gl::Format{ GL_RGB10_A2, GL_RGBA, GL_INT_2_10_10_10_REV, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::A2B10G10R10_USCALED_PACK32:
            conversion.closest = gpufmt::gl::Format{ GL_RGB10_A2UI, GL_RGBA, GL_UNSIGNED_INT_2_10_10_10_REV, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::A2B10G10R10_SSCALED_PACK32:
            conversion.closest = gpufmt::gl::Format{ GL_RGB10_A2UI, GL_RGBA, GL_INT_2_10_10_10_REV, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
#endif
        case gpufmt::Format::A2B10G10R10_UINT_PACK32:
            conversion.exact = gpufmt::gl::Format{ GL_RGB10_A2UI, GL_RGBA_INTEGER, GL_UNSIGNED_INT_2_10_10_10_REV, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::A2B10G10R10_SINT_PACK32:
            conversion.exact = gpufmt::gl::Format{ GL_RGB10_A2UI, GL_RGBA_INTEGER, GL_INT_2_10_10_10_REV, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R16_UNORM:
            conversion.exact = gpufmt::gl::Format{ GL_R16, GL_RED, GL_UNSIGNED_SHORT, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R16_SNORM:
            conversion.exact = gpufmt::gl::Format{ GL_R16_SNORM, GL_RED, GL_SHORT, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::R16_USCALED:
            conversion.closest = gpufmt::gl::Format{ GL_R16UI, GL_RED, GL_UNSIGNED_SHORT, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R16_SSCALED:
            conversion.closest = gpufmt::gl::Format{ GL_R16I, GL_RED, GL_SHORT, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
#endif
        case gpufmt::Format::R16_UINT:
            conversion.exact = gpufmt::gl::Format{ GL_R16UI, GL_RED_INTEGER, GL_UNSIGNED_SHORT, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R16_SINT:
            conversion.exact = gpufmt::gl::Format{ GL_R16I, GL_RED_INTEGER, GL_SHORT, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R16_SFLOAT:
            conversion.exact = gpufmt::gl::Format{ GL_R16F, GL_RED, GL_HALF_FLOAT, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R16G16_UNORM:
            conversion.exact = gpufmt::gl::Format{ GL_RG16, GL_RG, GL_UNSIGNED_SHORT, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R16G16_SNORM:
            conversion.exact = gpufmt::gl::Format{ GL_RG16_SNORM, GL_RG, GL_SHORT, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::R16G16_USCALED:
            conversion.closest = gpufmt::gl::Format{ GL_RG16UI, GL_RG, GL_UNSIGNED_SHORT, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R16G16_SSCALED:
            conversion.closest = gpufmt::gl::Format{ GL_RG16I, GL_RG, GL_SHORT, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
#endif
        case gpufmt::Format::R16G16_UINT:
            conversion.exact = gpufmt::gl::Format{ GL_RG16UI, GL_RG_INTEGER, GL_UNSIGNED_SHORT, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R16G16_SINT:
            conversion.exact = gpufmt::gl::Format{ GL_RG16I, GL_RG_INTEGER, GL_SHORT, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R16G16_SFLOAT:
            conversion.exact = gpufmt::gl::Format{ GL_RG16F, GL_RG, GL_HALF_FLOAT, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R16G16B16_UNORM:
            conversion.exact = gpufmt::gl::Format{ GL_RGB16, GL_RGB, GL_UNSIGNED_SHORT, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R16G16B16_SNORM:
            conversion.exact = gpufmt::gl::Format{ GL_RGB16_SNORM, GL_RGB, GL_SHORT, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::R16G16B16_USCALED:
            conversion.closest = gpufmt::gl::Format{ GL_RGB16UI, GL_RGB, GL_UNSIGNED_SHORT, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R16G16B16_SSCALED:
            conversion.closest = gpufmt::gl::Format{ GL_RGB16I, GL_RGB, GL_SHORT, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
#endif
        case gpufmt::Format::R16G16B16_UINT:
            conversion.exact = gpufmt::gl::Format{ GL_RGB16UI, GL_RGB_INTEGER, GL_UNSIGNED_SHORT, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R16G16B16_SINT:
            conversion.exact = gpufmt::gl::Format{ GL_RGB16I, GL_RGB_INTEGER, GL_SHORT, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R16G16B16_SFLOAT:
            conversion.exact = gpufmt::gl::Format{ GL_RGB16F, GL_RGB, GL_HALF_FLOAT, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R16G16B16A16_UNORM:
            conversion.exact = gpufmt::gl::Format{ GL_RGBA16, GL_RGBA, GL_UNSIGNED_SHORT, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R16G16B16A16_SNORM:
            conversion.exact = gpufmt::gl::Format{ GL_RGBA16_SNORM, GL_RGBA, GL_SHORT, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::R16G16B16A16_USCALED:
            conversion.closest = gpufmt::gl::Format{ GL_RGBA16UI, GL_RGBA, GL_UNSIGNED_SHORT, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R16G16B16A16_SSCALED:
            conversion.closest = gpufmt::gl::Format{ GL_RGBA16I, GL_RGBA, GL_SHORT, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
#endif
        case gpufmt::Format::R16G16B16A16_UINT:
            conversion.exact = gpufmt::gl::Format{ GL_RGBA16UI, GL_RGBA_INTEGER, GL_UNSIGNED_SHORT, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R16G16B16A16_SINT:
            conversion.exact = gpufmt::gl::Format{ GL_RGBA16I, GL_RGBA_INTEGER, GL_SHORT, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R16G16B16A16_SFLOAT:
            conversion.exact = gpufmt::gl::Format{ GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R32_UINT:
            conversion.exact = gpufmt::gl::Format{ GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R32_SINT:
            conversion.exact = gpufmt::gl::Format{ GL_R32I, GL_RED_INTEGER, GL_INT, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R32_SFLOAT:
            conversion.exact = gpufmt::gl::Format{ GL_R32F, GL_RED, GL_FLOAT, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R32G32_UINT:
            conversion.exact = gpufmt::gl::Format{ GL_RG32UI, GL_RG_INTEGER, GL_UNSIGNED_INT, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R32G32_SINT:
            conversion.exact = gpufmt::gl::Format{ GL_RG32I, GL_RG_INTEGER, GL_INT, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R32G32_SFLOAT:
            conversion.exact = gpufmt::gl::Format{ GL_RG32F, GL_RG, GL_FLOAT, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R32G32B32_UINT:
            conversion.exact = gpufmt::gl::Format{ GL_RGB32UI, GL_RGB_INTEGER, GL_UNSIGNED_INT, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R32G32B32_SINT:
            conversion.exact = gpufmt::gl::Format{ GL_RGB32I, GL_RGB_INTEGER, GL_INT, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R32G32B32_SFLOAT:
            conversion.exact = gpufmt::gl::Format{ GL_RGB32F, GL_RGB, GL_FLOAT, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R32G32B32A32_UINT:
            conversion.exact = gpufmt::gl::Format{ GL_RGBA32UI, GL_RGBA_INTEGER, GL_UNSIGNED_INT, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R32G32B32A32_SINT:
            conversion.exact = gpufmt::gl::Format{ GL_RGBA32I, GL_RGBA_INTEGER, GL_INT, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::R32G32B32A32_SFLOAT:
            conversion.exact = gpufmt::gl::Format{ GL_RGBA32F, GL_RGBA, GL_FLOAT, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
#ifndef GF_EXCLUDE_64BIT_FORMATS
        case gpufmt::Format::R64_UINT:
            //no support for 64bit formats
            break;
        case gpufmt::Format::R64_SINT:
            //no support for 64bit formats
            break;
        case gpufmt::Format::R64_SFLOAT:
            //no support for 64bit formats
            break;
        case gpufmt::Format::R64G64_UINT:
            //no support for 64bit formats
            break;
        case gpufmt::Format::R64G64_SINT:
            //no support for 64bit formats
            break;
        case gpufmt::Format::R64G64_SFLOAT:
            //no support for 64bit formats
            break;
        case gpufmt::Format::R64G64B64_UINT:
            //no support for 64bit formats
            break;
        case gpufmt::Format::R64G64B64_SINT:
            //no support for 64bit formats
            break;
        case gpufmt::Format::R64G64B64_SFLOAT:
            //no support for 64bit formats
            break;
        case gpufmt::Format::R64G64B64A64_UINT:
            //no support for 64bit formats
            break;
        case gpufmt::Format::R64G64B64A64_SINT:
            //no support for 64bit formats
            break;
        case gpufmt::Format::R64G64B64A64_SFLOAT:
            //no support for 64bit formats
            break;
#endif // GF_EXCLUDE_64BIT_FORMATS
        case gpufmt::Format::B10G11R11_UFLOAT_PACK32:
            conversion.exact = gpufmt::gl::Format{ GL_R11F_G11F_B10F, GL_RGB, GL_UNSIGNED_INT_10F_11F_11F_REV, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::E5B9G9R9_UFLOAT_PACK32:
            conversion.exact = gpufmt::gl::Format{ GL_RGB9_E5, GL_RGB, GL_UNSIGNED_INT_5_9_9_9_REV, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::D16_UNORM:
            conversion.exact = gpufmt::gl::Format{ GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::X8_D24_UNORM_PACK32:
            conversion.exact = gpufmt::gl::Format{ GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT_24_8, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::D32_SFLOAT:
            conversion.exact = gpufmt::gl::Format{ GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::S8_UINT:
            conversion.exact = gpufmt::gl::Format{ GL_STENCIL_INDEX8, GL_STENCIL, GL_UNSIGNED_BYTE, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::D16_UNORM_S8_UINT:
            break;
        case gpufmt::Format::D24_UNORM_S8_UINT:
            conversion.exact = gpufmt::gl::Format{ GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
        case gpufmt::Format::D32_SFLOAT_S8_UINT:
            conversion.exact = gpufmt::gl::Format{ GL_DEPTH32F_STENCIL8, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA} };
            break;
#ifndef GF_EXCLUDE_COMPRESSED_FORMATS
#ifndef GF_EXCLUDE_BC_COMPRESSED_FORMATS
        case gpufmt::Format::BC1_RGB_UNORM_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_RGB_S3TC_DXT1_EXT };
            break;
        case gpufmt::Format::BC1_RGB_SRGB_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_SRGB_S3TC_DXT1_EXT };
            break;
        case gpufmt::Format::BC1_RGBA_UNORM_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_RGBA_S3TC_DXT1_EXT };
            break;
        case gpufmt::Format::BC1_RGBA_SRGB_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT };
            break;
        case gpufmt::Format::BC2_UNORM_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_RGBA_S3TC_DXT3_EXT };
            break;
        case gpufmt::Format::BC2_SRGB_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT };
            break;
        case gpufmt::Format::BC3_UNORM_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_RGBA_S3TC_DXT5_EXT };
            break;
        case gpufmt::Format::BC3_SRGB_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT };
            break;
        case gpufmt::Format::BC4_UNORM_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_RED_RGTC1 };
            break;
        case gpufmt::Format::BC4_SNORM_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_SIGNED_RED_RGTC1 };
            break;
        case gpufmt::Format::BC5_UNORM_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_RG_RGTC2 };
            break;
        case gpufmt::Format::BC5_SNORM_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_SIGNED_RG_RGTC2 };
            break;
        case gpufmt::Format::BC6H_UFLOAT_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT };
            break;
        case gpufmt::Format::BC6H_SFLOAT_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT };
            break;
        case gpufmt::Format::BC7_UNORM_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_RGBA_BPTC_UNORM };
            break;
        case gpufmt::Format::BC7_SRGB_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM };
            break;
#endif //GF_EXCLUDE_BC_COMPRESSED_FORMATS
#ifndef GF_EXCLUDE_ETC_COMPRESSED_FORMATS
        case gpufmt::Format::ETC2_R8G8B8_UNORM_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_RGB8_ETC2 };
            break;
        case gpufmt::Format::ETC2_R8G8B8_SRGB_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_SRGB8_ETC2 };
            break;
        case gpufmt::Format::ETC2_R8G8B8A1_UNORM_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2 };
            break;
        case gpufmt::Format::ETC2_R8G8B8A1_SRGB_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2 };
            break;
        case gpufmt::Format::ETC2_R8G8B8A8_UNORM_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_RGBA8_ETC2_EAC };
            break;
        case gpufmt::Format::ETC2_R8G8B8A8_SRGB_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC };
            break;
        case gpufmt::Format::EAC_R11_UNORM_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_R11_EAC };
            break;
        case gpufmt::Format::EAC_R11_SNORM_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_SIGNED_R11_EAC };
            break;
        case gpufmt::Format::EAC_R11G11_UNORM_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_RG11_EAC };
            break;
        case gpufmt::Format::EAC_R11G11_SNORM_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_SIGNED_RG11_EAC };
            break;
#endif //GF_EXCLUDE_ETC_COMPRESSED_FORMATS
#ifndef GF_EXCLUDE_ASTC_COMPRESSED_FORMATS
        case gpufmt::Format::ASTC_4x4_UNORM_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_RGBA_ASTC_4x4_KHR };
            break;
        case gpufmt::Format::ASTC_4x4_SRGB_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR };
            break;
        case gpufmt::Format::ASTC_5x4_UNORM_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_RGBA_ASTC_5x4_KHR };
            break;
        case gpufmt::Format::ASTC_5x4_SRGB_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR };
            break;
        case gpufmt::Format::ASTC_5x5_UNORM_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_RGBA_ASTC_5x5_KHR };
            break;
        case gpufmt::Format::ASTC_5x5_SRGB_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR };
            break;
        case gpufmt::Format::ASTC_6x5_UNORM_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_RGBA_ASTC_6x5_KHR };
            break;
        case gpufmt::Format::ASTC_6x5_SRGB_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR };
            break;
        case gpufmt::Format::ASTC_6x6_UNORM_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_RGBA_ASTC_6x6_KHR };
            break;
        case gpufmt::Format::ASTC_6x6_SRGB_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR };
            break;
        case gpufmt::Format::ASTC_8x5_UNORM_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_RGBA_ASTC_8x5_KHR };
            break;
        case gpufmt::Format::ASTC_8x5_SRGB_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR };
            break;
        case gpufmt::Format::ASTC_8x6_UNORM_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_RGBA_ASTC_8x6_KHR };
            break;
        case gpufmt::Format::ASTC_8x6_SRGB_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR };
            break;
        case gpufmt::Format::ASTC_8x8_UNORM_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_RGBA_ASTC_8x8_KHR };
            break;
        case gpufmt::Format::ASTC_8x8_SRGB_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR };
            break;
        case gpufmt::Format::ASTC_10x5_UNORM_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_RGBA_ASTC_10x5_KHR };
            break;
        case gpufmt::Format::ASTC_10x5_SRGB_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR };
            break;
        case gpufmt::Format::ASTC_10x6_UNORM_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_RGBA_ASTC_10x6_KHR };
            break;
        case gpufmt::Format::ASTC_10x6_SRGB_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR };
            break;
        case gpufmt::Format::ASTC_10x8_UNORM_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_RGBA_ASTC_10x8_KHR };
            break;
        case gpufmt::Format::ASTC_10x8_SRGB_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR };
            break;
        case gpufmt::Format::ASTC_10x10_UNORM_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_RGBA_ASTC_10x10_KHR };
            break;
        case gpufmt::Format::ASTC_10x10_SRGB_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR };
            break;
        case gpufmt::Format::ASTC_12x10_UNORM_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_RGBA_ASTC_12x10_KHR };
            break;
        case gpufmt::Format::ASTC_12x10_SRGB_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR };
            break;
        case gpufmt::Format::ASTC_12x12_UNORM_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_RGBA_ASTC_12x12_KHR };
            break;
        case gpufmt::Format::ASTC_12x12_SRGB_BLOCK:
            conversion.exact = gpufmt::gl::Format{ GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR };
            break;
#endif //GF_EXCLUDE_ASTC_COMPRESSED_FORMATS
#ifndef GF_EXCLUDE_PVRTC_COMPRESSED_FORMATS
        case gpufmt::Format::PVRTC1_2BPP_UNORM_BLOCK_IMG:
            conversion.exact = gpufmt::gl::Format{ static_cast<GLenum>(GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG) };
            break;
        case gpufmt::Format::PVRTC1_4BPP_UNORM_BLOCK_IMG:
            conversion.exact = gpufmt::gl::Format{ static_cast<GLenum>(GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG) };
            break;
        case gpufmt::Format::PVRTC2_2BPP_UNORM_BLOCK_IMG:
            conversion.exact = gpufmt::gl::Format{ static_cast<GLenum>(GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG) };
            break;
        case gpufmt::Format::PVRTC2_4BPP_UNORM_BLOCK_IMG:
            conversion.exact = gpufmt::gl::Format{ static_cast<GLenum>(GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG) };
            break;
        case gpufmt::Format::PVRTC1_2BPP_SRGB_BLOCK_IMG:
            conversion.exact = gpufmt::gl::Format{ static_cast<GLenum>(GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV1_EXT) };
            break;
        case gpufmt::Format::PVRTC1_4BPP_SRGB_BLOCK_IMG:
            conversion.exact = gpufmt::gl::Format{ static_cast<GLenum>(GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV1_EXT) };
            break;
        case gpufmt::Format::PVRTC2_2BPP_SRGB_BLOCK_IMG:
            conversion.exact = gpufmt::gl::Format{ static_cast<GLenum>(GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV2_IMG) };
            break;
        case gpufmt::Format::PVRTC2_4BPP_SRGB_BLOCK_IMG:
            conversion.exact = gpufmt::gl::Format{ static_cast<GLenum>(GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV2_IMG) };
            break;
#endif //GF_EXCLUDE_PVRTC_COMPRESSED_FORMATS
#endif //GF_EXCLUDE_COMPRESSED_FORMATS
        default:
            break;
        }

        return conversion;
    }

    [[nodiscard]]
    static constexpr std::optional<gpufmt::Format> translateFormat(GLenum internalFormat, GLenum format, GLenum type) noexcept {
        for(gpufmt::Format searchFormat : gpufmt::FormatEnumerator()) {
            gpufmt::gl::FormatConversion conversion = translateFormat(searchFormat);

            if(!conversion.exact) {
                continue;
            }

            if(conversion.exact->internalFormat == internalFormat &&
               conversion.exact->format == format &&
               conversion.exact->type == type)
            {
                return std::make_optional(searchFormat);
            }
        }

        //opengles format ext
        if(internalFormat == GL_ALPHA8_EXT &&
           format == GL_ALPHA &&
           type == GL_UNSIGNED_BYTE)
        {
            return gpufmt::Format::A8_UNORM;
        }
        //pvrtextool outputs -format b8g8r8a8,UBN,lRGB as this monstrosity
        else if(internalFormat == GL_BGRA &&
                format == GL_BGRA &&
                type == GL_UNSIGNED_BYTE)
        {
            return gpufmt::Format::B8G8R8A8_UNORM;
        }
#ifndef GF_EXCLUDE_ETC_COMPRESSED_FORMATS
        //GL_COMPRESSED_RGB_ETC1
        else if(static_cast<uint32>(internalFormat) == GL_ETC1_RGB8_OES)
        {
            return gpufmt::Format::ETC2_R8G8B8_UNORM_BLOCK;
        }
#endif //GF_EXCLUDE_ETC_COMPRESSED_FORMATS
        return std::nullopt;
    }

    [[nodiscard]]
    static constexpr std::optional<gpufmt::gl::Format> internalFormatToFormat(GLenum internalFormat) noexcept {
        switch(internalFormat) {
        case GL_R8:
            return gpufmt::gl::Format{ internalFormat, GL_RED, GL_UNSIGNED_BYTE };
        case GL_R8_SNORM:
            return gpufmt::gl::Format{ internalFormat, GL_RED, GL_BYTE };
        case GL_R16:
            return gpufmt::gl::Format{ internalFormat, GL_RED, GL_UNSIGNED_SHORT };
        case GL_R16_SNORM:
            return gpufmt::gl::Format{ internalFormat, GL_RED, GL_SHORT };
        case GL_RG8:
            return gpufmt::gl::Format{ internalFormat, GL_RG, GL_UNSIGNED_BYTE };
        case GL_RG8_SNORM:
            return gpufmt::gl::Format{ internalFormat, GL_RG, GL_BYTE };
        case GL_RG16:
            return gpufmt::gl::Format{ internalFormat, GL_RG, GL_UNSIGNED_SHORT };
        case GL_RG16_SNORM:
            return gpufmt::gl::Format{ internalFormat, GL_RG, GL_SHORT };
        case GL_R3_G3_B2:
            return gpufmt::gl::Format{ internalFormat, GL_RGB, GL_UNSIGNED_BYTE_2_3_3_REV };
        case GL_RGB4:
            return gpufmt::gl::Format{ internalFormat, GL_RGB, GL_UNSIGNED_SHORT_4_4_4_4_REV };
        case GL_RGB5:
            return gpufmt::gl::Format{ internalFormat, GL_RGB, GL_UNSIGNED_SHORT_1_5_5_5_REV };
        case GL_RGB565:
            return gpufmt::gl::Format{ internalFormat, GL_RGB, GL_UNSIGNED_SHORT_5_6_5_REV };
        case GL_RGB8:
            return gpufmt::gl::Format{ internalFormat, GL_RGB, GL_UNSIGNED_BYTE };
        case GL_RGB8_SNORM:
            return gpufmt::gl::Format{ internalFormat, GL_RGB, GL_BYTE };
        case GL_RGB10:
            return gpufmt::gl::Format{ internalFormat, GL_RGB, GL_UNSIGNED_INT_2_10_10_10_REV };
        case GL_RGB12:
            return gpufmt::gl::Format{ internalFormat, GL_RGB, GL_UNSIGNED_SHORT };
        case GL_RGB16:
            return gpufmt::gl::Format{ internalFormat, GL_RGB, GL_UNSIGNED_SHORT };
        case GL_RGB16_SNORM:
            return gpufmt::gl::Format{ internalFormat, GL_RGB, GL_SHORT };
        case GL_RGBA2:
            return gpufmt::gl::Format{ internalFormat, GL_RGBA, GL_UNSIGNED_BYTE };
        case GL_RGBA4:
            return gpufmt::gl::Format{ internalFormat, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4_REV };
        case GL_RGB5_A1:
            return gpufmt::gl::Format{ internalFormat, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV };
        case GL_RGBA8:
            return gpufmt::gl::Format{ internalFormat, GL_RGBA, GL_UNSIGNED_BYTE };
        case GL_RGBA8_SNORM:
            return gpufmt::gl::Format{ internalFormat, GL_RGBA, GL_BYTE };
        case GL_RGB10_A2:
            return gpufmt::gl::Format{ internalFormat, GL_RGBA, GL_UNSIGNED_INT_2_10_10_10_REV };
        case GL_RGB10_A2UI:
            return gpufmt::gl::Format{ internalFormat, GL_RGBA, GL_UNSIGNED_INT_2_10_10_10_REV };
        case GL_RGBA12:
            return gpufmt::gl::Format{ internalFormat };
        case GL_RGBA16:
            return gpufmt::gl::Format{ internalFormat, GL_RGBA, GL_UNSIGNED_SHORT };
        case GL_RGBA16_SNORM:
            return gpufmt::gl::Format{ internalFormat, GL_RGBA, GL_SHORT };
        case GL_SRGB8:
            return gpufmt::gl::Format{ internalFormat, GL_RGB, GL_UNSIGNED_BYTE };
        case GL_SRGB8_ALPHA8:
            return gpufmt::gl::Format{ internalFormat, GL_RGBA, GL_UNSIGNED_BYTE };
        case GL_R16F:
            return gpufmt::gl::Format{ internalFormat, GL_RED, GL_HALF_FLOAT };
        case GL_RG16F:
            return gpufmt::gl::Format{ internalFormat, GL_RG, GL_HALF_FLOAT };
        case GL_RGB16F:
            return gpufmt::gl::Format{ internalFormat, GL_RGB, GL_HALF_FLOAT };
        case GL_RGBA16F:
            return gpufmt::gl::Format{ internalFormat, GL_RGBA, GL_HALF_FLOAT };
        case GL_R32F:
            return gpufmt::gl::Format{ internalFormat, GL_RED, GL_FLOAT };
        case GL_RG32F:
            return gpufmt::gl::Format{ internalFormat, GL_RG, GL_FLOAT };
        case GL_RGB32F:
            return gpufmt::gl::Format{ internalFormat, GL_RGB, GL_FLOAT };
        case GL_RGBA32F:
            return gpufmt::gl::Format{ internalFormat, GL_RGBA, GL_FLOAT };
        case GL_R11F_G11F_B10F:
            return gpufmt::gl::Format{ internalFormat, GL_RGB, GL_UNSIGNED_INT_10F_11F_11F_REV };
        case GL_RGB9_E5:
            return gpufmt::gl::Format{ internalFormat, GL_RGB, GL_UNSIGNED_INT_5_9_9_9_REV };
        case GL_R8UI:
            return gpufmt::gl::Format{ internalFormat, GL_RED, GL_UNSIGNED_BYTE };
        case GL_R16I:
            return gpufmt::gl::Format{ internalFormat, GL_RED, GL_SHORT };
        case GL_R16UI:
            return gpufmt::gl::Format{ internalFormat, GL_RED, GL_UNSIGNED_SHORT };
        case GL_R32I:
            return gpufmt::gl::Format{ internalFormat, GL_RED, GL_INT };
        case GL_R32UI:
            return gpufmt::gl::Format{ internalFormat, GL_RED, GL_UNSIGNED_INT };
        case GL_RG8UI:
            return gpufmt::gl::Format{ internalFormat, GL_RG, GL_UNSIGNED_BYTE };
        case GL_RG16I:
            return gpufmt::gl::Format{ internalFormat, GL_RG, GL_SHORT };
        case GL_RG16UI:
            return gpufmt::gl::Format{ internalFormat, GL_RG, GL_UNSIGNED_SHORT };
        case GL_RG32I:
            return gpufmt::gl::Format{ internalFormat, GL_RG, GL_INT };
        case GL_RG32UI:
            return gpufmt::gl::Format{ internalFormat, GL_RG, GL_UNSIGNED_INT };
        case GL_RGB8I:
            return gpufmt::gl::Format{ internalFormat, GL_RGB, GL_BYTE };
        case GL_RGB8UI:
            return gpufmt::gl::Format{ internalFormat, GL_RGB, GL_UNSIGNED_BYTE };
        case GL_RGB16I:
            return gpufmt::gl::Format{ internalFormat, GL_RGB, GL_SHORT };
        case GL_RGB16UI:
            return gpufmt::gl::Format{ internalFormat, GL_RGB, GL_UNSIGNED_SHORT };
        case GL_RGB32I:
            return gpufmt::gl::Format{ internalFormat, GL_RGB, GL_INT };
        case GL_RGB32UI:
            return gpufmt::gl::Format{ internalFormat, GL_RGB, GL_UNSIGNED_INT };
        case GL_RGBA8I:
            return gpufmt::gl::Format{ internalFormat, GL_RGBA, GL_BYTE };
        case GL_RGBA8UI:
            return gpufmt::gl::Format{ internalFormat, GL_RGBA, GL_UNSIGNED_BYTE };
        case GL_RGBA16I:
            return gpufmt::gl::Format{ internalFormat, GL_RGBA, GL_SHORT };
        case GL_RGBA16UI:
            return gpufmt::gl::Format{ internalFormat, GL_RGBA, GL_UNSIGNED_SHORT };
        case GL_RGBA32I:
            return gpufmt::gl::Format{ internalFormat, GL_RGBA, GL_INT };
        case GL_RGBA32UI:
            return gpufmt::gl::Format{ internalFormat, GL_RGBA, GL_UNSIGNED_INT };
        case GL_COMPRESSED_RED:
            return gpufmt::gl::Format{ internalFormat };
        case GL_COMPRESSED_RG:
            return gpufmt::gl::Format{ internalFormat };
        case GL_COMPRESSED_RGB:
            return gpufmt::gl::Format{ internalFormat };
        case GL_COMPRESSED_RGBA:
            return gpufmt::gl::Format{ internalFormat };
        case GL_COMPRESSED_SRGB:
            return gpufmt::gl::Format{ internalFormat };
        case GL_COMPRESSED_SRGB_ALPHA:
            return gpufmt::gl::Format{ internalFormat };
        case GL_COMPRESSED_RED_RGTC1:
            return gpufmt::gl::Format{ internalFormat };
        case GL_COMPRESSED_SIGNED_RED_RGTC1:
            return gpufmt::gl::Format{ internalFormat };
        case GL_COMPRESSED_RG_RGTC2:
            return gpufmt::gl::Format{ internalFormat };
        case GL_COMPRESSED_SIGNED_RG_RGTC2:
            return gpufmt::gl::Format{ internalFormat };
        case GL_COMPRESSED_RGBA_BPTC_UNORM:
            return gpufmt::gl::Format{ internalFormat };
        case GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM:
            return gpufmt::gl::Format{ internalFormat };
        case GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT:
            return gpufmt::gl::Format{ internalFormat };
        case GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT:
            return gpufmt::gl::Format{ internalFormat };
        case GL_COMPRESSED_RGB8_ETC2:
            return gpufmt::gl::Format{ internalFormat };
        case GL_COMPRESSED_SRGB8_ETC2:
            return gpufmt::gl::Format{ internalFormat };
        case GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2:
            return gpufmt::gl::Format{ internalFormat };
        case GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2:
            return gpufmt::gl::Format{ internalFormat };
        case GL_COMPRESSED_RGBA8_ETC2_EAC:
            return gpufmt::gl::Format{ internalFormat };
        case GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:
            return gpufmt::gl::Format{ internalFormat };
        case GL_COMPRESSED_R11_EAC:
            return gpufmt::gl::Format{ internalFormat };
        case GL_COMPRESSED_SIGNED_R11_EAC:
            return gpufmt::gl::Format{ internalFormat };
        case GL_COMPRESSED_RG11_EAC:
            return gpufmt::gl::Format{ internalFormat };
        case GL_COMPRESSED_SIGNED_RG11_EAC:
            return gpufmt::gl::Format{ internalFormat };
        case GL_ETC1_RGB8_OES:
            return gpufmt::gl::Format{ internalFormat };
        case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
            return gpufmt::gl::Format{ internalFormat };
        case GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG:
            return gpufmt::gl::Format{ internalFormat };
        case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG:
            return gpufmt::gl::Format{ internalFormat };
        case GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG:
            return gpufmt::gl::Format{ internalFormat };
        case GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG:
            return gpufmt::gl::Format{ internalFormat };
        case GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG:
            return gpufmt::gl::Format{ internalFormat };
        case GL_COMPRESSED_SRGB_PVRTC_2BPPV1_EXT:
            return gpufmt::gl::Format{ internalFormat };
        case GL_COMPRESSED_SRGB_PVRTC_4BPPV1_EXT:
            return gpufmt::gl::Format{ internalFormat };
        case GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV1_EXT:
            return gpufmt::gl::Format{ internalFormat };
        case GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV1_EXT:
            return gpufmt::gl::Format{ internalFormat };
        case GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV2_IMG:
            return gpufmt::gl::Format{ internalFormat };
        case GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV2_IMG:
            return gpufmt::gl::Format{ internalFormat };
        default:
            return std::nullopt;
        }
    }

    [[nodiscard]]
    static constexpr GLenum nextBestInternalFormatLossless(GLenum internalFormat) noexcept {
        switch(internalFormat) {
        case GL_R8:
            return GL_RG8;
        case GL_R8_SNORM:
            return GL_RG8_SNORM;
        case GL_R16:
            return GL_RG16;
        case GL_R16_SNORM:
            return GL_RG16_SNORM;
        case GL_RG8:
            return GL_RGB8;
        case GL_RG8_SNORM:
            return GL_RGB8_SNORM;
        case GL_RG16:
            return GL_RGB16;
        case GL_RG16_SNORM:
            return GL_RGB16_SNORM;
        case GL_R3_G3_B2:
            return GL_RGB4;
        case GL_RGB4:
            return GL_RGB5;
        case GL_RGB5:
            return GL_RGB565;
        case GL_RGB565:
            return GL_RGB8;
        case GL_RGB8:
            return GL_RGBA8;
        case GL_RGB8_SNORM:
            return GL_RGBA8_SNORM;
        case GL_RGB10:
            return GL_RGB12;
        case GL_RGB12:
            return GL_RGB16;
        case GL_RGB16:
            return GL_RGBA16;
        case GL_RGB16_SNORM:
            return GL_RGBA16_SNORM;
        case GL_RGBA2:
            return GL_RGBA4;
        case GL_RGBA4:
            return GL_RGBA8;
        case GL_RGB5_A1:
            return GL_RGBA8;
        case GL_RGBA8:
            return GL_RGBA12;
        case GL_RGBA8_SNORM:
            return GL_RGBA16_SNORM;
        case GL_RGB10_A2:
            return GL_RGBA12;
        case GL_RGB10_A2UI:
            return GL_RGBA16UI;
        case GL_RGBA12:
            return GL_RGBA16;
        case GL_RGBA16:
            return GL_RGBA32F;
        case GL_RGBA16_SNORM:
            return GL_RGBA32F;
        case GL_SRGB8:
            return GL_SRGB8_ALPHA8;
        case GL_SRGB8_ALPHA8:
            return GL_RGBA16;
        case GL_R16F:
            return GL_R32F;
        case GL_RG16F:
            return GL_RG32F;
        case GL_RGB16F:
            return GL_RGB32F;
        case GL_RGBA16F:
            return GL_RGBA32F;
        case GL_R32F:
            return GL_RG32F;
        case GL_RG32F:
            return GL_RGB32F;
        case GL_RGB32F:
            return GL_RGBA32F;
        case GL_RGBA32F:
            return GL_NONE;
        case GL_R11F_G11F_B10F:
            return GL_RGB16F;
        case GL_RGB9_E5:
            return GL_RGB16F;
        case GL_R8UI:
            return GL_RG8UI;
        case GL_R16I:
            return GL_RG16I;
        case GL_R16UI:
            return GL_RG16UI;
        case GL_R32I:
            return GL_RG32I;
        case GL_R32UI:
            return GL_RG32UI;
        case GL_RG8UI:
            return GL_RGB8UI;
        case GL_RG16I:
            return GL_RGB16I;
        case GL_RG16UI:
            return GL_RGB16UI;
        case GL_RG32I:
            return GL_RGB32I;
        case GL_RG32UI:
            return GL_RGB32UI;
        case GL_RGB8I:
            return GL_RGBA8I;
        case GL_RGB8UI:
            return GL_RGBA8UI;
        case GL_RGB16I:
            return GL_RGBA16I;
        case GL_RGB16UI:
            return GL_RGBA16UI;
        case GL_RGB32I:
            return GL_RGBA32I;
        case GL_RGB32UI:
            return GL_RGBA32UI;
        case GL_RGBA8I:
            return GL_RGBA16I;
        case GL_RGBA8UI:
            return GL_RGBA16UI;
        case GL_RGBA16I:
            return GL_RGBA32I;
        case GL_RGBA16UI:
            return GL_RGBA32UI;
        case GL_RGBA32I:
            return GL_NONE;
        case GL_RGBA32UI:
            return GL_NONE;
        case GL_COMPRESSED_RED:
            return GL_R8;
        case GL_COMPRESSED_RG:
            return GL_RG8;
        case GL_COMPRESSED_RGB:
            return GL_RGB8;
        case GL_COMPRESSED_RGBA:
            return GL_RGBA8;
        case GL_COMPRESSED_SRGB:
            return GL_SRGB8;
        case GL_COMPRESSED_SRGB_ALPHA:
            return GL_RGBA8;
        case GL_COMPRESSED_RED_RGTC1:
            return GL_R8;
        case GL_COMPRESSED_SIGNED_RED_RGTC1:
            return GL_R8_SNORM;
        case GL_COMPRESSED_RG_RGTC2:
            return GL_RG8;
        case GL_COMPRESSED_SIGNED_RG_RGTC2:
            return GL_RG8_SNORM;
        case GL_COMPRESSED_RGBA_BPTC_UNORM:
            return GL_RGBA8;
        case GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM:
            return GL_SRGB8_ALPHA8;
        case GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT:
            return GL_RGB16F;
        case GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT:
            return GL_RGB16F;
        case GL_COMPRESSED_RGB8_ETC2:
            return GL_RGB8;
        case GL_COMPRESSED_SRGB8_ETC2:
            return GL_SRGB8;
        case GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2:
            return GL_RGBA8;
        case GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2:
            return GL_SRGB8_ALPHA8;
        case GL_COMPRESSED_RGBA8_ETC2_EAC:
            return GL_RGBA8;
        case GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:
            return GL_SRGB8_ALPHA8;
        case GL_COMPRESSED_R11_EAC:
            return GL_R16;
        case GL_COMPRESSED_SIGNED_R11_EAC:
            return GL_R16_SNORM;
        case GL_COMPRESSED_RG11_EAC:
            return GL_RG16;
        case GL_COMPRESSED_SIGNED_RG11_EAC:
            return GL_RG16_SNORM;
        case GL_ETC1_RGB8_OES:
            return GL_RGB8;
        case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
            return GL_RGB8;
        case GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG:
            return GL_RGB8;
        case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG:
            return GL_RGBA8;
        case GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG:
            return GL_RGBA8;
        case GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG:
            return GL_RGBA8;
        case GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG:
            return GL_RGBA8;
        case GL_COMPRESSED_SRGB_PVRTC_2BPPV1_EXT:
            return GL_SRGB8;
        case GL_COMPRESSED_SRGB_PVRTC_4BPPV1_EXT:
            return GL_SRGB8;
        case GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV1_EXT:
            return GL_SRGB8_ALPHA8;
        case GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV1_EXT:
            return GL_SRGB8_ALPHA8;
        case GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV2_IMG:
            return GL_SRGB8_ALPHA8;
        case GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV2_IMG:
            return GL_SRGB8_ALPHA8;
        default:
            return GL_NONE;
        }
    }

    [[nodiscard]]
    static constexpr std::string_view internalFormatString(GLenum internalFormat) noexcept {
        switch(internalFormat)
        {
        case GL_ALPHA8_EXT:
            return "GL_ALPHA8_EXT";
        case GL_ALPHA8_SNORM:
            return "GL_ALPHA8_SNORM";
        case GL_RG8:
            return "GL_RG8";
        case GL_RGBA4:
            return "GL_RGBA4";
        case GL_RGB565:
            return "GL_RGB565";
        case GL_RGB5_A1:
            return "GL_RGB5_A1";
        case GL_R8:
            return "GL_R8";
        case GL_R8_SNORM:
            return "GL_R8_SNORM";
        case GL_R8UI:
            return "GL_R8UI";
        case GL_R8I:
            return "GL_R8I";
        case GL_SR8_EXT:
            return "GL_SR8_EXT";
        case GL_RG8_SNORM:
            return "GL_RG8_SNORM";
        case GL_RG8UI:
            return "GL_RG8UI";
        case GL_RG8I:
            return "GL_RG8I";
        case GL_RGB8:
            return "GL_RGB8";
        case GL_RGB8_SNORM:
            return "GL_RGB8_SNORM";
        case GL_RGB8UI:
            return "GL_RGB8UI";
        case GL_RGB8I:
            return "GL_RGB8I";
        case GL_SRGB8:
            return "GL_SRGB8";
        case GL_RGBA8:
            return "GL_RGBA8";
        case GL_BGRA:
            return "GL_BGRA"; //i hate that this value is here, but pvrtextool does some weird stuff
        case GL_RGBA8_SNORM:
            return "GL_RGBA8_SNORM";
        case GL_RGBA8UI:
            return "GL_RGBA8UI";
        case GL_RGBA8I:
            return "GL_RGBA8I";
        case GL_SRGB8_ALPHA8:
            return "GL_SRGB8_ALPHA8";
        case GL_RGB10_A2:
            return "GL_RGBA10_A2";
        case GL_RGB10_A2UI:
            return "GL_RGBA10_A2UI";
        case GL_R16:
            return "GL_R16";
        case GL_R16_SNORM:
            return "GL_R16_SNORM";
        case GL_R16UI:
            return "GL_R16UI";
        case GL_R16I:
            return "GL_R16I";
        case GL_R16F:
            return "GL_R16F";
        case GL_RG16:
            return "GL_RG16";
        case GL_RG16_SNORM:
            return "GL_RG16_SNORM";
        case GL_RG16UI:
            return "GL_RG16UI";
        case GL_RG16I:
            return "GL_RG16I";
        case GL_RG16F:
            return "GL_RG16F";
        case GL_RGB16:
            return "GL_RGB16";
        case GL_RGB16_SNORM:
            return "GL_RGB16_SNORM";
        case GL_RGB16UI:
            return "GL_RGB16UI";
        case GL_RGB16I:
            return "GL_RGB16I";
        case GL_RGB16F:
            return "GL_RGB16F";
        case GL_RGBA16:
            return "GL_RGBA16";
        case GL_RGBA16_SNORM:
            return "GL_RGBA16_SNORM";
        case GL_RGBA16UI:
            return "GL_RGBA16UI";
        case GL_RGBA16I:
            return "GL_RGBA16I";
        case GL_RGBA16F:
            return "GL_RGBA16F";
        case GL_R32UI:
            return "GL_R32UI";
        case GL_R32I:
            return "GL_R32I";
        case GL_R32F:
            return "GL_R32F";
        case GL_RG32UI:
            return "GL_RG32UI";
        case GL_RG32I:
            return "GL_RG32I";
        case GL_RG32F:
            return "GL_RG32F";
        case GL_RGB32UI:
            return "GL_RGB32UI";
        case GL_RGB32I:
            return "GL_RGB32I";
        case GL_RGB32F:
            return "GL_RGB32F";
        case GL_RGBA32UI:
            return "GL_RGBA32UI";
        case GL_RGBA32I:
            return "GL_RGBA32I";
        case GL_RGBA32F:
            return "GL_RGBA32F";
        case GL_R11F_G11F_B10F:
            return "GL_R11F_G11F_B10F";
        case GL_RGB9_E5:
            return "GL_RGB9_E5";
        case GL_DEPTH_COMPONENT16:
            return "GL_DEPTH_COMPONENT16";
        case GL_DEPTH_COMPONENT24:
            return "GL_DEPTH_COMPONENT24";
        case GL_DEPTH_COMPONENT32F:
            return "GL_DEPTH_COMPONENT32F";
        case GL_STENCIL_INDEX8:
            return "GL_STENCIL_INDEX8";
        case GL_DEPTH24_STENCIL8:
            return "GL_DEPTH24_STENCIL8";
        case GL_DEPTH32F_STENCIL8:
            return "GL_DEPTH32F_STENCIL8";
        case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
            return "GL_COMPRESSED_RGB_S3TC_DXT1_EXT";
        case GL_COMPRESSED_SRGB_S3TC_DXT1_EXT:
            return "GL_COMPRESSED_SRGB_S3TC_DXT1_EXT";
        case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
            return "GL_COMPRESSED_RGBA_S3TC_DXT1_EXT";
        case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT:
            return "GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT";
        case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
            return "GL_COMPRESSED_RGBA_S3TC_DXT3_EXT";
        case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT:
            return "GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT";
        case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
            return "GL_COMPRESSED_RGBA_S3TC_DXT5_EXT";
        case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT:
            return "GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT";
        case GL_COMPRESSED_RED_RGTC1:
            return "GL_COMPRESSED_RED_RGTC1";
        case GL_COMPRESSED_SIGNED_RED_RGTC1:
            return "GL_COMPRESSED_SIGNED_RED_RGTC1";
        case GL_COMPRESSED_RG_RGTC2:
            return "GL_COMPRESSED_RG_RGTC2";
        case GL_COMPRESSED_SIGNED_RG_RGTC2:
            return "GL_COMPRESSED_SIGNED_RG_RGTC2";
        case GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT:
            return "GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT";
        case GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT:
            return "GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT";
        case GL_COMPRESSED_RGBA_BPTC_UNORM:
            return "GL_COMPRESSED_RGBA_BPTC_UNORM";
        case GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM:
            return "GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM";
        case GL_COMPRESSED_RGB8_ETC2:
            return "GL_COMPRESSED_RGB8_ETC2";
        case GL_COMPRESSED_SRGB8_ETC2:
            return "GL_COMPRESSED_SRGB8_ETC2";
        case GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2:
            return "GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2";
        case GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2:
            return "GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2";
        case GL_COMPRESSED_RGBA8_ETC2_EAC:
            return "GL_COMPRESSED_RGBA8_ETC2_EAC";
        case GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:
            return "GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC";
        case GL_COMPRESSED_R11_EAC:
            return "GL_COMPRESSED_R11_EAC";
        case GL_COMPRESSED_SIGNED_R11_EAC:
            return "GL_COMPRESSED_SIGNED_R11_EAC";
        case GL_COMPRESSED_RG11_EAC:
            return "GL_COMPRESSED_RG11_EAC";
        case GL_COMPRESSED_SIGNED_RG11_EAC:
            return "GL_COMPRESSED_SIGNED_RG11_EAC";
        case GL_COMPRESSED_RGBA_ASTC_4x4_KHR:
            return "GL_COMPRESSED_RGBA_ASTC_4x4_KHR";
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR:
            return "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR";
        case GL_COMPRESSED_RGBA_ASTC_5x4_KHR:
            return "GL_COMPRESSED_RGBA_ASTC_5x4_KHR";
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR:
            return "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR";
        case GL_COMPRESSED_RGBA_ASTC_5x5_KHR:
            return "GL_COMPRESSED_RGBA_ASTC_5x5_KHR";
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR:
            return "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR";
        case GL_COMPRESSED_RGBA_ASTC_6x5_KHR:
            return "GL_COMPRESSED_RGBA_ASTC_6x5_KHR";
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR:
            return "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR";
        case GL_COMPRESSED_RGBA_ASTC_6x6_KHR:
            return "GL_COMPRESSED_RGBA_ASTC_6x6_KHR";
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR:
            return "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR";
        case GL_COMPRESSED_RGBA_ASTC_8x5_KHR:
            return "GL_COMPRESSED_RGBA_ASTC_8x5_KHR";
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR:
            return "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR";
        case GL_COMPRESSED_RGBA_ASTC_8x6_KHR:
            return "GL_COMPRESSED_RGBA_ASTC_8x6_KHR";
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR:
            return "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR";
        case GL_COMPRESSED_RGBA_ASTC_8x8_KHR:
            return "GL_COMPRESSED_RGBA_ASTC_8x8_KHR";
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR:
            return "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR";
        case GL_COMPRESSED_RGBA_ASTC_10x5_KHR:
            return "GL_COMPRESSED_RGBA_ASTC_10x5_KHR";
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR:
            return "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR";
        case GL_COMPRESSED_RGBA_ASTC_10x6_KHR:
            return "GL_COMPRESSED_RGBA_ASTC_10x6_KHR";
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR:
            return "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR";
        case GL_COMPRESSED_RGBA_ASTC_10x8_KHR:
            return "GL_COMPRESSED_RGBA_ASTC_10x8_KHR";
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR:
            return "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR";
        case GL_COMPRESSED_RGBA_ASTC_10x10_KHR:
            return "GL_COMPRESSED_RGBA_ASTC_10x10_KHR";
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR:
            return "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR";
        case GL_COMPRESSED_RGBA_ASTC_12x10_KHR:
            return "GL_COMPRESSED_RGBA_ASTC_12x10_KHR";
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR:
            return "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR";
        case GL_COMPRESSED_RGBA_ASTC_12x12_KHR:
            return "GL_COMPRESSED_RGBA_ASTC_12x12_KHR";
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR:
            return "GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR";
        case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
            return "GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG";
        case GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG:
            return "GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG";
        case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG:
            return "GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG";
        case GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG:
            return "GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG";
        case GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG:
            return "GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG";
        case GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG:
            return "GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG";
        case GL_COMPRESSED_SRGB_PVRTC_2BPPV1_EXT:
            return "GL_COMPRESSED_SRGB_PVRTC_2BPPV1_EXT";
        case GL_COMPRESSED_SRGB_PVRTC_4BPPV1_EXT:
            return "GL_COMPRESSED_SRGB_PVRTC_4BPPV1_EXT";
        case GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV1_EXT:
            return "GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV1_EXT";
        case GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV1_EXT:
            return "GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV1_EXT";
        case GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV2_IMG:
            return "GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV2_IMG";
        case GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV2_IMG:
            return "GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV2_IMG";
        case 0:
            return "0";
        default:
            return "GL_INVALID_ENUM";
        }
    }

    [[nodiscard]]
    static constexpr std::string_view formatString(GLenum format) noexcept {
        switch(format) {
        case GL_RG:
            return "GL_RG";
        case GL_RGBA:
            return "GL_RGBA";
        case GL_BGRA:
            return "GL_BGRA";
        case GL_RGB:
            return "GL_RGB";
        case GL_BGR:
            return "GL_BGR";
        case GL_RED:
            return "GL_RED";
        case GL_ALPHA:
            return "GL_ALPHA";
        case GL_RED_INTEGER:
            return "GL_RED_INTEGER";
        case GL_RG_INTEGER:
            return "GL_RG_INTEGER";
        case GL_RGB_INTEGER:
            return "GL_RGB_INTEGER";
        case GL_BGR_INTEGER:
            return "GL_BGR_INTEGER";
        case GL_RGBA_INTEGER:
            return "GL_RGBA_INTEGER";
        case GL_BGRA_INTEGER:
            return "GL_BGRA_INTEGER";
        case GL_DEPTH_COMPONENT:
            return "GL_DEPTH_COMPONENT";
        case GL_STENCIL:
            return "GL_STENCIL";
        case GL_DEPTH_STENCIL:
            return "GL_DEPTH_STENCIL";
        case 0:
            return "0";
        default:
            return "GL_INVALID_ENUM";
        }
    }

    [[nodiscard]]
    static constexpr std::string_view typeString(GLenum type) noexcept {
        switch(type)
        {
        case GL_UNSIGNED_BYTE:
            return "GL_UNSIGNED_BYTE";
        case GL_UNSIGNED_SHORT_4_4_4_4:
            return "GL_UNSIGNED_SHORT_4_4_4_4";
        case GL_UNSIGNED_SHORT_4_4_4_4_REV:
            return "GL_UNSIGNED_SHORT_4_4_4_4_REV";
        case GL_UNSIGNED_SHORT_5_6_5:
            return "GL_UNSIGNED_SHORT_5_6_5";
        case GL_UNSIGNED_SHORT_5_6_5_REV:
            return "GL_UNSIGNED_SHORT_5_6_5_REV";
        case GL_UNSIGNED_SHORT_5_5_5_1:
            return "GL_UNSIGNED_SHORT_5_5_5_1";
        case GL_UNSIGNED_SHORT_1_5_5_5_REV:
            return "GL_UNSIGNED_SHORT_1_5_5_5_REV";
        case GL_BYTE:
            return "GL_BYTE";
        case GL_UNSIGNED_INT_8_8_8_8:
            return "GL_UNSIGNED_INT_8_8_8_8";
        case GL_UNSIGNED_INT_8_8_8_8_REV:
            return "GL_UNSIGNED_INT_8_8_8_8_REV";
        case GL_UNSIGNED_INT_2_10_10_10_REV:
            return "GL_UNSIGNED_INT_2_10_10_10_REV";
        case GL_UNSIGNED_SHORT:
            return "GL_UNSIGNED_SHORT";
        case GL_SHORT:
            return "GL_SHORT";
        case GL_HALF_FLOAT:
            return "GL_HALF_FLOAT";
        case GL_UNSIGNED_INT:
            return "GL_UNSIGNED_INT";
        case GL_INT:
            return "GL_INT";
        case GL_FLOAT:
            return "GL_FLOAT";
        case GL_UNSIGNED_INT_10F_11F_11F_REV:
            return "GL_UNSIGNED_INT_10F_11F_11F_REV";
        case GL_UNSIGNED_INT_5_9_9_9_REV:
            return "GL_UNSIGNED_INT_5_9_9_9_REV";
        case GL_UNSIGNED_INT_24_8:
            return "GL_UNSIGNED_INT_24_8";
        case GL_FLOAT_32_UNSIGNED_INT_24_8_REV:
            return "GL_FLOAT_32_UNSIGNED_INT_24_8_REV";
        case 0:
            return "0";
        default:
            return "GL_INVALID_ENUM";
        }
    }

    [[nodiscard]]
    static constexpr std::string_view swizzleString(GLenum swizzleValue) noexcept {
        switch(swizzleValue) {
        case GL_RED:
            return "GL_RED";
        case GL_GREEN:
            return "GL_GREEN";
        case GL_BLUE:
            return "GL_BLUE";
        case GL_ALPHA:
            return "GL_ALPHA";
        case GL_ZERO:
            return "GL_ZERO";
        case GL_ONE:
            return "GL_ONE";
        default:
            return "GL_INVALID_ENUM";
        }
    }

    static constexpr bool formatIsCompressed(FormatConversion format) {
        if(!format) {
            return false;
        }

        GLenum internalFormat;

        if(format.exact) {
            internalFormat = format.exact->internalFormat;
        } else {
            internalFormat = format.closest->internalFormat;
        }

        switch(internalFormat) {
        case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
            return true;
        case GL_COMPRESSED_SRGB_S3TC_DXT1_EXT:
            return true;
        case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
            return true;
        case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT:
            return true;
        case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
            return true;
        case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT:
            return true;
        case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
            return true;
        case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT:
            return true;
        case GL_COMPRESSED_RED_RGTC1:
            return true;
        case GL_COMPRESSED_SIGNED_RED_RGTC1:
            return true;
        case GL_COMPRESSED_RG_RGTC2:
            return true;
        case GL_COMPRESSED_SIGNED_RG_RGTC2:
            return true;
        case GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT:
            return true;
        case GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT:
            return true;
        case GL_COMPRESSED_RGBA_BPTC_UNORM:
            return true;
        case GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM:
            return true;
        case GL_COMPRESSED_RGB8_ETC2:
            return true;
        case GL_COMPRESSED_SRGB8_ETC2:
            return true;
        case GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2:
            return true;
        case GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2:
            return true;
        case GL_COMPRESSED_RGBA8_ETC2_EAC:
            return true;
        case GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:
            return true;
        case GL_COMPRESSED_R11_EAC:
            return true;
        case GL_COMPRESSED_SIGNED_R11_EAC:
            return true;
        case GL_COMPRESSED_RG11_EAC:
            return true;
        case GL_COMPRESSED_SIGNED_RG11_EAC:
            return true;
        case GL_COMPRESSED_RGBA_ASTC_4x4_KHR:
            return true;
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR:
            return true;
        case GL_COMPRESSED_RGBA_ASTC_5x4_KHR:
            return true;
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR:
            return true;
        case GL_COMPRESSED_RGBA_ASTC_5x5_KHR:
            return true;
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR:
            return true;
        case GL_COMPRESSED_RGBA_ASTC_6x5_KHR:
            return true;
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR:
            return true;
        case GL_COMPRESSED_RGBA_ASTC_6x6_KHR:
            return true;
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR:
            return true;
        case GL_COMPRESSED_RGBA_ASTC_8x5_KHR:
            return true;
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR:
            return true;
        case GL_COMPRESSED_RGBA_ASTC_8x6_KHR:
            return true;
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR:
            return true;
        case GL_COMPRESSED_RGBA_ASTC_8x8_KHR:
            return true;
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR:
            return true;
        case GL_COMPRESSED_RGBA_ASTC_10x5_KHR:
            return true;
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR:
            return true;
        case GL_COMPRESSED_RGBA_ASTC_10x6_KHR:
            return true;
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR:
            return true;
        case GL_COMPRESSED_RGBA_ASTC_10x8_KHR:
            return true;
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR:
            return true;
        case GL_COMPRESSED_RGBA_ASTC_10x10_KHR:
            return true;
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR:
            return true;
        case GL_COMPRESSED_RGBA_ASTC_12x10_KHR:
            return true;
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR:
            return true;
        case GL_COMPRESSED_RGBA_ASTC_12x12_KHR:
            return true;
        case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR:
            return true;
        default:
            return false;
        }
    }
}
