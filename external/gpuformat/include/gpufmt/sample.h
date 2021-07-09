#pragma once

#include <gpufmt/config.h>
#include <gpufmt/definitions.h>
#include <gpufmt/format.h>
#include <gpufmt/internal/sample_variant.h>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <memory>
#include <vector>

namespace gpufmt {
    namespace internal {
        template<class FromT, class ToT>
        [[nodiscard]]
        inline static constexpr glm::vec<4, ToT> sampleCast(const glm::vec<4, FromT> &from) noexcept {
            return static_cast<glm::vec<4, ToT>>(from);
        }

        [[nodiscard]]
        inline static float linearTosRGB(float linear) noexcept {
            linear = std::clamp(linear, 0.0f, 1.0f);
            return (linear <= 0.0031308) ?
                linear * 12.92f :
                1.055f * std::pow(linear, 1.0f / 2.4f) - 0.055f;
        }

        [[nodiscard]]
        inline static float sRGBToLinear(float sRGB) noexcept {
            sRGB = std::clamp(sRGB, 0.0f, 1.0f);
            return (sRGB <= 0.04045f) ?
                sRGB / 12.92f :
                std::pow((sRGB + 0.055f) / 1.055f, 2.4f);
        }

        [[nodiscard]]
        inline static glm::vec3 linearTosRGB(const glm::vec3 &linear) noexcept {
            return glm::vec3(linearTosRGB(linear.r),
                             linearTosRGB(linear.g),
                             linearTosRGB(linear.b));
        }

        [[nodiscard]]
        inline static glm::vec3 sRGBToLinear(const glm::vec3 sRGB) noexcept {
            return glm::vec3(sRGBToLinear(sRGB.r),
                             sRGBToLinear(sRGB.g),
                             sRGBToLinear(sRGB.b));
        }

        class BaseBlockSampler;
    }
    
    [[nodiscard]]
    std::pair<gpufmt::SampleVariant, gpufmt::SampleVariant> sampleRange(gpufmt::Format format) noexcept;

    class BlockSampler {
    public:
        BlockSampler() noexcept;
        BlockSampler(Format format) noexcept;
        BlockSampler(const BlockSampler &) noexcept = delete;
        BlockSampler(BlockSampler &&other) noexcept;
        ~BlockSampler();

        BlockSampler &operator=(const BlockSampler &) noexcept = delete;
        BlockSampler &operator=(BlockSampler &&other) noexcept;

        [[nodiscard]]
        gpufmt::Format format() const noexcept;
        [[nodiscard]]
        const Extent &blockExtent() const noexcept;

        [[nodiscard]]
        size_t blockTexelCount() const noexcept;

        [[nodiscard]]
        std::vector<gpufmt::byte> narrowSample(const Surface<const gpufmt::byte> &surface, const gpufmt::Extent &bloxel, BlockSampleError &error) const noexcept;

        BlockSampleError narrowSampleTo(const Surface<const gpufmt::byte> &surface, const gpufmt::Extent &bloxel, gpufmt::span<gpufmt::byte> sampleBuffer) const noexcept;

        [[nodiscard]]
        std::vector<gpufmt::byte> wideSample(const Surface<const gpufmt::byte> &surface, const gpufmt::Extent &bloxel, BlockSampleError &error) const noexcept;

        [[nodiscard]]
        BlockSampleError wideSampleTo(const Surface<const gpufmt::byte> &surface, const gpufmt::Extent &bloxel, gpufmt::span<gpufmt::byte> sampleBuffer) const noexcept;

        [[nodiscard]]
        std::vector<gpufmt::SampleVariant> variantSample(const Surface<const gpufmt::byte> &surface, const gpufmt::Extent &bloxel, BlockSampleError &error) const noexcept;

        BlockSampleError variantSampleTo(const Surface<const gpufmt::byte> &surface, const gpufmt::Extent &bloxel, gpufmt::span<gpufmt::SampleVariant> samples) const noexcept;

    private:
        std::unique_ptr<gpufmt::internal::BaseBlockSampler> mSampler;
        Format mFormat = Format::UNDEFINED;
    };
}