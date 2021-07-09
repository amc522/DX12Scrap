#pragma once

#include <gpufmt/config.h>

#if !defined(GF_EXCLUDE_COMPRESSED_FORMATS) && !defined(GF_EXCLUDE_BC_COMPRESSED_FORMATS)

#include <glm/fwd.hpp>
#include <array>

namespace gpufmt {
    struct BC1Block {
        uint16_t color0;
        uint16_t color1;
        std::array<uint8_t, 4> row;
    };

    struct BC2Block {
        std::array<uint16_t, 4> alphaRow;
        uint16_t color0;
        uint16_t color1;
        std::array<uint8_t, 4> row;
    };

    struct BC3Block {
        std::array<uint8_t, 2> alpha;
        std::array<uint8_t, 6> alphaBitmap;
        uint16_t color0;
        uint16_t color1;
        std::array<uint8_t, 4> row;
    };

    struct BC4Block {
        uint8_t red0;
        uint8_t red1;
        std::array<uint8_t, 6> bitmap;
    };

    struct BC5Block {
        uint8_t red0;
        uint8_t red1;
        std::array<uint8_t, 6> redBitmap;
        uint8_t green0;
        uint8_t green1;
        std::array<uint8_t, 6> greenBitmap;
    };

    using BC6hBlock = glm::u64vec2;
    using BC7Block = glm::u64vec2;

    namespace internal {
        void decompressBC1Block_RGB(const BC1Block &block, gpufmt::span<glm::vec3, 16> texels) noexcept;

        void decompressBC1Block_RGBA(const BC1Block &block, gpufmt::span<glm::vec4, 16> texels) noexcept;

        void decompressBC2Block(const BC2Block &block, gpufmt::span<glm::vec4, 16> texels) noexcept;

        void decompressBC3Block(const BC3Block &block, gpufmt::span<glm::vec4, 16> texels) noexcept;

        void decompressBC4Block_UNorm1(const BC4Block &block, gpufmt::span<float, 16> texels) noexcept;

        void decompressBC4Block_UNorm4(const BC4Block &block, gpufmt::span<glm::vec4, 16> texels) noexcept;

        void decompressBC4Block_SNorm1(const BC4Block &block, gpufmt::span<float, 16> texels) noexcept;

        void decompressBC4Block_SNorm4(const BC4Block &block, gpufmt::span<glm::vec4, 16> texels) noexcept;

        void decompressBC5Block_UNorm2(const BC5Block &block, gpufmt::span<glm::vec2, 16> texels) noexcept;

        void decompressBC5Block_UNorm4(const BC5Block &block, gpufmt::span<glm::vec4, 16> texels) noexcept;

        void decompressBC5Block_SNorm2(const BC5Block &block, gpufmt::span<glm::vec2, 16> texels) noexcept;

        void decompressBC5Block_SNorm4(const BC5Block &block, gpufmt::span<glm::vec4, 16> texels) noexcept;

        void decompressBC6hBlock_UFloat3(const BC6hBlock &block, gpufmt::span<glm::vec3, 16> texels) noexcept;

        void decompressBC6hBlock_SFloat3(const BC6hBlock &block, gpufmt::span<glm::vec3, 16> texels) noexcept;

        void decompressBC6hBlock_UFloat3(const BC6hBlock &block, gpufmt::span<glm::u16vec3, 16> texels) noexcept;

        void decompressBC6hBlock_SFloat3(const BC6hBlock &block, gpufmt::span<glm::u16vec3, 16> texels) noexcept;

        void decompressBC6hBlock_UFloat4(const BC6hBlock &block, gpufmt::span<glm::vec4, 16> texels) noexcept;

        void decompressBC6hBlock_SFloat4(const BC6hBlock &block, gpufmt::span<glm::vec4, 16> texels) noexcept;

        void decompressBC6hBlock_UFloat4(const BC6hBlock &block, gpufmt::span<glm::u16vec4, 16> texels) noexcept;

        void decompressBC6hBlock_SFloat4(const BC6hBlock &block, gpufmt::span<glm::u16vec4, 16> texels) noexcept;

        void decompressBC7Block(const BC7Block &block, gpufmt::span<glm::vec4, 16> texels) noexcept;

        void decompressBC7Block(const BC7Block &block, gpufmt::span<glm::u8vec4, 16> texels) noexcept;
    }
}

#endif