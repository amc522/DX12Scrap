#include "gpufmt/internal/bc.h"

#if !defined(GF_EXCLUDE_COMPRESSED_FORMATS) && !defined(GF_EXCLUDE_BC_COMPRESSED_FORMATS)

#include "gpufmt/definitions.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/packing.hpp>
#include <glm/gtc/packing.hpp>
#include <DirectXTex/BC.h>

#include <array>

namespace gpufmt::internal {
    [[nodiscard]]
    constexpr void createSingleChannelLUT(bool interpolate6, float min, std::array<float, 8> &lut) noexcept {
        if(interpolate6) {
            lut[2] = (6.0f / 7.0f) * lut[0] + (1.0f / 7.0f) * lut[1];
            lut[3] = (5.0f / 7.0f) * lut[0] + (2.0f / 7.0f) * lut[1];
            lut[4] = (4.0f / 7.0f) * lut[0] + (3.0f / 7.0f) * lut[1];
            lut[5] = (3.0f / 7.0f) * lut[0] + (4.0f / 7.0f) * lut[1];
            lut[6] = (2.0f / 7.0f) * lut[0] + (5.0f / 7.0f) * lut[1];
            lut[7] = (1.0f / 7.0f) * lut[0] + (6.0f / 7.0f) * lut[1];
        } else {
            lut[2] = (4.0f / 5.0f) * lut[0] + (1.0f / 5.0f) * lut[1];
            lut[3] = (3.0f / 5.0f) * lut[0] + (2.0f / 5.0f) * lut[1];
            lut[4] = (2.0f / 5.0f) * lut[0] + (3.0f / 5.0f) * lut[1];
            lut[5] = (1.0f / 5.0f) * lut[0] + (4.0f / 5.0f) * lut[1];
            lut[6] = min;
            lut[7] = 1.0f;
        }
    }

    [[nodiscard]]
    constexpr void singleChannelBitmapDataUNorm(uint8_t channel0, uint8_t channel1, const std::array<uint8_t, 6> &channelBitmap, std::array<float, 8> &lut, uint32_t &contiguousBitmap) noexcept {
        lut[0] = channel0 / 255.0f;
        lut[1] = channel1 / 255.0f;

        createSingleChannelLUT(channel0 > channel1, 0.0f, lut);

        contiguousBitmap = static_cast<uint32_t>(channelBitmap[0]) | (static_cast<uint32_t>(channelBitmap[1]) << 8) | (static_cast<uint32_t>(channelBitmap[2]) << 16);
        contiguousBitmap |= (static_cast<uint32_t>(channelBitmap[3]) | (static_cast<uint32_t>(channelBitmap[4]) << 8) | (static_cast<uint32_t>(channelBitmap[5]) << 16)) << 24;
    }

    [[nodiscard]]
    constexpr void singleChannelBitmapDataSNorm(uint8_t channel0, uint8_t channel1, const std::array<uint8_t, 6> &channelBitmap, std::array<float, 8> &lut, uint32_t &contiguousBitmap) noexcept {
        lut[0] = (channel0 / 255.0f) * 2.0f - 1.0f;
        lut[1] = (channel1 / 255.0f) * 2.0f - 1.0f;

        createSingleChannelLUT(channel0 > channel1, -1.0f, lut);

        contiguousBitmap = static_cast<uint32_t>(channelBitmap[0]) | (static_cast<uint32_t>(channelBitmap[1]) << 8) | (static_cast<uint32_t>(channelBitmap[2]) << 16);
        contiguousBitmap |= (static_cast<uint32_t>(channelBitmap[3]) | (static_cast<uint32_t>(channelBitmap[4]) << 8) | (static_cast<uint32_t>(channelBitmap[5]) << 16)) << 24;
    }

    [[nodiscard]]
    constexpr void singleChannelBitmapDataSNorm(uint8_t channel0, uint32_t channel1, bool interpolate6, const std::array<uint8_t, 6> &channelBitmap, std::array<float, 8> &lut, uint32_t &contiguousBitmap) noexcept {
        lut[0] = (channel0 / 255.0f) * 2.0f - 1.0f;
        lut[1] = (channel1 / 255.0f) * 2.0f - 1.0f;

        createSingleChannelLUT(interpolate6, -1.0f, lut);

        contiguousBitmap = static_cast<uint32_t>(channelBitmap[0]) | (static_cast<uint32_t>(channelBitmap[1]) << 8) | (static_cast<uint32_t>(channelBitmap[2]) << 16);
        contiguousBitmap |= (static_cast<uint32_t>(channelBitmap[3]) | (static_cast<uint32_t>(channelBitmap[4]) << 8) | (static_cast<uint32_t>(channelBitmap[5]) << 16)) << 24;
    }

#if defined(GF_COMPILER_MSVC)
#pragma warning(push)
#pragma warning(disable : 4701) //potentially uninitialized local variable 'texels' used
#endif

    void decompressBC1Block_RGB(const BC1Block &block, gpufmt::span<glm::vec3, 16> texels) noexcept {
        std::array<glm::vec3, 4> color;

        color[0] = glm::vec3(glm::unpackUnorm1x5_1x6_1x5(block.color0));
        std::swap(color[0].r, color[0].b);
        color[1] = glm::vec3(glm::unpackUnorm1x5_1x6_1x5(block.color1));
        std::swap(color[1].r, color[1].b);

        if(block.color0 > block.color1) {
            color[2] = (2.0f / 3.0f) * color[0] + (1.0f / 3.0f) * color[1];
            color[3] = (1.0f / 3.0f) * color[0] + (2.0f / 3.0f) * color[1];
        }

        for(size_t row = 0; row < 4; ++row) {
            for(size_t column = 0; column < 4; ++column) {
                const auto colorIndex = (block.row[row] >> (column * 2u)) & 0x3u;
                texels[row * 4u + column] = color[colorIndex];
            }
        }
    }

    void decompressBC1Block_RGBA(const BC1Block &block, gpufmt::span<glm::vec4, 16> texels) noexcept {
        std::array<glm::vec4, 4> color;

        color[0] = glm::vec4(glm::unpackUnorm1x5_1x6_1x5(block.color0), 1.0f);
        std::swap(color[0].r, color[0].b);
        color[1] = glm::vec4(glm::unpackUnorm1x5_1x6_1x5(block.color1), 1.0f);
        std::swap(color[1].r, color[1].b);

        if(block.color0 > block.color1) {
            color[2] = (2.0f / 3.0f) * color[0] + (1.0f / 3.0f) * color[1];
            color[3] = (1.0f / 3.0f) * color[0] + (2.0f / 3.0f) * color[1];
        } else {
            color[2] = (color[0] + color[1]) / 2.0f;
            color[3] = glm::vec4(0.0f);
        }

        for(size_t row = 0; row < 4; ++row) {
            for(size_t column = 0; column < 4; ++column) {
                const auto colorIndex = (block.row[row] >> (column * 2u)) & 0x3u;
                texels[row * 4u + column] = color[colorIndex];
            }
        }
    }

    void decompressBC2Block(const BC2Block &block, gpufmt::span<glm::vec4, 16> texels) noexcept {
        std::array<glm::vec3, 4> color;

        color[0] = glm::vec3(glm::unpackUnorm1x5_1x6_1x5(block.color0));
        std::swap(color[0].r, color[0].b);
        color[1] = glm::vec3(glm::unpackUnorm1x5_1x6_1x5(block.color1));
        std::swap(color[1].r, color[1].b);

        color[2] = (2.0f / 3.0f) * color[0] + (1.0f / 3.0f) * color[1];
        color[3] = (1.0f / 3.0f) * color[0] + (2.0f / 3.0f) * color[1];

        for(size_t row = 0; row < 4; ++row) {
            for(size_t column = 0; column < 4; ++column) {
                const auto colorIndex = (block.row[row] >> (column * 2u)) & 0x3u;
                const float alpha = ((block.alphaRow[row] >> (column * 4)) & 0xFu) / 15.0f;
                texels[row * 4u + column] = glm::vec4(color[colorIndex], alpha);
            }
        }
    }

    void decompressBC3Block(const BC3Block &block, gpufmt::span<glm::vec4, 16> texels) noexcept {
        std::array<glm::vec3, 4> color;
        std::array<float, 8> alpha;

        color[0] = glm::vec3(glm::unpackUnorm1x5_1x6_1x5(block.color0));
        std::swap(color[0].r, color[0].b);
        color[1] = glm::vec3(glm::unpackUnorm1x5_1x6_1x5(block.color1));
        std::swap(color[1].r, color[1].b);

        color[2] = (2.0f / 3.0f) * color[0] + (1.0f / 3.0f) * color[1];
        color[3] = (1.0f / 3.0f) * color[0] + (2.0f / 3.0f) * color[1];

        alpha[0] = block.alpha[0] / 255.0f;
        alpha[1] = block.alpha[1] / 255.0f;

        if(alpha[0] > alpha[1]) {
            alpha[2] = (6.0f / 7.0f) * alpha[0] + (1.0f / 7.0f) * alpha[1];
            alpha[3] = (5.0f / 7.0f) * alpha[0] + (2.0f / 7.0f) * alpha[1];
            alpha[4] = (4.0f / 7.0f) * alpha[0] + (3.0f / 7.0f) * alpha[1];
            alpha[5] = (3.0f / 7.0f) * alpha[0] + (4.0f / 7.0f) * alpha[1];
            alpha[6] = (2.0f / 7.0f) * alpha[0] + (5.0f / 7.0f) * alpha[1];
            alpha[7] = (1.0f / 7.0f) * alpha[0] + (6.0f / 7.0f) * alpha[1];
        } else {
            alpha[2] = (4.0f / 5.0f) * alpha[0] + (1.0f / 5.0f) * alpha[1];
            alpha[3] = (3.0f / 5.0f) * alpha[0] + (2.0f / 5.0f) * alpha[1];
            alpha[4] = (2.0f / 5.0f) * alpha[0] + (3.0f / 5.0f) * alpha[1];
            alpha[5] = (1.0f / 5.0f) * alpha[0] + (4.0f / 5.0f) * alpha[1];
            alpha[6] = 0.0f;
            alpha[7] = 1.0f;
        }

        uint32_t bitmap;
        bitmap = static_cast<uint32_t>(block.alphaBitmap[0]) | (static_cast<uint32_t>(block.alphaBitmap[1]) << 8) | (static_cast<uint32_t>(block.alphaBitmap[2]) << 16);
        bitmap |= (static_cast<uint32_t>(block.alphaBitmap[3]) | (static_cast<uint32_t>(block.alphaBitmap[4]) << 8) | (static_cast<uint32_t>(block.alphaBitmap[5]) << 16)) << 24;

        for(uint8_t row = 0; row < 4; ++row) {
            for(uint8_t column = 0; column < 4; ++column) {
                const uint8_t texelIndex = row * 4u + column;
                const auto colorIndex = (block.row[row] >> (column * 2u)) & 0x3u;
                const auto alphaIndex = (bitmap >> (texelIndex * 3u)) & 0x7u;
                texels[texelIndex] = glm::vec4(color[colorIndex], alpha[alphaIndex]);
            }
        }
    }

    void decompressBC4Block_UNorm1(const BC4Block &block, gpufmt::span<float, 16> texels) noexcept {
        std::array<float, 8> redLUT;
        uint32_t bitmap;

        singleChannelBitmapDataUNorm(block.red0, block.red1, block.bitmap, redLUT, bitmap);

        for(uint8_t row = 0; row < 4; ++row) {
            for(uint8_t column = 0; column < 4; ++column) {
                const uint8_t texelIndex = row * 4u + column;
                const auto redIndex = (bitmap >> (texelIndex * 3u)) & 0x7u;
                texels[texelIndex] = redLUT[redIndex];
            }
        }
    }

    void decompressBC4Block_UNorm4(const BC4Block &block, gpufmt::span<glm::vec4, 16> texels) noexcept {
        std::array<float, 8> redLUT;
        uint32_t bitmap;

        singleChannelBitmapDataUNorm(block.red0, block.red1, block.bitmap, redLUT, bitmap);

        for(uint8_t row = 0; row < 4; ++row) {
            for(uint8_t column = 0; column < 4; ++column) {
                const uint8_t texelIndex = row * 4u + column;
                const auto redIndex = (bitmap >> (texelIndex * 3u)) & 0x7u;
                texels[texelIndex] = glm::vec4(redLUT[redIndex], 0.0f, 0.0f, 1.0f);
            }
        }
    }

    void decompressBC4Block_SNorm1(const BC4Block &block, gpufmt::span<float, 16> texels) noexcept {
        std::array<float, 8> redLUT;
        uint32_t bitmap;

        singleChannelBitmapDataSNorm(block.red0, block.red1, block.bitmap, redLUT, bitmap);

        for(uint8_t row = 0; row < 4; ++row) {
            for(uint8_t column = 0; column < 4; ++column) {
                const uint8_t texelIndex = row * 4u + column;
                const auto redIndex = (bitmap >> (texelIndex * 3u)) & 0x7u;
                texels[texelIndex] = redLUT[redIndex];
            }
        }
    }

    void decompressBC4Block_SNorm4(const BC4Block &block, gpufmt::span<glm::vec4, 16> texels) noexcept {
        std::array<float, 8> redLUT;
        uint32_t bitmap;

        singleChannelBitmapDataSNorm(block.red0, block.red1, block.bitmap, redLUT, bitmap);

        for(uint8_t row = 0; row < 4; ++row) {
            for(uint8_t column = 0; column < 4; ++column) {
                const uint8_t texelIndex = row * 4u + column;
                const auto redIndex = (bitmap >> (texelIndex * 3u)) & 0x7u;
                texels[texelIndex] = glm::vec4(redLUT[redIndex], 0.0f, 0.0f, 1.0f);
            }
        }
    }

    void decompressBC5Block_UNorm2(const BC5Block &block, gpufmt::span<glm::vec2, 16> texels) noexcept {
        std::array<float, 8> redLUT;
        uint32_t redBitmap;

        singleChannelBitmapDataUNorm(block.red0, block.red1, block.redBitmap, redLUT, redBitmap);

        std::array<float, 8> greenLUT;
        uint32_t greenBitmap;

        singleChannelBitmapDataUNorm(block.green0, block.green1, block.greenBitmap, greenLUT, greenBitmap);

        for(uint8_t row = 0; row < 4; ++row) {
            for(uint8_t column = 0; column < 4; ++column) {
                const uint8_t texelIndex = row * 4u + column;
                const auto redIndex = (redBitmap >> (texelIndex * 3u)) & 0x7u;
                const auto greenIndex = (greenBitmap >> (texelIndex * 3u)) & 0x7u;
                texels[texelIndex] = glm::vec2(redLUT[redIndex], greenLUT[greenIndex]);
            }
        }
    }

    void decompressBC5Block_UNorm4(const BC5Block &block, gpufmt::span<glm::vec4, 16> texels) noexcept {
        std::array<float, 8> redLUT;
        uint32_t redBitmap;

        singleChannelBitmapDataUNorm(block.red0, block.red1, block.redBitmap, redLUT, redBitmap);

        std::array<float, 8> greenLUT;
        uint32_t greenBitmap;

        singleChannelBitmapDataUNorm(block.green0, block.green1, block.greenBitmap, greenLUT, greenBitmap);

        for(uint8_t row = 0; row < 4; ++row) {
            for(uint8_t column = 0; column < 4; ++column) {
                const uint8_t texelIndex = row * 4u + column;
                const auto redIndex = (redBitmap >> (texelIndex * 3u)) & 0x7u;
                const auto greenIndex = (greenBitmap >> (texelIndex * 3u)) & 0x7u;
                texels[texelIndex] = glm::vec4(redLUT[redIndex], greenLUT[greenIndex], 0.0f, 1.0f);
            }
        }
    }

    void decompressBC5Block_SNorm2(const BC5Block &block, gpufmt::span<glm::vec2, 16> texels) noexcept {
        std::array<float, 8> redLUT;
        uint32_t redBitmap;

        singleChannelBitmapDataSNorm(block.red0, block.red1, block.redBitmap, redLUT, redBitmap);

        std::array<float, 8> greenLUT;
        uint32_t greenBitmap;

        singleChannelBitmapDataSNorm(block.green0, block.green1, block.red0 > block.red1, block.greenBitmap, greenLUT, greenBitmap);

        for(uint8_t row = 0; row < 4; ++row) {
            for(uint8_t column = 0; column < 4; ++column) {
                const uint8_t texelIndex = row * 4u + column;
                const auto redIndex = (redBitmap >> (texelIndex * 3u)) & 0x7u;
                const auto greenIndex = (greenBitmap >> (texelIndex * 3u)) & 0x7u;
                texels[texelIndex] = glm::vec2(redLUT[redIndex], greenLUT[greenIndex]);
            }
        }
    }

    void decompressBC5Block_SNorm4(const BC5Block &block, gpufmt::span<glm::vec4, 16> texels) noexcept {
        std::array<float, 8> redLUT;
        uint32_t redBitmap;

        singleChannelBitmapDataSNorm(block.red0, block.red1, block.redBitmap, redLUT, redBitmap);

        std::array<float, 8> greenLUT;
        uint32_t greenBitmap;

        singleChannelBitmapDataSNorm(block.green0, block.green1, block.red0 > block.red1, block.greenBitmap, greenLUT, greenBitmap);

        for(uint8_t row = 0; row < 4; ++row) {
            for(uint8_t column = 0; column < 4; ++column) {
                const uint8_t texelIndex = row * 4u + column;
                const auto redIndex = (redBitmap >> (texelIndex * 3u)) & 0x7u;
                const auto greenIndex = (greenBitmap >> (texelIndex * 3u)) & 0x7u;
                texels[texelIndex] = glm::vec4(redLUT[redIndex], greenLUT[greenIndex], 0.0f, 1.0f);
            }
        }
    }

    void decompressBC6hBlock_UFloat3(const BC6hBlock &block, gpufmt::span<glm::vec3, 16> texels) noexcept {
        DirectX::D3DXDecodeBC6HU(reinterpret_cast<DirectX::HDRColor*>(texels.data()), reinterpret_cast<const uint8_t*>(&block));
    }

    void decompressBC6hBlock_SFloat3(const BC6hBlock &block, gpufmt::span<glm::vec3, 16> texels) noexcept {
        DirectX::D3DXDecodeBC6HS(reinterpret_cast<DirectX::HDRColor*>(texels.data()), reinterpret_cast<const uint8_t*>(&block));
    }

    void decompressBC6hBlock_UFloat3(const BC6hBlock &block, gpufmt::span<glm::u16vec3, 16> texels) noexcept {
        DirectX::D3DXDecodeBC6HU(reinterpret_cast<DirectX::PackedVector::XMHALF4*>(texels.data()), reinterpret_cast<const uint8_t*>(&block));
    }

    void decompressBC6hBlock_SFloat3(const BC6hBlock &block, gpufmt::span<glm::u16vec3, 16> texels) noexcept {
        DirectX::D3DXDecodeBC6HS(reinterpret_cast<DirectX::PackedVector::XMHALF4*>(texels.data()), reinterpret_cast<const uint8_t*>(&block));
    }

    void decompressBC6hBlock_UFloat4(const BC6hBlock &block, gpufmt::span<glm::vec4, 16> texels) noexcept {
        DirectX::D3DXDecodeBC6HU(reinterpret_cast<DirectX::XMVECTOR*>(texels.data()), reinterpret_cast<const uint8_t*>(&block));
    }

    void decompressBC6hBlock_SFloat4(const BC6hBlock &block, gpufmt::span<glm::vec4, 16> texels) noexcept {
        DirectX::D3DXDecodeBC6HS(reinterpret_cast<DirectX::XMVECTOR*>(texels.data()), reinterpret_cast<const uint8_t*>(&block));
    }

    void decompressBC6hBlock_UFloat4(const BC6hBlock &block, gpufmt::span<glm::u16vec4, 16> texels) noexcept {
        DirectX::D3DXDecodeBC6HU(reinterpret_cast<DirectX::PackedVector::XMHALF4*>(texels.data()), reinterpret_cast<const uint8_t*>(&block));
    }

    void decompressBC6hBlock_SFloat4(const BC6hBlock &block, gpufmt::span<glm::u16vec4, 16> texels) noexcept {
        DirectX::D3DXDecodeBC6HS(reinterpret_cast<DirectX::PackedVector::XMHALF4*>(texels.data()), reinterpret_cast<const uint8_t*>(&block));
    }

    void decompressBC7Block(const BC7Block &block, gpufmt::span<glm::vec4, 16> texels) noexcept {
        DirectX::D3DXDecodeBC7(reinterpret_cast<DirectX::XMVECTOR*>(texels.data()), reinterpret_cast<const uint8_t*>(&block));
    }

    void decompressBC7Block(const BC7Block &block, gpufmt::span<glm::u8vec4, 16> texels) noexcept {
        DirectX::D3DXDecodeBC7(reinterpret_cast<DirectX::XMVECTOR *>(texels.data()), reinterpret_cast<const uint8_t *>(&block));
    }

#if defined(GF_COMPILER_MSVC)
#pragma warning(pop)
#endif
}

#endif