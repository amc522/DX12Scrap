#pragma once

#include <gpufmt/config.h>
#include <gpufmt/definitions.h>

#if !defined(GF_EXCLUDE_COMPRESSED_FORMATS) && !defined(GF_EXCLUDE_ETC_COMPRESSED_FORMATS)

#include <glm/fwd.hpp>

#include <cstdint>

namespace gpufmt::internal {
    void decompressETC2_RGB(uint64_t block, gpufmt::span<glm::vec3, 16> texels) noexcept;
    void decompressETC2_RGB(uint64_t block, gpufmt::span<glm::vec4, 16> texels) noexcept;
    void decompressETC2_RGB(const gpufmt::Surface<const uint64_t> &blocks, const gpufmt::Surface<glm::u8vec3> &texels) noexcept;
    void decompressETC2_RGB(const gpufmt::Surface<const uint64_t> &blocks, const gpufmt::Surface<glm::u8vec4> &texels) noexcept;

    void decompressETC2_RGB8A1(uint64_t block, gpufmt::span<glm::vec4, 16> texels) noexcept;
    void decompressETC2_RGB8A1(const gpufmt::Surface<const uint64_t> &blocks, const gpufmt::Surface<glm::u8vec4> &texels) noexcept;

    void decompressETC2_RGBA(const glm::u64vec2 &block, gpufmt::span<glm::vec4, 16> texels) noexcept;
    void decompressETC2_RGBA(const gpufmt::Surface<const glm::u64vec2> &blocks, const gpufmt::Surface<glm::u8vec4> &texels) noexcept;

    void decompressEAC_R_UNorm(uint64_t block, gpufmt::span<float, 16> texels) noexcept;
    void decompressEAC_R_UNorm(const gpufmt::Surface<const uint64_t> &blocks, const gpufmt::Surface<uint16_t> &texels) noexcept;

    void decompressEAC_R_SNorm(uint64_t block, gpufmt::span<float, 16> texels) noexcept;
    void decompressEAC_R_SNorm(const gpufmt::Surface<const uint64_t> &blocks, const gpufmt::Surface<int16_t> &texels) noexcept;

    void decompressEAC_RG_UNorm(const glm::u64vec2 &block, gpufmt::span<glm::vec2, 16> texels) noexcept;
    void decompressEAC_RG_UNorm(const gpufmt::Surface<const glm::u64vec2> &blocks, const gpufmt::Surface<glm::u16vec2> &texels) noexcept;

    void decompressEAC_RG_SNorm(const glm::u64vec2 &block, gpufmt::span<glm::vec2, 16> texels) noexcept;
    void decompressEAC_RG_SNorm(const gpufmt::Surface<const glm::u64vec2> &blocks, const gpufmt::Surface<glm::i16vec2> &texels) noexcept;
}

#endif