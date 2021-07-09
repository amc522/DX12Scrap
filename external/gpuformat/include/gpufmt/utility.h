#pragma once

#include <gpufmt/format.h>

#include <cassert>
#include <optional>

namespace gpufmt {
    class FormatIterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = gpufmt::Format;
        using difference_type = ptrdiff_t;
        using reference = const value_type;
        using pointer = std::add_pointer_t<reference>;

        FormatIterator() = default;

        explicit constexpr FormatIterator(gpufmt::Format format) noexcept
            : mFormat(format)
        {}

        constexpr reference operator*() const {
            return mFormat;
        }

        constexpr FormatIterator &operator++() {
            mFormat = static_cast<Format>(static_cast<std::underlying_type_t<Format>>(mFormat) + 1);

            assert(mFormat >= Format::First && mFormat <= static_cast<Format>(static_cast<std::underlying_type_t<Format>>(Format::Last) + 1));
            return *this;
        }

        constexpr FormatIterator &operator++(int) {
            auto ret = *this;
            ++(*this);
            return ret;
        }

        constexpr friend bool operator==(FormatIterator lhs,
                                         FormatIterator rhs) noexcept
        {
            return lhs.mFormat == rhs.mFormat;
        }

        constexpr friend bool operator!=(FormatIterator lhs,
                                         FormatIterator rhs) noexcept
        {
            return !(lhs == rhs);
        }

        constexpr friend bool operator<(FormatIterator lhs,
                                        FormatIterator rhs) noexcept
        {
            return lhs.mFormat < rhs.mFormat;
        }

        constexpr friend bool operator<=(FormatIterator lhs,
                                         FormatIterator rhs) noexcept
        {
            return !(rhs < lhs);
        }

        constexpr friend bool operator>(FormatIterator lhs,
                                        FormatIterator rhs) noexcept
        {
            return rhs < lhs;
        }

        constexpr friend bool operator>=(FormatIterator lhs,
                                         FormatIterator rhs) noexcept
        {
            return !(rhs > lhs);
        }
    private:
        gpufmt::Format mFormat = Format::First;
    };

    template <Format StartFormat = Format::First, Format EndFormat = Format::Last>
    class FormatEnumerator {
    public:
        static_assert(StartFormat >= Format::First && StartFormat <= Format::Last);
        static_assert(EndFormat >= Format::First && EndFormat <= Format::Last);

        using iterator = FormatIterator;
        using const_iterator = iterator;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        constexpr iterator begin() const noexcept {
            return FormatIterator{ StartFormat };
        }

        constexpr iterator end() const noexcept {
            using UnderlyingType = std::underlying_type_t<Format>;
            return FormatIterator{ static_cast<Format>(static_cast<UnderlyingType>(EndFormat) + UnderlyingType(1)) };
        }

        constexpr const_iterator cbegin() const noexcept {
            return FormatIterator{ Format::First };
        }

        constexpr const_iterator cend() const noexcept {
            using UnderlyingType = std::underlying_type_t<StartFormat>;
            return FormatIterator{ static_cast<Format>(static_cast<UnderlyingType>(EndFormat) + UnderlyingType(1)) };
        }

        constexpr reverse_iterator rbegin() const noexcept {
            return reverse_iterator{ end() };
        }

        constexpr reverse_iterator rend() const noexcept {
            return reverse_iterator{ begin() };
        }

        constexpr const_reverse_iterator crbegin() const noexcept {
            return const_reverse_iterator{ cend() };
        }

        constexpr const_reverse_iterator crend() const noexcept {
            return const_reverse_iterator{ cbegin() };
        }
    };

    inline std::optional<gpufmt::Format> sRGBFormat(gpufmt::Format format) {
        std::optional<gpufmt::Format> srgb;

        switch(format)
        {
        case gpufmt::Format::UNDEFINED:
            break;
        case gpufmt::Format::R4G4_UNORM_PACK8:
            break;
        case gpufmt::Format::R4G4B4A4_UNORM_PACK16:
            break;
        case gpufmt::Format::B4G4R4A4_UNORM_PACK16:
            break;
        case gpufmt::Format::A4R4G4B4_UNORM_PACK16:
            break;
        case gpufmt::Format::R5G6B5_UNORM_PACK16:
            break;
        case gpufmt::Format::B5G6R5_UNORM_PACK16:
            break;
        case gpufmt::Format::R5G5B5A1_UNORM_PACK16:
            break;
        case gpufmt::Format::B5G5R5A1_UNORM_PACK16:
            break;
        case gpufmt::Format::A1R5G5B5_UNORM_PACK16:
            break;
        case gpufmt::Format::R8_UNORM:
            srgb = gpufmt::Format::R8_SRGB;
            break;
        case gpufmt::Format::R8_SNORM:
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::R8_USCALED:
            break;
        case gpufmt::Format::R8_SSCALED:
            break;
#endif
        case gpufmt::Format::R8_UINT:
            break;
        case gpufmt::Format::R8_SINT:
            break;
        case gpufmt::Format::R8_SRGB:
            srgb = gpufmt::Format::R8_SRGB;
            break;
        case gpufmt::Format::R8G8_UNORM:
            srgb = gpufmt::Format::R8G8_SRGB;
            break;
        case gpufmt::Format::R8G8_SNORM:
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::R8G8_USCALED:
            break;
        case gpufmt::Format::R8G8_SSCALED:
            break;
#endif
        case gpufmt::Format::R8G8_UINT:
            break;
        case gpufmt::Format::R8G8_SINT:
            break;
        case gpufmt::Format::R8G8_SRGB:
            srgb = gpufmt::Format::R8G8_SRGB;
            break;
        case gpufmt::Format::R8G8B8_UNORM:
            srgb = gpufmt::Format::R8G8B8_SRGB;
            break;
        case gpufmt::Format::R8G8B8_SNORM:
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::R8G8B8_USCALED:
            break;
        case gpufmt::Format::R8G8B8_SSCALED:
            break;
#endif
        case gpufmt::Format::R8G8B8_UINT:
            break;
        case gpufmt::Format::R8G8B8_SINT:
            break;
        case gpufmt::Format::R8G8B8_SRGB:
            srgb = gpufmt::Format::R8G8B8_SRGB;
            break;
        case gpufmt::Format::B8G8R8_UNORM:
            srgb = gpufmt::Format::B8G8R8_SRGB;
            break;
        case gpufmt::Format::B8G8R8_SNORM:
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::B8G8R8_USCALED:
            break;
        case gpufmt::Format::B8G8R8_SSCALED:
            break;
#endif
        case gpufmt::Format::B8G8R8_UINT:
            break;
        case gpufmt::Format::B8G8R8_SINT:
            break;
        case gpufmt::Format::B8G8R8_SRGB:
            srgb = gpufmt::Format::B8G8R8_SRGB;
            break;
        case gpufmt::Format::R8G8B8A8_UNORM:
            srgb = gpufmt::Format::R8G8B8A8_SRGB;
            break;
        case gpufmt::Format::R8G8B8A8_SNORM:
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::R8G8B8A8_USCALED:
            break;
        case gpufmt::Format::R8G8B8A8_SSCALED:
            break;
#endif
        case gpufmt::Format::R8G8B8A8_UINT:
            break;
        case gpufmt::Format::R8G8B8A8_SINT:
            break;
        case gpufmt::Format::R8G8B8A8_SRGB:
            srgb = gpufmt::Format::R8G8B8A8_SRGB;
            break;
        case gpufmt::Format::B8G8R8A8_UNORM:
            srgb = gpufmt::Format::B8G8R8A8_SRGB;
            break;
        case gpufmt::Format::B8G8R8A8_SNORM:
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::B8G8R8A8_USCALED:
            break;
        case gpufmt::Format::B8G8R8A8_SSCALED:
            break;
#endif
        case gpufmt::Format::B8G8R8A8_UINT:
            break;
        case gpufmt::Format::B8G8R8A8_SINT:
            break;
        case gpufmt::Format::B8G8R8A8_SRGB:
            srgb = gpufmt::Format::B8G8R8A8_SRGB;
            break;
        case gpufmt::Format::B8G8R8X8_UNORM:
            srgb = gpufmt::Format::B8G8R8X8_SRGB;
            break;
        case gpufmt::Format::B8G8R8X8_SRGB:
            srgb = gpufmt::Format::B8G8R8X8_SRGB;
            break;
        case gpufmt::Format::A8B8G8R8_UNORM_PACK32:
            srgb = gpufmt::Format::A8B8G8R8_SRGB_PACK32;
            break;
        case gpufmt::Format::A8B8G8R8_SNORM_PACK32:
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::A8B8G8R8_USCALED_PACK32:
            break;
        case gpufmt::Format::A8B8G8R8_SSCALED_PACK32:
            break;
#endif
        case gpufmt::Format::A8B8G8R8_UINT_PACK32:
            break;
        case gpufmt::Format::A8B8G8R8_SINT_PACK32:
            break;
        case gpufmt::Format::A8B8G8R8_SRGB_PACK32:
            srgb = gpufmt::Format::A8B8G8R8_SRGB_PACK32;
            break;
        case gpufmt::Format::A2R10G10B10_UNORM_PACK32:
            break;
        case gpufmt::Format::A2R10G10B10_SNORM_PACK32:
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::A2R10G10B10_USCALED_PACK32:
            break;
        case gpufmt::Format::A2R10G10B10_SSCALED_PACK32:
            break;
#endif
        case gpufmt::Format::A2R10G10B10_UINT_PACK32:
            break;
        case gpufmt::Format::A2R10G10B10_SINT_PACK32:
            break;
        case gpufmt::Format::A2B10G10R10_UNORM_PACK32:
            break;
        case gpufmt::Format::A2B10G10R10_SNORM_PACK32:
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::A2B10G10R10_USCALED_PACK32:
            break;
        case gpufmt::Format::A2B10G10R10_SSCALED_PACK32:
            break;
#endif
        case gpufmt::Format::A2B10G10R10_UINT_PACK32:
            break;
        case gpufmt::Format::A2B10G10R10_SINT_PACK32:
            break;
        case gpufmt::Format::R16_UNORM:
            break;
        case gpufmt::Format::R16_SNORM:
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::R16_USCALED:
            break;
        case gpufmt::Format::R16_SSCALED:
            break;
#endif
        case gpufmt::Format::R16_UINT:
            break;
        case gpufmt::Format::R16_SINT:
            break;
        case gpufmt::Format::R16_SFLOAT:
            break;
        case gpufmt::Format::R16G16_UNORM:
            break;
        case gpufmt::Format::R16G16_SNORM:
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::R16G16_USCALED:
            break;
        case gpufmt::Format::R16G16_SSCALED:
            break;
#endif
        case gpufmt::Format::R16G16_UINT:
            break;
        case gpufmt::Format::R16G16_SINT:
            break;
        case gpufmt::Format::R16G16_SFLOAT:
            break;
        case gpufmt::Format::R16G16B16_UNORM:
            break;
        case gpufmt::Format::R16G16B16_SNORM:
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::R16G16B16_USCALED:
            break;
        case gpufmt::Format::R16G16B16_SSCALED:
            break;
#endif
        case gpufmt::Format::R16G16B16_UINT:
            break;
        case gpufmt::Format::R16G16B16_SINT:
            break;
        case gpufmt::Format::R16G16B16_SFLOAT:
            break;
        case gpufmt::Format::R16G16B16A16_UNORM:
            break;
        case gpufmt::Format::R16G16B16A16_SNORM:
            break;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        case gpufmt::Format::R16G16B16A16_USCALED:
            break;
        case gpufmt::Format::R16G16B16A16_SSCALED:
            break;
#endif
        case gpufmt::Format::R16G16B16A16_UINT:
            break;
        case gpufmt::Format::R16G16B16A16_SINT:
            break;
        case gpufmt::Format::R16G16B16A16_SFLOAT:
            break;
        case gpufmt::Format::R32_UINT:
            break;
        case gpufmt::Format::R32_SINT:
            break;
        case gpufmt::Format::R32_SFLOAT:
            break;
        case gpufmt::Format::R32G32_UINT:
            break;
        case gpufmt::Format::R32G32_SINT:
            break;
        case gpufmt::Format::R32G32_SFLOAT:
            break;
        case gpufmt::Format::R32G32B32_UINT:
            break;
        case gpufmt::Format::R32G32B32_SINT:
            break;
        case gpufmt::Format::R32G32B32_SFLOAT:
            break;
        case gpufmt::Format::R32G32B32A32_UINT:
            break;
        case gpufmt::Format::R32G32B32A32_SINT:
            break;
        case gpufmt::Format::R32G32B32A32_SFLOAT:
            break;
#ifndef GF_EXCLUDE_64BIT_FORMATS
        case gpufmt::Format::R64_UINT:
            break;
        case gpufmt::Format::R64_SINT:
            break;
        case gpufmt::Format::R64_SFLOAT:
            break;
        case gpufmt::Format::R64G64_UINT:
            break;
        case gpufmt::Format::R64G64_SINT:
            break;
        case gpufmt::Format::R64G64_SFLOAT:
            break;
        case gpufmt::Format::R64G64B64_UINT:
            break;
        case gpufmt::Format::R64G64B64_SINT:
            break;
        case gpufmt::Format::R64G64B64_SFLOAT:
            break;
        case gpufmt::Format::R64G64B64A64_UINT:
            break;
        case gpufmt::Format::R64G64B64A64_SINT:
            break;
        case gpufmt::Format::R64G64B64A64_SFLOAT:
            break;
#endif
        case gpufmt::Format::B10G11R11_UFLOAT_PACK32:
            break;
        case gpufmt::Format::E5B9G9R9_UFLOAT_PACK32:
            break;
        case gpufmt::Format::D16_UNORM:
            break;
        case gpufmt::Format::X8_D24_UNORM_PACK32:
            break;
        case gpufmt::Format::D32_SFLOAT:
            break;
        case gpufmt::Format::S8_UINT:
            break;
        case gpufmt::Format::D16_UNORM_S8_UINT:
            break;
        case gpufmt::Format::D24_UNORM_S8_UINT:
            break;
        case gpufmt::Format::D32_SFLOAT_S8_UINT:
            break;
#ifndef GF_EXCLUDE_COMPRESSED_FORMATS
#ifndef GF_EXCLUDE_BC_COMPRESSED_FORMATS
        case gpufmt::Format::BC1_RGB_UNORM_BLOCK:
            srgb = gpufmt::Format::BC1_RGB_SRGB_BLOCK;
            break;
        case gpufmt::Format::BC1_RGB_SRGB_BLOCK:
            srgb = gpufmt::Format::BC1_RGB_SRGB_BLOCK;
            break;
        case gpufmt::Format::BC1_RGBA_UNORM_BLOCK:
            srgb = gpufmt::Format::BC1_RGBA_SRGB_BLOCK;
            break;
        case gpufmt::Format::BC1_RGBA_SRGB_BLOCK:
            srgb = gpufmt::Format::BC1_RGBA_SRGB_BLOCK;
            break;
        case gpufmt::Format::BC2_UNORM_BLOCK:
            srgb = gpufmt::Format::BC2_SRGB_BLOCK;
            break;
        case gpufmt::Format::BC2_SRGB_BLOCK:
            srgb = gpufmt::Format::BC2_SRGB_BLOCK;
            break;
        case gpufmt::Format::BC3_UNORM_BLOCK:
            srgb = gpufmt::Format::BC3_SRGB_BLOCK;
            break;
        case gpufmt::Format::BC3_SRGB_BLOCK:
            srgb = gpufmt::Format::BC3_SRGB_BLOCK;
            break;
        case gpufmt::Format::BC4_UNORM_BLOCK:
            break;
        case gpufmt::Format::BC4_SNORM_BLOCK:
            break;
        case gpufmt::Format::BC5_UNORM_BLOCK:
            break;
        case gpufmt::Format::BC5_SNORM_BLOCK:
            break;
        case gpufmt::Format::BC6H_UFLOAT_BLOCK:
            break;
        case gpufmt::Format::BC6H_SFLOAT_BLOCK:
            break;
        case gpufmt::Format::BC7_UNORM_BLOCK:
            srgb = gpufmt::Format::BC7_SRGB_BLOCK;
            break;
        case gpufmt::Format::BC7_SRGB_BLOCK:
            srgb = gpufmt::Format::BC7_SRGB_BLOCK;
            break;
#endif //GF_EXCLUDE_BC_COMPRESSED_FORMATS
#ifndef GF_EXCLUDE_ETC_COMPRESSED_FORMATS
        case gpufmt::Format::ETC2_R8G8B8_UNORM_BLOCK:
            srgb = gpufmt::Format::ETC2_R8G8B8_SRGB_BLOCK;
            break;
        case gpufmt::Format::ETC2_R8G8B8_SRGB_BLOCK:
            srgb = gpufmt::Format::ETC2_R8G8B8_SRGB_BLOCK;
            break;
        case gpufmt::Format::ETC2_R8G8B8A1_UNORM_BLOCK:
            srgb = gpufmt::Format::ETC2_R8G8B8A1_SRGB_BLOCK;
            break;
        case gpufmt::Format::ETC2_R8G8B8A1_SRGB_BLOCK:
            srgb = gpufmt::Format::ETC2_R8G8B8A1_SRGB_BLOCK;
            break;
        case gpufmt::Format::ETC2_R8G8B8A8_UNORM_BLOCK:
            srgb = gpufmt::Format::ETC2_R8G8B8A8_SRGB_BLOCK;
            break;
        case gpufmt::Format::ETC2_R8G8B8A8_SRGB_BLOCK:
            srgb = gpufmt::Format::ETC2_R8G8B8A8_SRGB_BLOCK;
            break;
        case gpufmt::Format::EAC_R11_UNORM_BLOCK:
            break;
        case gpufmt::Format::EAC_R11_SNORM_BLOCK:
            break;
        case gpufmt::Format::EAC_R11G11_UNORM_BLOCK:
            break;
        case gpufmt::Format::EAC_R11G11_SNORM_BLOCK:
            break;
#endif //GF_EXCLUDE_ETC_COMPRESSED_FORMATS
#ifndef GF_EXCLUDE_ASTC_COMPRESSED_FORMATS
        case gpufmt::Format::ASTC_4x4_UNORM_BLOCK:
            srgb = gpufmt::Format::ASTC_4x4_SRGB_BLOCK;
            break;
        case gpufmt::Format::ASTC_4x4_SRGB_BLOCK:
            srgb = gpufmt::Format::ASTC_4x4_SRGB_BLOCK;
            break;
        case gpufmt::Format::ASTC_5x4_UNORM_BLOCK:
            srgb = gpufmt::Format::ASTC_5x4_SRGB_BLOCK;
            break;
        case gpufmt::Format::ASTC_5x4_SRGB_BLOCK:
            srgb = gpufmt::Format::ASTC_5x4_SRGB_BLOCK;
            break;
        case gpufmt::Format::ASTC_5x5_UNORM_BLOCK:
            srgb = gpufmt::Format::ASTC_5x5_SRGB_BLOCK;
            break;
        case gpufmt::Format::ASTC_5x5_SRGB_BLOCK:
            srgb = gpufmt::Format::ASTC_5x5_SRGB_BLOCK;
            break;
        case gpufmt::Format::ASTC_6x5_UNORM_BLOCK:
            srgb = gpufmt::Format::ASTC_6x5_SRGB_BLOCK;
            break;
        case gpufmt::Format::ASTC_6x5_SRGB_BLOCK:
            srgb = gpufmt::Format::ASTC_6x5_SRGB_BLOCK;
            break;
        case gpufmt::Format::ASTC_6x6_UNORM_BLOCK:
            srgb = gpufmt::Format::ASTC_6x6_SRGB_BLOCK;
            break;
        case gpufmt::Format::ASTC_6x6_SRGB_BLOCK:
            srgb = gpufmt::Format::ASTC_6x6_SRGB_BLOCK;
            break;
        case gpufmt::Format::ASTC_8x5_UNORM_BLOCK:
            srgb = gpufmt::Format::ASTC_8x5_SRGB_BLOCK;
            break;
        case gpufmt::Format::ASTC_8x5_SRGB_BLOCK:
            srgb = gpufmt::Format::ASTC_8x5_SRGB_BLOCK;
            break;
        case gpufmt::Format::ASTC_8x6_UNORM_BLOCK:
            srgb = gpufmt::Format::ASTC_8x6_SRGB_BLOCK;
            break;
        case gpufmt::Format::ASTC_8x6_SRGB_BLOCK:
            srgb = gpufmt::Format::ASTC_8x6_SRGB_BLOCK;
            break;
        case gpufmt::Format::ASTC_8x8_UNORM_BLOCK:
            srgb = gpufmt::Format::ASTC_8x8_SRGB_BLOCK;
            break;
        case gpufmt::Format::ASTC_8x8_SRGB_BLOCK:
            srgb = gpufmt::Format::ASTC_8x8_SRGB_BLOCK;
            break;
        case gpufmt::Format::ASTC_10x5_UNORM_BLOCK:
            srgb = gpufmt::Format::ASTC_10x5_SRGB_BLOCK;
            break;
        case gpufmt::Format::ASTC_10x5_SRGB_BLOCK:
            srgb = gpufmt::Format::ASTC_10x5_SRGB_BLOCK;
            break;
        case gpufmt::Format::ASTC_10x6_UNORM_BLOCK:
            srgb = gpufmt::Format::ASTC_10x6_SRGB_BLOCK;
            break;
        case gpufmt::Format::ASTC_10x6_SRGB_BLOCK:
            srgb = gpufmt::Format::ASTC_10x6_SRGB_BLOCK;
            break;
        case gpufmt::Format::ASTC_10x8_UNORM_BLOCK:
            srgb = gpufmt::Format::ASTC_10x8_SRGB_BLOCK;
            break;
        case gpufmt::Format::ASTC_10x8_SRGB_BLOCK:
            srgb = gpufmt::Format::ASTC_10x8_SRGB_BLOCK;
            break;
        case gpufmt::Format::ASTC_10x10_UNORM_BLOCK:
            srgb = gpufmt::Format::ASTC_10x10_SRGB_BLOCK;
            break;
        case gpufmt::Format::ASTC_10x10_SRGB_BLOCK:
            srgb = gpufmt::Format::ASTC_10x10_SRGB_BLOCK;
            break;
        case gpufmt::Format::ASTC_12x10_UNORM_BLOCK:
            srgb = gpufmt::Format::ASTC_12x10_SRGB_BLOCK;
            break;
        case gpufmt::Format::ASTC_12x10_SRGB_BLOCK:
            srgb = gpufmt::Format::ASTC_12x10_SRGB_BLOCK;
            break;
        case gpufmt::Format::ASTC_12x12_UNORM_BLOCK:
            srgb = gpufmt::Format::ASTC_12x12_SRGB_BLOCK;
            break;
        case gpufmt::Format::ASTC_12x12_SRGB_BLOCK:
            srgb = gpufmt::Format::ASTC_12x12_SRGB_BLOCK;
            break;
#endif //GF_EXCLUDE_ASTC_COMPRESSED_FORMATS
#ifndef GF_EXCLUDE_PVRTC_COMPRESSED_FORMATS
        case gpufmt::Format::PVRTC1_2BPP_UNORM_BLOCK_IMG:
            srgb = gpufmt::Format::PVRTC1_2BPP_SRGB_BLOCK_IMG;
            break;
        case gpufmt::Format::PVRTC1_4BPP_UNORM_BLOCK_IMG:
            srgb = gpufmt::Format::PVRTC1_4BPP_SRGB_BLOCK_IMG;
            break;
        case gpufmt::Format::PVRTC2_2BPP_UNORM_BLOCK_IMG:
            srgb = gpufmt::Format::PVRTC2_2BPP_SRGB_BLOCK_IMG;
            break;
        case gpufmt::Format::PVRTC2_4BPP_UNORM_BLOCK_IMG:
            srgb = gpufmt::Format::PVRTC2_4BPP_SRGB_BLOCK_IMG;
            break;
        case gpufmt::Format::PVRTC1_2BPP_SRGB_BLOCK_IMG:
            srgb = gpufmt::Format::PVRTC1_2BPP_SRGB_BLOCK_IMG;
            break;
        case gpufmt::Format::PVRTC1_4BPP_SRGB_BLOCK_IMG:
            srgb = gpufmt::Format::PVRTC1_4BPP_SRGB_BLOCK_IMG;
            break;
        case gpufmt::Format::PVRTC2_2BPP_SRGB_BLOCK_IMG:
            srgb = gpufmt::Format::PVRTC2_2BPP_SRGB_BLOCK_IMG;
            break;
        case gpufmt::Format::PVRTC2_4BPP_SRGB_BLOCK_IMG:
            srgb = gpufmt::Format::PVRTC2_4BPP_SRGB_BLOCK_IMG;
            break;
#endif //GF_EXCLUDE_PVRTC_COMPRESSED_FORMATS
#endif //GF_EXCLUDE_COMPRESSED_FORMATS
        default:
            break;
        }

        return srgb;
    }
}