#include "gpufmt/internal/etc.h"

#if !defined(GF_EXCLUDE_COMPRESSED_FORMATS) && !defined(GF_EXCLUDE_ETC_COMPRESSED_FORMATS)

#include "gpufmt/internal/bit_packing.h"
#include "gpufmt/internal/byte_packing.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/vec_swizzle.hpp>
#include <switftshader/ETC_Decoder.hpp>

#include <array>

namespace gpufmt::internal {
    void decompressETC2_RGB(uint64_t block, gpufmt::span<glm::vec3, 16> texels) noexcept {
        std::array<glm::u8vec4, 16> u8bgraTexels;
        ETC_Decoder::Decode(reinterpret_cast<const unsigned char*>(&block),
                            reinterpret_cast<unsigned char*>(u8bgraTexels.data()),
                            4, 4,
                            4, 4,
                            sizeof(glm::u8vec4) * 4,
                            sizeof(glm::u8vec4),
                            ETC_Decoder::ETC_RGB);

        std::transform(u8bgraTexels.cbegin(), u8bgraTexels.cend(), texels.begin(), [](glm::u8vec4 bgraTexel) {
            return glm::xyz(internal::unpackUNorm4(glm::zyxw(bgraTexel)));
        });
    }

    void decompressETC2_RGB(const gpufmt::Surface<const uint64_t> &compressedSurface, const gpufmt::Surface<glm::u8vec3> &decompressedTexels) noexcept {
        ETC_Decoder::Decode(reinterpret_cast<const unsigned char*>(compressedSurface.blockData.data()),
                            reinterpret_cast<unsigned char*>(decompressedTexels.blockData.data()),
                            compressedSurface.extentInBlocks.x * 4, compressedSurface.extentInBlocks.y * 4,
                            decompressedTexels.extentInBlocks.x, decompressedTexels.extentInBlocks.y,
                            sizeof(glm::u8vec3) * decompressedTexels.extentInBlocks.x,
                            sizeof(glm::u8vec3),
                            ETC_Decoder::ETC_RGB);
    }

    void decompressETC2_RGB(const gpufmt::Surface<const uint64_t> &compressedBlocks, const gpufmt::Surface<glm::u8vec4> &decompressedTexels) noexcept {
        ETC_Decoder::Decode(reinterpret_cast<const unsigned char*>(compressedBlocks.blockData.data()),
                            reinterpret_cast<unsigned char*>(decompressedTexels.blockData.data()),
                            compressedBlocks.extentInBlocks.x * 4, compressedBlocks.extentInBlocks.y * 4,
                            decompressedTexels.extentInBlocks.x, decompressedTexels.extentInBlocks.y,
                            sizeof(glm::u8vec4) * decompressedTexels.extentInBlocks.x,
                            sizeof(glm::u8vec4),
                            ETC_Decoder::ETC_RGB);
    }

    void decompressETC2_RGB8A1(uint64_t block, gpufmt::span<glm::vec4, 16> texels) noexcept {
        std::array<glm::u8vec4, 16> u8bgraTexels;
        ETC_Decoder::Decode(reinterpret_cast<const unsigned char*>(&block),
                            reinterpret_cast<unsigned char*>(u8bgraTexels.data()),
                            4, 4,
                            4, 4,
                            sizeof(glm::u8vec4) * 4,
                            sizeof(glm::u8vec4),
                            ETC_Decoder::ETC_RGB_PUNCHTHROUGH_ALPHA);

        std::transform(u8bgraTexels.cbegin(), u8bgraTexels.cend(), texels.begin(), [](glm::u8vec4 bgraTexel) {
            return internal::unpackUNorm4(glm::zyxw(bgraTexel));
        });
    }

    void decompressETC2_RGB8A1(const gpufmt::Surface<const uint64_t> &compressedBlocks, const gpufmt::Surface<glm::u8vec4> &decompressedTexels) noexcept {
        ETC_Decoder::Decode(reinterpret_cast<const unsigned char*>(compressedBlocks.blockData.data()),
                            reinterpret_cast<unsigned char*>(decompressedTexels.blockData.data()),
                            compressedBlocks.extentInBlocks.x * 4, compressedBlocks.extentInBlocks.y * 4,
                            decompressedTexels.extentInBlocks.x, decompressedTexels.extentInBlocks.y,
                            sizeof(glm::u8vec4) * decompressedTexels.extentInBlocks.x,
                            sizeof(glm::u8vec4),
                            ETC_Decoder::ETC_RGB_PUNCHTHROUGH_ALPHA);
    }

    void decompressETC2_RGBA(const glm::u64vec2 &block, gpufmt::span<glm::vec4, 16> texels) noexcept {
        std::array<glm::u8vec4, 16> u8bgraTexels;
        ETC_Decoder::Decode(reinterpret_cast<const unsigned char*>(glm::value_ptr(block)),
                            reinterpret_cast<unsigned char*>(u8bgraTexels.data()),
                            4, 4,
                            4, 4,
                            sizeof(glm::u8vec4) * 4,
                            sizeof(glm::u8vec4),
                            ETC_Decoder::ETC_RGBA);

        std::transform(u8bgraTexels.cbegin(), u8bgraTexels.cend(), texels.begin(), [](glm::u8vec4 bgraTexel) {
            return internal::unpackUNorm4(glm::zyxw(bgraTexel));
        });
    }

    void decompressETC2_RGBA(const gpufmt::Surface<const glm::u64vec2> &compressedBlocks, const gpufmt::Surface<glm::u8vec4> &decompressedTexels) noexcept {
        ETC_Decoder::Decode(reinterpret_cast<const unsigned char*>(compressedBlocks.blockData.data()),
                            reinterpret_cast<unsigned char*>(decompressedTexels.blockData.data()),
                            compressedBlocks.extentInBlocks.x * 4, compressedBlocks.extentInBlocks.y * 4,
                            decompressedTexels.extentInBlocks.x, decompressedTexels.extentInBlocks.y,
                            sizeof(glm::u8vec4) * decompressedTexels.extentInBlocks.x,
                            sizeof(glm::u8vec4),
                            ETC_Decoder::ETC_RGBA);
    }

    void decompressEAC_R_UNorm(uint64_t block, gpufmt::span<float, 16> texels) noexcept {
        std::array<uint32_t, 16> unpackedTexels;
        ETC_Decoder::Decode(reinterpret_cast<const unsigned char*>(&block),
                            reinterpret_cast<unsigned char*>(unpackedTexels.data()),
                            4, 4,
                            4, 4,
                            sizeof(uint32_t) * 4,
                            sizeof(uint32_t),
                            ETC_Decoder::ETC_R_UNSIGNED);

        std::transform(unpackedTexels.cbegin(), unpackedTexels.cend(), texels.begin(), [](uint32_t unpackedTexel) {
            return internal::unpackBitsUNorm<uint32_t, 11, 0>(unpackedTexel);
        });
    }

    void decompressEAC_R_UNorm(const gpufmt::Surface<const uint64_t> &compressedBlocks, const gpufmt::Surface<uint16_t> &decompressedTexels) noexcept {
        ETC_Decoder::Decode(reinterpret_cast<const unsigned char*>(compressedBlocks.blockData.data()),
                            reinterpret_cast<unsigned char*>(decompressedTexels.blockData.data()),
                            compressedBlocks.extentInBlocks.x * 4, compressedBlocks.extentInBlocks.y * 4,
                            decompressedTexels.extentInBlocks.x, decompressedTexels.extentInBlocks.y,
                            sizeof(uint16_t) * decompressedTexels.extentInBlocks.x,
                            sizeof(uint16_t),
                            ETC_Decoder::ETC_R_UNSIGNED);
    }

    void decompressEAC_R_SNorm(uint64_t block, gpufmt::span<float, 16> texels) noexcept {
        std::array<uint32_t, 16> unpackedTexels;
        ETC_Decoder::Decode(reinterpret_cast<const unsigned char*>(&block),
                            reinterpret_cast<unsigned char*>(unpackedTexels.data()),
                            4, 4,
                            4, 4,
                            sizeof(uint32_t) * 4,
                            sizeof(uint32_t),
                            ETC_Decoder::ETC_R_SIGNED);

        std::transform(unpackedTexels.cbegin(), unpackedTexels.cend(), texels.begin(), [](uint32_t unpackedTexel) {
            return internal::unpackBitsSNorm<uint32_t, 11, 0>(unpackedTexel);
        });
    }

    void decompressEAC_R_SNorm(const gpufmt::Surface<const uint64_t> &compressedBlocks, const gpufmt::Surface<int16_t> &decompressedTexels) noexcept {
        ETC_Decoder::Decode(reinterpret_cast<const unsigned char*>(compressedBlocks.blockData.data()),
                            reinterpret_cast<unsigned char*>(decompressedTexels.blockData.data()),
                            compressedBlocks.extentInBlocks.x * 4, compressedBlocks.extentInBlocks.y * 4,
                            decompressedTexels.extentInBlocks.x, decompressedTexels.extentInBlocks.y,
                            sizeof(uint16_t) * decompressedTexels.extentInBlocks.x,
                            sizeof(uint16_t),
                            ETC_Decoder::ETC_R_SIGNED);
    }

    void decompressEAC_RG_UNorm(const glm::u64vec2 &block, gpufmt::span<glm::vec2, 16> texels) noexcept {
        std::array<glm::u32vec2, 16> unpackedTexels;
        ETC_Decoder::Decode(reinterpret_cast<const unsigned char*>(glm::value_ptr(block)),
                            reinterpret_cast<unsigned char*>(unpackedTexels.data()),
                            4, 4,
                            4, 4,
                            sizeof(glm::u32vec2) * 4,
                            sizeof(glm::u32vec2),
                            ETC_Decoder::ETC_RG_UNSIGNED);

        std::transform(unpackedTexels.cbegin(), unpackedTexels.cend(), texels.begin(), [](glm::u32vec2 unpackedTexel) {
            return glm::vec2(internal::unpackBitsUNorm<uint32_t, 11, 0>(unpackedTexel.r), internal::unpackBitsUNorm<uint32_t, 11, 0>(unpackedTexel.g));
        });
    }

    void decompressEAC_RG_UNorm(const gpufmt::Surface<const glm::u64vec2> &compressedBlocks, const gpufmt::Surface<glm::u16vec2> &decompressedTexels) noexcept {
        ETC_Decoder::Decode(reinterpret_cast<const unsigned char*>(compressedBlocks.blockData.data()),
                            reinterpret_cast<unsigned char*>(decompressedTexels.blockData.data()),
                            compressedBlocks.extentInBlocks.x * 4, compressedBlocks.extentInBlocks.y * 4,
                            decompressedTexels.extentInBlocks.x, decompressedTexels.extentInBlocks.y,
                            sizeof(glm::u16vec2) * decompressedTexels.extentInBlocks.x,
                            sizeof(glm::u16vec2),
                            ETC_Decoder::ETC_RG_UNSIGNED);
    }

    void decompressEAC_RG_SNorm(const glm::u64vec2 &block, gpufmt::span<glm::vec2, 16> texels) noexcept {
        std::array<glm::u32vec2, 16> unpackedTexels;
        ETC_Decoder::Decode(reinterpret_cast<const unsigned char*>(glm::value_ptr(block)),
                            reinterpret_cast<unsigned char*>(unpackedTexels.data()),
                            4, 4,
                            4, 4,
                            sizeof(glm::u32vec2) * 4,
                            sizeof(glm::u32vec2),
                            ETC_Decoder::ETC_RG_SIGNED);

        std::transform(unpackedTexels.cbegin(), unpackedTexels.cend(), texels.begin(), [](glm::u32vec2 unpackedTexel) {
            return glm::vec2(internal::unpackBitsSNorm<uint32_t, 11, 0>(unpackedTexel.r), internal::unpackBitsSNorm<uint32_t, 11, 0>(unpackedTexel.g));
        });
    }

    void decompressEAC_RG_SNorm(const gpufmt::Surface<const glm::u64vec2> &compressedBlocks, const gpufmt::Surface<glm::i16vec2> &decompressedTexels) noexcept {
        ETC_Decoder::Decode(reinterpret_cast<const unsigned char*>(compressedBlocks.blockData.data()),
                            reinterpret_cast<unsigned char*>(decompressedTexels.blockData.data()),
                            compressedBlocks.extentInBlocks.x * 4, compressedBlocks.extentInBlocks.y * 4,
                            decompressedTexels.extentInBlocks.x, decompressedTexels.extentInBlocks.y,
                            sizeof(glm::i16vec2) * decompressedTexels.extentInBlocks.x,
                            sizeof(glm::i16vec2),
                            ETC_Decoder::ETC_RG_SIGNED);
    }
}

#endif