#include <gpufmt/sample.h>
#include <gpufmt/traits.h>
#include <gpufmt/storage.h>

#include <iterator>

namespace gpufmt {
    namespace internal {
        template<gpufmt::Format FormatV>
        class SampleRangeHelper {
        public:
            using Traits = FormatTraits<FormatV>;

            [[nodiscard]]
            constexpr std::pair<gpufmt::SampleVariant, gpufmt::SampleVariant> operator()() const noexcept {
                const auto lowest = FormatLimits<FormatV>::lowest();
                const auto maximum = FormatLimits<FormatV>::max();
                
                const Traits::WideSampleType lowestVec{ lowest, lowest, lowest, lowest };
                const Traits::WideSampleType maxVec{ maximum, maximum, maximum, maximum };
                return std::make_pair(lowestVec, maxVec);
            }
        };

        template<>
        class SampleRangeHelper<Format::UNDEFINED> {
        public:
            [[nodiscard]]
            constexpr std::pair<gpufmt::SampleVariant, gpufmt::SampleVariant> operator()() const noexcept {
                return std::make_pair(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
            }
        };

        class BaseBlockSampler {
        public:
            virtual ~BaseBlockSampler() noexcept = default;

            [[nodiscard]]
            virtual const Extent &blockExtent() const noexcept = 0;

            [[nodiscard]]
            virtual size_t blockTexelCount() const noexcept = 0;

            [[nodiscard]]
            virtual std::vector<SampleVariant> variantSample(const Surface<const gpufmt::byte> &surface, const gpufmt::Extent &bloxel, BlockSampleError &error) const noexcept = 0;

            [[nodiscard]]
            virtual std::vector<gpufmt::byte> narrowSample(const Surface<const gpufmt::byte> &surface, const gpufmt::Extent &bloxel, BlockSampleError &error) const noexcept = 0;

            [[nodiscard]]
            virtual std::vector<gpufmt::byte> wideSample(const Surface<const gpufmt::byte> &surface, const gpufmt::Extent &bloxel, BlockSampleError &error) const noexcept = 0;

            [[nodiscard]]
            virtual BlockSampleError variantSampleTo(const Surface<const gpufmt::byte> &surface, const gpufmt::Extent &bloxel, std::back_insert_iterator<std::vector<SampleVariant>> sampleInserter) const noexcept = 0;

            [[nodiscard]]
            virtual BlockSampleError variantSampleTo(const Surface<const gpufmt::byte> &surface, const gpufmt::Extent &bloxel, gpufmt::span<SampleVariant> samples) const noexcept = 0;

            [[nodiscard]]
            virtual BlockSampleError narrowSampleTo(const Surface<const gpufmt::byte> &surface, const gpufmt::Extent &bloxel, gpufmt::span<gpufmt::byte> sampleData) const noexcept = 0;

            [[nodiscard]]
            virtual BlockSampleError wideSampleTo(const Surface<const gpufmt::byte> &surface, const gpufmt::Extent &bloxel, gpufmt::span<gpufmt::byte> sampleData) const noexcept = 0;
        };

        template<Format FormatV>
        class BlockSamplerT : public BaseBlockSampler {
        public:
            BlockSamplerT() noexcept = default;
            BlockSamplerT(const BlockSamplerT &) noexcept = default;
            BlockSamplerT(BlockSamplerT &&) noexcept = default;
            ~BlockSamplerT() noexcept override = default;

            BlockSamplerT &operator=(const BlockSamplerT &) noexcept = default;
            BlockSamplerT &operator=(BlockSamplerT &&) noexcept = default;

            [[nodiscard]]
            const Extent &blockExtent() const noexcept override {
                return FormatTraits<FormatV>::BlockExtent;
            }

            [[nodiscard]]
            size_t blockTexelCount() const noexcept override {
                return FormatTraits<FormatV>::BlockTexelCount;
            }

            [[nodiscard]]
            std::vector<SampleVariant> variantSample(const Surface<const gpufmt::byte> &surface, const gpufmt::Extent &bloxel, BlockSampleError &error) const noexcept final {
                std::vector<SampleVariant> texels;
                texels.reserve(FormatTraits<FormatV>::BlockTexelCount);

                error = variantSampleTo(surface, bloxel, std::back_inserter<std::vector<SampleVariant>>(texels));
                return texels;
            }

            [[nodiscard]]
            std::vector<gpufmt::byte> narrowSample(const Surface<const gpufmt::byte> &surface, const gpufmt::Extent &bloxel, BlockSampleError &error) const noexcept final {
                std::vector<gpufmt::byte> texels;
                texels.resize(FormatTraits<FormatV>::NarrowSampleBlockByteSize);

                error = narrowSampleTo(surface, bloxel, texels);
                return texels;
            }

            [[nodiscard]]
            std::vector<gpufmt::byte> wideSample(const Surface<const gpufmt::byte> &surface, const gpufmt::Extent &bloxel, BlockSampleError &error) const noexcept final {
                std::vector<gpufmt::byte> texels;
                texels.resize(FormatTraits<FormatV>::WideSampleBlockByteSize);

                error = wideSampleTo(surface, bloxel, texels);
                return texels;
            }

            [[nodiscard]]
            BlockSampleError variantSampleTo(const Surface<const gpufmt::byte> &surface, const gpufmt::Extent &bloxel, [[maybe_unused]] std::back_insert_iterator<std::vector<SampleVariant>> inserter) const noexcept final {
                using Traits = FormatTraits<FormatV>;
                using Storage = FormatStorage<FormatV>;

                if constexpr(Traits::info.depth || Traits::info.stencil) {
                    return BlockSampleError::DepthStencilUnsupported;
                } else if constexpr(Traits::info.blockByteSize == 0 || !Storage::Readable) {
                    return BlockSampleError::InvalidFormat;
                } else {
                    size_t expectedSurfaceByteSize = Traits::BlockByteSize * surface.extentInBlocks.x * surface.extentInBlocks.y * surface.extentInBlocks.z;

                    if(surface.blockData.size_bytes() < expectedSurfaceByteSize) {
                        return BlockSampleError::SourceTooSmall;
                    }

                    gpufmt::span<const Traits::BlockType> blockSpan(reinterpret_cast<const Traits::BlockType *>(surface.blockData.data()), surface.blockData.size_bytes() / Traits::BlockByteSize);
                    size_t blockIndex = Storage::getBlockIndex(bloxel.x, bloxel.y, bloxel.z, surface.extentInBlocks);
                    
                    if(blockIndex > blockSpan.size()) {
                        return BlockSampleError::BloxelOutOfRange;
                    }
                    
                    std::array<typename Traits::WideSampleType, Traits::BlockTexelCount> wideSamples;

                    Storage::loadBlock(blockSpan[blockIndex], wideSamples);

                    for(const auto &sample : wideSamples) {
                        inserter = sample;
                    }

                    return BlockSampleError::None;
                }
            }

            [[nodiscard]]
            BlockSampleError variantSampleTo(const Surface<const gpufmt::byte> &surface, const gpufmt::Extent &bloxel, gpufmt::span<SampleVariant> samples) const noexcept {
                using Traits = FormatTraits<FormatV>;
                using Storage = FormatStorage<FormatV>;

                if constexpr(Traits::info.depth || Traits::info.stencil) {
                    return BlockSampleError::DepthStencilUnsupported;
                } else if constexpr(Traits::info.blockByteSize == 0 || !Storage::Readable) {
                    return BlockSampleError::InvalidFormat;
                } else {
                    size_t expectedSurfaceByteSize = Traits::BlockByteSize * surface.extentInBlocks.x * surface.extentInBlocks.y * surface.extentInBlocks.z;

                    if(surface.blockData.size_bytes() < expectedSurfaceByteSize) {
                        return BlockSampleError::SourceTooSmall;
                    }

                    gpufmt::span<const Traits::BlockType> blockSpan(reinterpret_cast<const Traits::BlockType *>(surface.blockData.data()), surface.blockData.size_bytes() / Traits::BlockByteSize);
                    size_t blockIndex = Storage::getBlockIndex(bloxel.x, bloxel.y, bloxel.z, surface.extentInBlocks);
                    
                    if(blockIndex > blockSpan.size()) {
                        return BlockSampleError::BloxelOutOfRange;
                    }
                    
                    std::array<typename Traits::WideSampleType, Traits::BlockTexelCount> wideSamples;

                    Storage::loadBlock(blockSpan[blockIndex], wideSamples);

                    std::transform(wideSamples.cbegin(), wideSamples.cend(), samples.begin(), [](const typename Traits::WideSampleType &wideSample) {
                        return wideSample;
                    });

                    return BlockSampleError::None;
                }
            }

            [[nodiscard]]
            BlockSampleError narrowSampleTo(const Surface<const gpufmt::byte> &surface, const gpufmt::Extent &bloxel, gpufmt::span<gpufmt::byte> byteStream) const noexcept final {
                using Traits = FormatTraits<FormatV>;
                using Storage = FormatStorage<FormatV>;

                if constexpr(Traits::info.depth || Traits::info.stencil) {
                    return BlockSampleError::DepthStencilUnsupported;
                } else if constexpr(Traits::info.blockByteSize == 0 || !Storage::Readable) {
                    return BlockSampleError::InvalidFormat;
                } else {
                    size_t expectedSurfaceByteSize = Traits::BlockByteSize * surface.extentInBlocks.x * surface.extentInBlocks.y * surface.extentInBlocks.z;

                    if(surface.blockData.size_bytes() < expectedSurfaceByteSize) {
                        return BlockSampleError::SourceTooSmall;
                    }

                    if(byteStream.size_bytes() < Traits::NarrowSampleBlockByteSize) {
                        return BlockSampleError::DestinationTooSmall;
                    }

                    gpufmt::span<const Traits::BlockType> blockSpan(reinterpret_cast<const Traits::BlockType *>(surface.blockData.data()), surface.blockData.size_bytes() / Traits::BlockByteSize);
                    size_t blockIndex = Storage::getBlockIndex(bloxel.x, bloxel.y, bloxel.z, surface.extentInBlocks);

                    if(blockIndex > blockSpan.size()) {
                        return BlockSampleError::BloxelOutOfRange;
                    }

                    gpufmt::span<typename Traits::NarrowSampleType, Traits::BlockTexelCount> sampleSpan(reinterpret_cast<typename Traits::NarrowSampleType *>(byteStream.data()), byteStream.size() / Traits::NarrowSampleByteSize);

                    Storage::loadBlock(blockSpan[blockIndex], sampleSpan);

                    return BlockSampleError::None;
                }
            }

            [[nodiscard]]
            BlockSampleError wideSampleTo(const Surface<const gpufmt::byte> &surface, const gpufmt::Extent &bloxel, gpufmt::span<gpufmt::byte> byteStream) const noexcept final {
                using Traits = FormatTraits<FormatV>;
                using Storage = FormatStorage<FormatV>;

                if constexpr(Traits::info.depth || Traits::info.stencil) {
                    return BlockSampleError::DepthStencilUnsupported;
                } else if constexpr(Traits::info.blockByteSize == 0 || !Storage::Readable) {
                    return BlockSampleError::InvalidFormat;
                } else {
                    size_t expectedSurfaceByteSize = Traits::BlockByteSize * surface.extentInBlocks.x * surface.extentInBlocks.y * surface.extentInBlocks.z;

                    if(surface.blockData.size_bytes() < expectedSurfaceByteSize) {
                        return BlockSampleError::SourceTooSmall;
                    }

                    if(byteStream.size_bytes() < Traits::WideSampleBlockByteSize) {
                        return BlockSampleError::DestinationTooSmall;
                    }

                    gpufmt::span<const Traits::BlockType> blockSpan(reinterpret_cast<const Traits::BlockType*>(surface.blockData.data()), surface.blockData.size_bytes() / Traits::BlockByteSize);
                    size_t blockIndex = Storage::getBlockIndex(bloxel.x, bloxel.y, bloxel.z, surface.extentInBlocks);

                    if(blockIndex > blockSpan.size()) {
                        return BlockSampleError::BloxelOutOfRange;
                    }

                    gpufmt::span<typename Traits::WideSampleType, Traits::BlockTexelCount> wideSamplesOut(reinterpret_cast<typename Traits::WideSampleType *>(byteStream.data()), byteStream.size_bytes() / Traits::WideSampleByteSize);
                    
                    Storage::loadBlock(blockSpan[blockIndex], wideSamplesOut);

                    return BlockSampleError::None;
                }
            }
        };

        template<gpufmt::Format FormatV>
        class MakeBlockSampler {
        public:
            std::unique_ptr<BaseBlockSampler> operator()() const noexcept {
                return std::make_unique<BlockSamplerT<FormatV>>();
            }
        };
    }

    [[nodiscard]]
    std::pair<gpufmt::SampleVariant, gpufmt::SampleVariant> sampleRange(gpufmt::Format format) noexcept {
        return visitFormat<internal::SampleRangeHelper>(format);
    }


    BlockSampler::BlockSampler() noexcept {}

    BlockSampler::BlockSampler(gpufmt::Format format) noexcept
        : mFormat(format)
    {
        mSampler = visitFormat<internal::MakeBlockSampler>(format);
    }

    BlockSampler::BlockSampler(BlockSampler &&other) noexcept
        : mSampler(std::move(other.mSampler))
        , mFormat(other.mFormat)
    {
    }

    BlockSampler::~BlockSampler() {}

    BlockSampler &BlockSampler::operator=(BlockSampler &&other) noexcept {
        mFormat = other.mFormat;
        other.mFormat = gpufmt::Format::UNDEFINED;

        mSampler = std::move(other.mSampler);

        return *this;
    }

    [[nodiscard]]
    gpufmt::Format BlockSampler::format() const noexcept {
        return mFormat;
    }

    [[nodiscard]]
    const Extent& BlockSampler::blockExtent() const noexcept {
        return mSampler->blockExtent();
    }

    [[nodiscard]]
    size_t BlockSampler::blockTexelCount() const noexcept {
        return mSampler->blockTexelCount();
    }

    std::vector<gpufmt::byte> BlockSampler::narrowSample(const Surface<const gpufmt::byte> &surface, const gpufmt::Extent &bloxel, BlockSampleError &error) const noexcept {
        return mSampler->narrowSample(surface, bloxel, error);
    }

    BlockSampleError BlockSampler::narrowSampleTo(const Surface<const gpufmt::byte> &surface, const gpufmt::Extent &bloxel, gpufmt::span<gpufmt::byte> sampleBuffer) const noexcept {
        return mSampler->narrowSampleTo(surface, bloxel, sampleBuffer);
    }

    std::vector<gpufmt::byte> BlockSampler::wideSample(const Surface<const gpufmt::byte> &surface, const gpufmt::Extent &bloxel, BlockSampleError &error) const noexcept {
        return mSampler->wideSample(surface, bloxel, error);
    }

    BlockSampleError BlockSampler::wideSampleTo(const Surface<const gpufmt::byte> &surface, const gpufmt::Extent &bloxel, gpufmt::span<gpufmt::byte> sampleBuffer) const noexcept {
        return mSampler->wideSampleTo(surface, bloxel, sampleBuffer);
    }

    std::vector<gpufmt::SampleVariant> BlockSampler::variantSample(const Surface<const gpufmt::byte> &surface, const gpufmt::Extent &bloxel, BlockSampleError &error) const noexcept {
        return mSampler->variantSample(surface, bloxel, error);
    }

    BlockSampleError BlockSampler::variantSampleTo(const Surface<const gpufmt::byte> &surface, const gpufmt::Extent &bloxel, gpufmt::span<gpufmt::SampleVariant> samples) const noexcept {
        return mSampler->variantSampleTo(surface, bloxel, samples);
    }
}