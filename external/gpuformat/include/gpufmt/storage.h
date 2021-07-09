#pragma once

#pragma once

#include <gpufmt/config.h>
#include <gpufmt/definitions.h>
#include <gpufmt/format.h>

#include <gpufmt/internal/bit_packing.h>
#include <gpufmt/internal/format_packing.h>
#include <gpufmt/traits.h>
#include <gpufmt/visit.h>

#ifndef GF_EXCLUDE_COMPRESSED_FORMATS
#ifndef GF_EXCLUDE_ASTC_COMPRESSED_FORMATS
#include <gpufmt/internal/astc.h>
#endif

#ifndef GF_EXCLUDE_BC_COMPRESSED_FORMATS
#include <gpufmt/internal/bc.h>
#endif

#ifndef GF_EXCLUDE_ETC_COMPRESSED_FORMATS
#include <gpufmt/internal/etc.h>
#endif

#ifndef GF_EXCLUDE_PVRTC_COMPRESSED_FORMATS
#include <gpufmt/internal/pvrtc.h>
#endif
#endif

#include <glm/gtc/packing.hpp>
#include <glm/gtx/vec_swizzle.hpp>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include <algorithm>

namespace gpufmt {
    namespace internal {
        template<class T>
        [[nodiscard]]
        inline static constexpr glm::vec<4, T> expandSampleType(T value) noexcept {
            return glm::vec<4, T>{value, T(0), T(0), T(1)};
        }

        template<class T>
        [[nodiscard]]
        inline static constexpr glm::vec<4, T> expandSampleType(glm::vec<2, T> value) noexcept {
            return glm::vec<4, T>{value, T(0), T(1)};
        }

        template<class T>
        [[nodiscard]]
        inline static constexpr glm::vec<4, T> expandSampleType(const glm::vec<3, T> &value) noexcept {
            return glm::vec<4, T>{value, T(1)};
        }

        template<class T>
        [[nodiscard]]
        inline static constexpr glm::vec<4, T> expandSampleType(const glm::vec<4, T> &value) noexcept {
            return value;
        }

        template<class T, size_t ComponentCount>
        class SampleTruncator {};

        template<class T>
        class SampleTruncator<T, 1> {
        public:
            [[nodiscard]]
            constexpr T operator()(const glm::vec<4, T> &value) const noexcept {
                return value.x;
            }
        };

        template<class T>
        class SampleTruncator<T, 2> {
        public:
            [[nodiscard]]
            constexpr glm::vec<2, T> operator()(const glm::vec<4, T> &value) const noexcept {
                return glm::vec<2, T>(value.x, value.y);
            }
        };

        template<class T>
        class SampleTruncator<T, 3> {
        public:
            [[nodiscard]]
            constexpr glm::vec<3, T> operator()(const glm::vec<4, T> &value) const noexcept {
                return glm::vec<3, T>(value.x, value.y, value.z);
            }
        };

        template<class T>
        class SampleTruncator<T, 4> {
        public:
            [[nodiscard]]
            constexpr glm::vec<4, T> operator()(const glm::vec<4, T> &value) const noexcept {
                return value;
            }
        };

        template<class T, size_t N>
        constexpr auto truncateSample(const glm::vec<4, T> &value) noexcept {
            return SampleTruncator<T, N>()(value);
        }

        class LinearBlockStorage {
        public:
            [[nodiscard]]
            static constexpr typename size_t getBlockIndex(uint32_t blockX, uint32_t blockY, uint32_t blockZ, const gpufmt::Extent &surfaceBlockExtent) noexcept {
                return static_cast<size_t>(blockZ) *(surfaceBlockExtent.y * surfaceBlockExtent.x) + blockY * surfaceBlockExtent.x + blockX;
            }

        protected:
            virtual ~LinearBlockStorage() = default;
        };

        template<gpufmt::Format FormatV>
        class BaseStandardFormatStorage : public LinearBlockStorage {
        public:
            using Traits = FormatTraits<FormatV>;
            static constexpr bool Writeable = Traits::info.writeable;
            static constexpr bool Readable = Traits::info.readable;
            static constexpr bool Decompressible = Traits::info.decompressible;
            
            // narrow load
            [[nodiscard]]
            static constexpr void loadBlock(typename Traits::BlockType value, gpufmt::span<typename Traits::NarrowSampleType, Traits::BlockTexelCount> samples) noexcept {
                loadBlock(value, samples[0]);
            }

            [[nodiscard]]
            static constexpr void loadBlock(typename Traits::BlockType value, typename Traits::NarrowSampleType &sample) noexcept {
                sample = internal::formatUnpack<FormatV>(value);
            }

            //wide load
            template<bool LocalNarrowIsWide = Traits::NarrowIsWide>
            [[nodiscard]]
            static constexpr void loadBlock(typename Traits::BlockType value, gpufmt::span<typename Traits::WideSampleType, Traits::BlockTexelCount> samples, typename std::enable_if<!LocalNarrowIsWide>::type* = 0) noexcept {
                loadBlock(value, samples[0]);
            }

            template<bool LocalNarrowIsWide = Traits::NarrowIsWide>
            [[nodiscard]]
            static constexpr void loadBlock(typename Traits::BlockType value, typename Traits::WideSampleType &sample, typename std::enable_if<!LocalNarrowIsWide>::type* = 0) noexcept {
                sample = expandSampleType(internal::formatUnpack<FormatV>(value));
            }

            //narrow store
            [[nodiscard]]
            static constexpr typename Traits::BlockType storeBlock(gpufmt::span<typename Traits::NarrowSampleType, Traits::BlockTexelCount> value) noexcept {
                return internal::formatPack<FormatV>(value[0]);
            }

            //wide store
            template<bool LocalNarrowIsWide = Traits::NarrowIsWide>
            [[nodiscard]]
            static constexpr typename Traits::BlockType storeBlock(gpufmt::span<typename Traits::WideSampleType, Traits::BlockTexelCount> value, typename std::enable_if<!LocalNarrowIsWide>::type* = 0) noexcept {
                const auto narrowValue = internal::truncateSample<typename Traits::ValueType, Traits::ComponentCount>(value[0]);
                return internal::formatPack<FormatV>(narrowValue);
            }

        protected:
            virtual ~BaseStandardFormatStorage() = default;
        };

        template<bool B, gpufmt::Format FormatTrue, gpufmt::Format FormatFalse>
        struct FormatConditional {
            static constexpr gpufmt::Format format = FormatTrue;
        };

        template<gpufmt::Format FormatTrue, gpufmt::Format FormatFalse>
        struct FormatConditional<false, FormatTrue, FormatFalse> {
            static constexpr gpufmt::Format format = FormatFalse;
        };

        template<gpufmt::Format FormatV, class CompressedBlockT, class DecompressedBlockT, class DecompressFunc>
        DecompressError decompressBlocks(const gpufmt::Surface<const CompressedBlockT> &compressedSurface,
                                         const gpufmt::Surface<DecompressedBlockT> &decompressedSurface,
                                         DecompressFunc func)
        {
            if(compressedSurface.extentInBlocks.x == 0 ||
               compressedSurface.extentInBlocks.y == 0 ||
               compressedSurface.extentInBlocks.z == 0)
            {
                return DecompressError::SourceTooSmall;
            }

            static constexpr Extent compressedBlockExtent = gpufmt::FormatTraits<FormatV>::BlockExtent;
            Extent maxDecompressedExtent = compressedSurface.extentInBlocks * compressedBlockExtent;
            Extent minDecompressedExtent = maxDecompressedExtent - (compressedBlockExtent - Extent{ 1, 1, 1 });

            if(decompressedSurface.extentInBlocks.x < minDecompressedExtent.x ||
               decompressedSurface.extentInBlocks.y < minDecompressedExtent.y || 
               decompressedSurface.extentInBlocks.z < minDecompressedExtent.z)
            {
                return DecompressError::DestinationTooSmall;
            }

            Extent fullDecompressExtents;

            if(decompressedSurface.extentInBlocks == maxDecompressedExtent) {
                fullDecompressExtents = compressedSurface.extentInBlocks;
            } else {
                fullDecompressExtents = compressedSurface.extentInBlocks - Extent{ 1, 1, 1 };
            }

            std::array<DecompressedBlockT, gpufmt::FormatTraits<FormatV>::BlockTexelCount> decompressedTexels;

            //handle all the blocks that can be fully decompressed
            for(uint32_t blockZ = 0; blockZ < fullDecompressExtents.z; ++blockZ) {
                for(uint32_t blockY = 0; blockY < fullDecompressExtents.y; ++blockY) {
                    for(uint32_t blockX = 0; blockX < fullDecompressExtents.x; ++blockX) {
                        uint32_t compressedIndex = blockZ * (compressedSurface.extentInBlocks.y * compressedSurface.extentInBlocks.x) +
                            blockY * (compressedSurface.extentInBlocks.x) + blockX;

                        func(compressedSurface.blockData[compressedIndex], decompressedTexels);

                        Extent topLeftTexel{ blockX * compressedSurface.extentInBlocks.x,
                                             blockY * compressedSurface.extentInBlocks.y,
                                             blockZ * compressedSurface.extentInBlocks.z };

                        uint32_t decompressedIndex = topLeftTexel.z * (decompressedSurface.extentInBlocks.y * decompressedSurface.extentInBlocks.x) +
                            topLeftTexel.y * (decompressedSurface.extentInBlocks.x) + topLeftTexel.x;

                        for(uint32_t y = 0; y < compressedBlockExtent.y; ++y) {
                            auto row = decompressedSurface.blockData.subspan(decompressedIndex, compressedBlockExtent.x);
                            std::copy(decompressedTexels.cbegin(), decompressedTexels.cbegin() + compressedBlockExtent.x, row.begin());

                            decompressedIndex += decompressedSurface.extentInBlocks.x;
                        }
                    }
                }
            }

            if(fullDecompressExtents == compressedSurface.extentInBlocks) {
                return DecompressError::None;
            }

            //handle the blocks that need partial copies
            Extent partialBlockExtent = compressedBlockExtent - (maxDecompressedExtent - decompressedSurface.extentInBlocks);

            // all the lonely blocks at the end of the row
            for(uint32_t blockZ = 0; blockZ < fullDecompressExtents.z; ++blockZ) {
                for(uint32_t blockY = 0; blockY < fullDecompressExtents.y; ++blockY) {
                    for(uint32_t blockX = fullDecompressExtents.x; blockX < compressedSurface.extentInBlocks.x; ++blockX) {
                        uint32_t compressedIndex = blockZ * (compressedSurface.extentInBlocks.y * compressedSurface.extentInBlocks.x) +
                            blockY * (compressedSurface.extentInBlocks.x) + blockX;

                        func(compressedSurface.blockData[compressedIndex], decompressedTexels);

                        Extent frontTopLeftTexel{ blockX * compressedSurface.extentInBlocks.x,
                                                  blockY * compressedSurface.extentInBlocks.y,
                                                  blockZ * compressedSurface.extentInBlocks.z };

                        uint32_t decompressedIndex = frontTopLeftTexel.z * (decompressedSurface.extentInBlocks.y * decompressedSurface.extentInBlocks.x) +
                            frontTopLeftTexel.y * (decompressedSurface.extentInBlocks.x) + frontTopLeftTexel.x;

                        for(uint32_t y = 0; y < compressedBlockExtent.y; ++y) {
                            auto row = decompressedSurface.blockData.subspan(decompressedIndex, partialBlockExtent.x);
                            std::copy(decompressedTexels.cbegin(), decompressedTexels.cbegin() + partialBlockExtent.x, row.begin());

                            decompressedIndex += decompressedSurface.extentInBlocks.x;
                        }
                    }
                }
            }

            // all the lonely blocks at the bottom of each slice
            for(uint32_t blockZ = 0; blockZ < fullDecompressExtents.z; ++blockZ) {
                for(uint32_t blockY = fullDecompressExtents.y; blockY < compressedSurface.extentInBlocks.y; ++blockY) {
                    for(uint32_t blockX = 0; blockX < fullDecompressExtents.x; ++blockX) {
                        uint32_t compressedIndex = blockZ * (compressedSurface.extentInBlocks.y * compressedSurface.extentInBlocks.x) +
                            blockY * (compressedSurface.extentInBlocks.x) + blockX;

                        func(compressedSurface.blockData[compressedIndex], decompressedTexels);

                        Extent frontTopLeftTexel{ blockX * compressedSurface.extentInBlocks.x,
                                                  blockY * compressedSurface.extentInBlocks.y,
                                                  blockZ * compressedSurface.extentInBlocks.z };

                        uint32_t decompressedIndex = frontTopLeftTexel.z * (decompressedSurface.extentInBlocks.y * decompressedSurface.extentInBlocks.x) +
                            frontTopLeftTexel.y * (decompressedSurface.extentInBlocks.x) + frontTopLeftTexel.x;

                        for(uint32_t y = 0; y < partialBlockExtent.y; ++y) {
                            auto row = decompressedSurface.blockData.subspan(decompressedIndex, compressedBlockExtent.x);
                            std::copy(decompressedTexels.cbegin(), decompressedTexels.cbegin() + compressedBlockExtent.x, row.begin());

                            decompressedIndex += decompressedSurface.extentInBlocks.x;
                        }
                    }
                }
            }

            // all the lonely block at the back of the surface
            for(uint32_t blockZ = fullDecompressExtents.z; blockZ < compressedSurface.extentInBlocks.z; ++blockZ) {
                for(uint32_t blockY = 0; blockY < fullDecompressExtents.y; ++blockY) {
                    for(uint32_t blockX = 0; blockX < fullDecompressExtents.x; ++blockX) {
                        uint32_t compressedIndex = blockZ * (compressedSurface.extentInBlocks.y * compressedSurface.extentInBlocks.x) +
                            blockY * (compressedSurface.extentInBlocks.x) + blockX;

                        func(compressedSurface.blockData[compressedIndex], decompressedTexels);

                        Extent frontTopLeftTexel{ blockX * compressedSurface.extentInBlocks.x,
                                                  blockY * compressedSurface.extentInBlocks.y,
                                                  blockZ * compressedSurface.extentInBlocks.z };

                        uint32_t decompressedIndex = frontTopLeftTexel.z * (decompressedSurface.extentInBlocks.y * decompressedSurface.extentInBlocks.x) +
                            frontTopLeftTexel.y * (decompressedSurface.extentInBlocks.x) + frontTopLeftTexel.x;

                        for(uint32_t y = 0; y < compressedBlockExtent.y; ++y) {
                            auto row = decompressedSurface.blockData.subspan(decompressedIndex, compressedBlockExtent.x);
                            std::copy(decompressedTexels.cbegin(), decompressedTexels.cbegin() + compressedBlockExtent.x, row.begin());

                            decompressedIndex += decompressedSurface.extentInBlocks.x;
                        }
                    }
                }
            }

            return DecompressError::None;
        }

        template<gpufmt::Format FormatV, class CompressedBlockT, class DecompressedBlockT, class DecompressFunc>
        DecompressError decompressSurface(const gpufmt::Surface<const CompressedBlockT> &compressedSurface,
                                          const gpufmt::Surface<DecompressedBlockT> &decompressedSurface,
                                          DecompressFunc func)
        {
            if(compressedSurface.extentInBlocks.x == 0 ||
               compressedSurface.extentInBlocks.y == 0 ||
               compressedSurface.extentInBlocks.z == 0)
            {
                return DecompressError::SourceTooSmall;
            }
            
            static constexpr Extent compressedBlockExtent = gpufmt::FormatTraits<FormatV>::BlockExtent;
            Extent maxDecompressedExtent = compressedSurface.extentInBlocks * compressedBlockExtent;
            Extent minDecompressedExtent = maxDecompressedExtent - (compressedBlockExtent - Extent{ 1, 1, 1 });

            if(decompressedSurface.extentInBlocks.x < minDecompressedExtent.x || 
               decompressedSurface.extentInBlocks.y < minDecompressedExtent.y ||
               decompressedSurface.extentInBlocks.z < minDecompressedExtent.z) {
                return DecompressError::DestinationTooSmall;
            }

            func(compressedSurface, decompressedSurface);

            return DecompressError::None;
        }
    }

    template<gpufmt::Format FormatV>
    class FormatStorage {
    public:
        static constexpr bool Writeable = false;
        static constexpr bool Readable = false;
        static constexpr bool Decompressible = false;
    };

    //R4G4_UNORM_PACK8
    template<>
    class FormatStorage<Format::R4G4_UNORM_PACK8> : public internal::BaseStandardFormatStorage<Format::R4G4_UNORM_PACK8> {};

    //R4G4B4A4_UNORM_PACK16
    template<>
    class FormatStorage<Format::R4G4B4A4_UNORM_PACK16> : public internal::BaseStandardFormatStorage<Format::R4G4B4A4_UNORM_PACK16> {};

    //B4G4R4A4_UNORM_PACK16
    template<>
    class FormatStorage<Format::B4G4R4A4_UNORM_PACK16> : public internal::BaseStandardFormatStorage<Format::B4G4R4A4_UNORM_PACK16> {};

    //A4R4G4B4_UNORM_PACK16
    template<>
    class FormatStorage<Format::A4R4G4B4_UNORM_PACK16> : public internal::BaseStandardFormatStorage<Format::A4R4G4B4_UNORM_PACK16> {};

    //R5G6B5_UNORM_PACK16
    template<>
    class FormatStorage<Format::R5G6B5_UNORM_PACK16> : public internal::BaseStandardFormatStorage<Format::R5G6B5_UNORM_PACK16> {};

    //B5G6R5_UNORM_PACK16
    template<>
    class FormatStorage<Format::B5G6R5_UNORM_PACK16> : public internal::BaseStandardFormatStorage<Format::B5G6R5_UNORM_PACK16> {};

    //R5G5B5A1_UNORM_PACK16
    template<>
    class FormatStorage<Format::R5G5B5A1_UNORM_PACK16> : public internal::BaseStandardFormatStorage<Format::R5G5B5A1_UNORM_PACK16> {};

    //B5G5R5A1_UNORM_PACK16
    template<>
    class FormatStorage<Format::B5G5R5A1_UNORM_PACK16> : public internal::BaseStandardFormatStorage<Format::B5G5R5A1_UNORM_PACK16> {};

    //A1R5G5B5_UNORM_PACK16
    template<>
    class FormatStorage<Format::A1R5G5B5_UNORM_PACK16> : public internal::BaseStandardFormatStorage<Format::A1R5G5B5_UNORM_PACK16> {};

    //R8_UNORM
    template<>
    class FormatStorage<Format::R8_UNORM> : public internal::BaseStandardFormatStorage<Format::R8_UNORM> {};

    //R8_SNORM
    template<>
    class FormatStorage<Format::R8_SNORM> : public internal::BaseStandardFormatStorage<Format::R8_SNORM> {};

#ifndef GF_EXCLUDE_SCALED_FORMATS
    //R8_USCALED
    template<>
    class FormatStorage<Format::R8_USCALED> : public internal::BaseStandardFormatStorage<Format::R8_USCALED> {};

    //R8_SSCALED
    template<>
    class FormatStorage<Format::R8_SSCALED> : public internal::BaseStandardFormatStorage<Format::R8_SSCALED> {};
#endif // GF_EXCLUDE_SCALED_FORMATS

    //R8_UINT
    template<>
    class FormatStorage<Format::R8_UINT> : public internal::BaseStandardFormatStorage<Format::R8_UINT> {};

    //R8_SINT
    template<>
    class FormatStorage<Format::R8_SINT> : public internal::BaseStandardFormatStorage<Format::R8_SINT> {};

    //R8_SRGB
    template<>
    class FormatStorage<Format::R8_SRGB> : public internal::BaseStandardFormatStorage<Format::R8_SRGB> {};

    //A8_UNORM
    template<>
    class FormatStorage<Format::A8_UNORM> : public internal::LinearBlockStorage {
    public:
        using Traits = FormatTraits<Format::A8_UNORM>;
        static constexpr bool Writeable = Traits::info.writeable;
        static constexpr bool Readable = Traits::info.readable;
        static constexpr bool Decompressible = Traits::info.decompressible;

        [[nodiscard]]
        static constexpr void loadBlock(typename Traits::BlockType value, gpufmt::span<typename Traits::NarrowSampleType, Traits::BlockTexelCount> samples) noexcept {
            samples[0] = internal::unpackUNorm(value);
        }

        [[nodiscard]]
        static constexpr void loadBlock(typename Traits::BlockType value, typename Traits::NarrowSampleType &sample) noexcept {
            sample = internal::unpackUNorm(value);
        }

        [[nodiscard]]
        static constexpr void loadBlock(typename Traits::BlockType value, gpufmt::span<typename Traits::WideSampleType, Traits::BlockTexelCount> samples) noexcept {
            samples[0].r = 0.0f;
            samples[0].g = 0.0f;
            samples[0].b = 0.0f;
            samples[0].a = internal::unpackUNorm(value);
        }

        [[nodiscard]]
        static constexpr void loadBlock(typename Traits::BlockType value, typename Traits::WideSampleType &sample) noexcept {
            sample.r = 0.0f;
            sample.g = 0.0f;
            sample.b = 0.0f;
            sample.a = internal::unpackUNorm(value);
        }

        [[nodiscard]]
        static constexpr typename Traits::BlockType storeBlock(gpufmt::span<typename Traits::NarrowSampleType, Traits::BlockTexelCount> value) noexcept {
            return internal::packUNorm<Traits::BlockType>(value[0]);
        }

        [[nodiscard]]
        static constexpr typename Traits::BlockType storeBlock(gpufmt::span<typename Traits::WideSampleType, Traits::BlockTexelCount> value) noexcept {
            return internal::packUNorm<Traits::BlockType>(value[0].a);
        }
    };

    //R8G8_UNORM
    template<>
    class FormatStorage<Format::R8G8_UNORM> : public internal::BaseStandardFormatStorage<Format::R8G8_UNORM> {};

    //R8G8_SNORM
    template<>
    class FormatStorage<Format::R8G8_SNORM> : public internal::BaseStandardFormatStorage<Format::R8G8_SNORM> {};

#ifndef GF_EXCLUDE_SCALED_FORMATS
    //R8G8_USCALED
    template<>
    class FormatStorage<Format::R8G8_USCALED> : public internal::BaseStandardFormatStorage<Format::R8G8_USCALED> {};

    //R8G8_SSCALED
    template<>
    class FormatStorage<Format::R8G8_SSCALED> : public internal::BaseStandardFormatStorage<Format::R8G8_SSCALED> {};
#endif // GF_EXCLUDE_SCALED_FORMATS

    //R8G8_UINT
    template<>
    class FormatStorage<Format::R8G8_UINT> : public internal::BaseStandardFormatStorage<Format::R8G8_UINT> {};

    //R8G8_SINT
    template<>
    class FormatStorage<Format::R8G8_SINT> : public internal::BaseStandardFormatStorage<Format::R8G8_SINT> {};

    //R8G8_SRGB
    template<>
    class FormatStorage<Format::R8G8_SRGB> : public internal::BaseStandardFormatStorage<Format::R8G8_SRGB> {};

    //R8G8B8_UNORM
    template<>
    class FormatStorage<Format::R8G8B8_UNORM> : public internal::BaseStandardFormatStorage<Format::R8G8B8_UNORM> {};

    //R8G8B8_SNORM
    template<>
    class FormatStorage<Format::R8G8B8_SNORM> : public internal::BaseStandardFormatStorage<Format::R8G8B8_SNORM> {};

#ifndef GF_EXCLUDE_SCALED_FORMATS
    //R8G8B8_USCALED
    template<>
    class FormatStorage<Format::R8G8B8_USCALED> : public internal::BaseStandardFormatStorage<Format::R8G8B8_USCALED> {};

    //R8G8B8_SSCALED
    template<>
    class FormatStorage<Format::R8G8B8_SSCALED> : public internal::BaseStandardFormatStorage<Format::R8G8B8_SSCALED> {};
#endif // GF_EXCLUDE_SCALED_FORMATS

    //R8G8B8_UINT
    template<>
    class FormatStorage<Format::R8G8B8_UINT> : public internal::BaseStandardFormatStorage<Format::R8G8B8_UINT> {};

    //R8G8B8_SINT
    template<>
    class FormatStorage<Format::R8G8B8_SINT> : public internal::BaseStandardFormatStorage<Format::R8G8B8_SINT> {};

    //R8G8B8_SRGB
    template<>
    class FormatStorage<Format::R8G8B8_SRGB> : public internal::BaseStandardFormatStorage<Format::R8G8B8_SRGB> {};

    //B8G8R8_UNORM
    template<>
    class FormatStorage<Format::B8G8R8_UNORM> : public internal::BaseStandardFormatStorage<Format::B8G8R8_UNORM> {};

    //B8G8R8_SNORM
    template<>
    class FormatStorage<Format::B8G8R8_SNORM> : public internal::BaseStandardFormatStorage<Format::B8G8R8_SNORM> {};

#ifndef GF_EXCLUDE_SCALED_FORMATS
    //B8G8R8_USCALED
    template<>
    class FormatStorage<Format::B8G8R8_USCALED> : public internal::BaseStandardFormatStorage<Format::B8G8R8_USCALED> {};

    //B8G8R8_SSCALED
    template<>
    class FormatStorage<Format::B8G8R8_SSCALED> : public internal::BaseStandardFormatStorage<Format::B8G8R8_SSCALED> {};
#endif // GF_EXCLUDE_SCALED_FORMATS

    //B8G8R8_UINT
    template<>
    class FormatStorage<Format::B8G8R8_UINT> : public internal::BaseStandardFormatStorage<Format::B8G8R8_UINT> {};

    //B8G8R8_SINT
    template<>
    class FormatStorage<Format::B8G8R8_SINT> : public internal::BaseStandardFormatStorage<Format::B8G8R8_SINT> {};

    //B8G8R8_SRGB
    template<>
    class FormatStorage<Format::B8G8R8_SRGB> : public internal::BaseStandardFormatStorage<Format::B8G8R8_SRGB> {};

    //R8G8B8A8_UNORM
    template<>
    class FormatStorage<Format::R8G8B8A8_UNORM> : public internal::BaseStandardFormatStorage<Format::R8G8B8A8_UNORM> {};

    //R8G8B8A8_SNORM
    template<>
    class FormatStorage<Format::R8G8B8A8_SNORM> : public internal::BaseStandardFormatStorage<Format::R8G8B8A8_SNORM> {};

#ifndef GF_EXCLUDE_SCALED_FORMATS
    //R8G8B8A8_USCALED
    template<>
    class FormatStorage<Format::R8G8B8A8_USCALED> : public internal::BaseStandardFormatStorage<Format::R8G8B8A8_USCALED> {};

    //R8G8B8A8_SSCALED
    template<>
    class FormatStorage<Format::R8G8B8A8_SSCALED> : public internal::BaseStandardFormatStorage<Format::R8G8B8A8_SSCALED> {};
#endif // GF_EXCLUDE_SCALED_FORMATS

    //R8G8B8A8_UINT
    template<>
    class FormatStorage<Format::R8G8B8A8_UINT> : public internal::BaseStandardFormatStorage<Format::R8G8B8A8_UINT> {};

    //R8G8B8A8_SINT
    template<>
    class FormatStorage<Format::R8G8B8A8_SINT> : public internal::BaseStandardFormatStorage<Format::R8G8B8A8_SINT> {};

    //R8G8B8A8_SRGB
    template<>
    class FormatStorage<Format::R8G8B8A8_SRGB> : public internal::BaseStandardFormatStorage<Format::R8G8B8A8_SRGB> {};

    //B8G8R8A8_UNORM
    template<>
    class FormatStorage<Format::B8G8R8A8_UNORM> : public internal::BaseStandardFormatStorage<Format::B8G8R8A8_UNORM> {};

    //B8G8R8A8_SNORM
    template<>
    class FormatStorage<Format::B8G8R8A8_SNORM> : public internal::BaseStandardFormatStorage<Format::B8G8R8A8_SNORM> {};

#ifndef GF_EXCLUDE_SCALED_FORMATS
    //B8G8R8A8_USCALED
    template<>
    class FormatStorage<Format::B8G8R8A8_USCALED> : public internal::BaseStandardFormatStorage<Format::B8G8R8A8_USCALED> {};

    //B8G8R8A8_SSCALED
    template<>
    class FormatStorage<Format::B8G8R8A8_SSCALED> : public internal::BaseStandardFormatStorage<Format::B8G8R8A8_SSCALED> {};
#endif // GF_EXCLUDE_SCALED_FORMATS

    //B8G8R8A8_UINT
    template<>
    class FormatStorage<Format::B8G8R8A8_UINT> : public internal::BaseStandardFormatStorage<Format::B8G8R8A8_UINT> {};

    //B8G8R8A8_SINT
    template<>
    class FormatStorage<Format::B8G8R8A8_SINT> : public internal::BaseStandardFormatStorage<Format::B8G8R8A8_SINT> {};

    //B8G8R8A8_SRGB
    template<>
    class FormatStorage<Format::B8G8R8A8_SRGB> : public internal::BaseStandardFormatStorage<Format::B8G8R8A8_SRGB> {};

    //B8G8R8X8_UNORM
    template<>
    class FormatStorage<Format::B8G8R8X8_UNORM> : public internal::BaseStandardFormatStorage<Format::B8G8R8X8_UNORM> {};

    //B8G8R8X8_SRGB
    template<>
    class FormatStorage<Format::B8G8R8X8_SRGB> : public internal::BaseStandardFormatStorage<Format::B8G8R8X8_SRGB> {};

    //A8B8G8R8_UNORM_PACK32
    template<>
    class FormatStorage<Format::A8B8G8R8_UNORM_PACK32> : public internal::BaseStandardFormatStorage<Format::A8B8G8R8_UNORM_PACK32> {};

    //A8B8G8R8_SNORM_PACK32
    template<>
    class FormatStorage<Format::A8B8G8R8_SNORM_PACK32> : public internal::BaseStandardFormatStorage<Format::A8B8G8R8_SNORM_PACK32> {};

#ifndef GF_EXCLUDE_SCALED_FORMATS
    //A8B8G8R8_USCALED_PACK32
    template<>
    class FormatStorage<Format::A8B8G8R8_USCALED_PACK32> : public internal::BaseStandardFormatStorage<Format::A8B8G8R8_USCALED_PACK32> {};

    //A8B8G8R8_SSCALED_PACK32
    template<>
    class FormatStorage<Format::A8B8G8R8_SSCALED_PACK32> : public internal::BaseStandardFormatStorage<Format::A8B8G8R8_SSCALED_PACK32> {};
#endif // GF_EXCLUDE_SCALED_FORMATS

    //A8B8G8R8_UINT_PACK32
    template<>
    class FormatStorage<Format::A8B8G8R8_UINT_PACK32> : public internal::BaseStandardFormatStorage<Format::A8B8G8R8_UINT_PACK32> {};

    //A8B8G8R8_SINT_PACK32
    template<>
    class FormatStorage<Format::A8B8G8R8_SINT_PACK32> : public internal::BaseStandardFormatStorage<Format::A8B8G8R8_SINT_PACK32> {};

    //A8B8G8R8_SRGB_PACK32
    template<>
    class FormatStorage<Format::A8B8G8R8_SRGB_PACK32> : public internal::BaseStandardFormatStorage<Format::A8B8G8R8_SRGB_PACK32> {};

    //A2R10G10B10_UNORM_PACK32
    template<>
    class FormatStorage<Format::A2R10G10B10_UNORM_PACK32> : public internal::BaseStandardFormatStorage<Format::A2R10G10B10_UNORM_PACK32> {};

    //A2R10G10B10_SNORM_PACK32
    template<>
    class FormatStorage<Format::A2R10G10B10_SNORM_PACK32> : public internal::BaseStandardFormatStorage<Format::A2R10G10B10_SNORM_PACK32> {};

#ifndef GF_EXCLUDE_SCALED_FORMATS
    //A2R10G10B10_USCALED_PACK32
    template<>
    class FormatStorage<Format::A2R10G10B10_USCALED_PACK32> : public internal::BaseStandardFormatStorage<Format::A2R10G10B10_USCALED_PACK32> {};

    //A2R10G10B10_SSCALED_PACK32
    template<>
    class FormatStorage<Format::A2R10G10B10_SSCALED_PACK32> : public internal::BaseStandardFormatStorage<Format::A2R10G10B10_SSCALED_PACK32> {};
#endif // GF_EXCLUDE_SCALED_FORMATS

    //A2R10G10B10_UINT_PACK32
    template<>
    class FormatStorage<Format::A2R10G10B10_UINT_PACK32> : public internal::BaseStandardFormatStorage<Format::A2R10G10B10_UINT_PACK32> {};

    //A2R10G10B10_SINT_PACK32
    template<>
    class FormatStorage<Format::A2R10G10B10_SINT_PACK32> : public internal::BaseStandardFormatStorage<Format::A2R10G10B10_SINT_PACK32> {};

    //A2B10G10R10_UNORM_PACK32
    template<>
    class FormatStorage<Format::A2B10G10R10_UNORM_PACK32> : public internal::BaseStandardFormatStorage<Format::A2B10G10R10_UNORM_PACK32> {};

    //A2B10G10R10_SNORM_PACK32
    template<>
    class FormatStorage<Format::A2B10G10R10_SNORM_PACK32> : public internal::BaseStandardFormatStorage<Format::A2B10G10R10_SNORM_PACK32> {};

#ifndef GF_EXCLUDE_SCALED_FORMATS
    //A2B10G10R10_USCALED_PACK32
    template<>
    class FormatStorage<Format::A2B10G10R10_USCALED_PACK32> : public internal::BaseStandardFormatStorage<Format::A2B10G10R10_USCALED_PACK32> {};

    //A2B10G10R10_SSCALED_PACK32
    template<>
    class FormatStorage<Format::A2B10G10R10_SSCALED_PACK32> : public internal::BaseStandardFormatStorage<Format::A2B10G10R10_SSCALED_PACK32> {};
#endif // GF_EXCLUDE_SCALED_FORMATS

    //A2B10G10R10_UINT_PACK32
    template<>
    class FormatStorage<Format::A2B10G10R10_UINT_PACK32> : public internal::BaseStandardFormatStorage<Format::A2B10G10R10_UINT_PACK32> {};

    //A2B10G10R10_SINT_PACK32
    template<>
    class FormatStorage<Format::A2B10G10R10_SINT_PACK32> : public internal::BaseStandardFormatStorage<Format::A2B10G10R10_SINT_PACK32> {};

    //R16_UNORM
    template<>
    class FormatStorage<Format::R16_UNORM> : public internal::BaseStandardFormatStorage<Format::R16_UNORM> {};

    //R16_SNORM
    template<>
    class FormatStorage<Format::R16_SNORM> : public internal::BaseStandardFormatStorage<Format::R16_SNORM> {};

#ifndef GF_EXCLUDE_SCALED_FORMATS
    //R16_USCALED
    template<>
    class FormatStorage<Format::R16_USCALED> : public internal::BaseStandardFormatStorage<Format::R16_USCALED> {};

    //R16_SSCALED
    template<>
    class FormatStorage<Format::R16_SSCALED> : public internal::BaseStandardFormatStorage<Format::R16_SSCALED> {};
#endif // GF_EXCLUDE_SCALED_FORMATS

    //R16_UINT
    template<>
    class FormatStorage<Format::R16_UINT> : public internal::BaseStandardFormatStorage<Format::R16_UINT> {};

    //R16_SINT
    template<>
    class FormatStorage<Format::R16_SINT> : public internal::BaseStandardFormatStorage<Format::R16_SINT> {};

    //R16_SFLOAT
    template<>
    class FormatStorage<Format::R16_SFLOAT> : public internal::BaseStandardFormatStorage<Format::R16_SFLOAT> {
    public:
        //narrow load
        [[nodiscard]]
        static constexpr void loadBlock(Traits::BlockType value, gpufmt::span<typename Traits::NarrowSampleType, Traits::BlockTexelCount> samples) noexcept {
            loadBlock(value, samples[0]);
        }

        [[nodiscard]]
        static constexpr void loadBlock(Traits::BlockType value, Traits::NarrowSampleType &sample) noexcept {
            sample = internal::formatUnpack<Format::R16_SFLOAT>(value);
        }

        //wide load
        [[nodiscard]]
        static constexpr void loadBlock(Traits::BlockType value, gpufmt::span<Traits::WideSampleType, Traits::BlockTexelCount> samples) noexcept {
            loadBlock(value, samples[0]);
        }

        [[nodiscard]]
        static constexpr void loadBlock(Traits::BlockType value, Traits::WideSampleType &sample) noexcept {
            sample.r = internal::unpackFloat(value);
            sample.g = 0.0f;
            sample.b = 0.0f;
            sample.a = 1.0f;
        }

        //narrow store
        [[nodiscard]]
        static constexpr typename Traits::BlockType storeBlock(gpufmt::span<typename Traits::NarrowSampleType, Traits::BlockTexelCount> value) noexcept {
            return internal::formatPack<Format::R16_SFLOAT>(value[0]);
        }

        //wide store
        [[nodiscard]]
        static constexpr typename Traits::BlockType storeBlock(gpufmt::span<Traits::WideSampleType, Traits::BlockTexelCount> value) noexcept {
            return internal::formatPack<Format::R16_SFLOAT>({ value[0].r });
        }
    };

    //R16G16_UNORM
    template<>
    class FormatStorage<Format::R16G16_UNORM> : public internal::BaseStandardFormatStorage<Format::R16G16_UNORM> {};

    //R16G16_SNORM
    template<>
    class FormatStorage<Format::R16G16_SNORM> : public internal::BaseStandardFormatStorage<Format::R16G16_SNORM> {};

#ifndef GF_EXCLUDE_SCALED_FORMATS
    //R16G16_USCALED
    template<>
    class FormatStorage<Format::R16G16_USCALED> : public internal::BaseStandardFormatStorage<Format::R16G16_USCALED> {};

    //R16G16_SSCALED
    template<>
    class FormatStorage<Format::R16G16_SSCALED> : public internal::BaseStandardFormatStorage<Format::R16G16_SSCALED> {};
#endif // GF_EXCLUDE_SCALED_FORMATS

    //R16G16_UINT
    template<>
    class FormatStorage<Format::R16G16_UINT> : public internal::BaseStandardFormatStorage<Format::R16G16_UINT> {};

    //R16G16_SINT
    template<>
    class FormatStorage<Format::R16G16_SINT> : public internal::BaseStandardFormatStorage<Format::R16G16_SINT> {};

    //R16G16_SFLOAT
    template<>
    class FormatStorage<Format::R16G16_SFLOAT> : public internal::BaseStandardFormatStorage<Format::R16G16_SFLOAT> {};

    //R16G16B16_UNORM
    template<>
    class FormatStorage<Format::R16G16B16_UNORM> : public internal::BaseStandardFormatStorage<Format::R16G16B16_UNORM> {};

    //R16G16B16_SNORM
    template<>
    class FormatStorage<Format::R16G16B16_SNORM> : public internal::BaseStandardFormatStorage<Format::R16G16B16_SNORM> {};

#ifndef GF_EXCLUDE_SCALED_FORMATS
    //R16G16B16_USCALED
    template<>
    class FormatStorage<Format::R16G16B16_USCALED> : public internal::BaseStandardFormatStorage<Format::R16G16B16_USCALED> {};

    //R16G16B16_SSCALED
    template<>
    class FormatStorage<Format::R16G16B16_SSCALED> : public internal::BaseStandardFormatStorage<Format::R16G16B16_SSCALED> {};
#endif // GF_EXCLUDE_SCALED_FORMATS

    //R16G16B16_UINT
    template<>
    class FormatStorage<Format::R16G16B16_UINT> : public internal::BaseStandardFormatStorage<Format::R16G16B16_UINT> {};

    //R16G16B16_SINT
    template<>
    class FormatStorage<Format::R16G16B16_SINT> : public internal::BaseStandardFormatStorage<Format::R16G16B16_SINT> {};

    //R16G16B16_SFLOAT
    template<>
    class FormatStorage<Format::R16G16B16_SFLOAT> : public internal::BaseStandardFormatStorage<Format::R16G16B16_SFLOAT> {};

    //R16G16B16A16_UNORM
    template<>
    class FormatStorage<Format::R16G16B16A16_UNORM> : public internal::BaseStandardFormatStorage<Format::R16G16B16A16_UNORM> {};

    //R16G16B16A16_SNORM
    template<>
    class FormatStorage<Format::R16G16B16A16_SNORM> : public internal::BaseStandardFormatStorage<Format::R16G16B16A16_SNORM> {};

#ifndef GF_EXCLUDE_SCALED_FORMATS
    //R16G16B16A16_USCALED
    template<>
    class FormatStorage<Format::R16G16B16A16_USCALED> : public internal::BaseStandardFormatStorage<Format::R16G16B16A16_USCALED> {};

    //R16G16B16A16_SSCALED
    template<>
    class FormatStorage<Format::R16G16B16A16_SSCALED> : public internal::BaseStandardFormatStorage<Format::R16G16B16A16_SSCALED> {};
#endif // GF_EXCLUDE_SCALED_FORMATS

    //R16G16B16A16_UINT
    template<>
    class FormatStorage<Format::R16G16B16A16_UINT> : public internal::BaseStandardFormatStorage<Format::R16G16B16A16_UINT> {};

    //R16G16B16A16_SINT
    template<>
    class FormatStorage<Format::R16G16B16A16_SINT> : public internal::BaseStandardFormatStorage<Format::R16G16B16A16_SINT> {};

    //R16G16B16A16_SFLOAT
    template<>
    class FormatStorage<Format::R16G16B16A16_SFLOAT> : public internal::BaseStandardFormatStorage<Format::R16G16B16A16_SFLOAT> {};

    //R32_UINT
    template<>
    class FormatStorage<Format::R32_UINT> : public internal::BaseStandardFormatStorage<Format::R32_UINT> {
    public:
        //narrow load
        [[nodiscard]]
        static constexpr void loadBlock(Traits::BlockType value, gpufmt::span<typename Traits::NarrowSampleType, Traits::BlockTexelCount> samples) noexcept {
            loadBlock(value, samples[0]);
        }

        [[nodiscard]]
        static constexpr void loadBlock(Traits::BlockType value, Traits::NarrowSampleType &sample) noexcept {
            sample = internal::formatUnpack<Format::R32_UINT>(value);
        }

        //wide load
        [[nodiscard]]
        static constexpr void loadBlock(Traits::BlockType value, gpufmt::span<Traits::WideSampleType, Traits::BlockTexelCount> samples) noexcept {
            loadBlock(value, samples[0]);
        }

        [[nodiscard]]
        static constexpr void loadBlock(Traits::BlockType value, Traits::WideSampleType &sample) noexcept {
            sample.r = value;
            sample.g = 0u;
            sample.b = 0u;
            sample.a = 1u;
        }

        //narrow store
        [[nodiscard]]
        static constexpr typename Traits::BlockType storeBlock(gpufmt::span<typename Traits::NarrowSampleType, Traits::BlockTexelCount> value) noexcept {
            return internal::formatPack<Format::R32_UINT>(value[0]);
        }

        //wide store
        [[nodiscard]]
        static constexpr typename Traits::BlockType storeBlock(gpufmt::span<Traits::WideSampleType, Traits::BlockTexelCount> value) noexcept {
            return internal::formatPack<Format::R32_UINT>({ value[0].r });
        }
    };

    //R32_SINT
    template<>
    class FormatStorage<Format::R32_SINT> : public internal::BaseStandardFormatStorage<Format::R32_SINT> {
    public:
        //narrow load
        [[nodiscard]]
        static constexpr void loadBlock(Traits::BlockType value, gpufmt::span<typename Traits::NarrowSampleType, Traits::BlockTexelCount> samples) noexcept {
            loadBlock(value, samples[0]);
        }

        [[nodiscard]]
        static constexpr void loadBlock(Traits::BlockType value, Traits::NarrowSampleType &sample) noexcept {
            sample = internal::formatUnpack<Format::R32_SINT>(value);
        }

        //wide load
        [[nodiscard]]
        static constexpr void loadBlock(Traits::BlockType value, gpufmt::span<Traits::WideSampleType, Traits::BlockTexelCount> samples) noexcept {
            loadBlock(value, samples[0]);
        }

        [[nodiscard]]
        static constexpr void loadBlock(Traits::BlockType value, Traits::WideSampleType &sample) noexcept {
            sample.r = value;
            sample.g = 0;
            sample.b = 0;
            sample.a = 1;
        }

        //narrow store
        [[nodiscard]]
        static constexpr typename Traits::BlockType storeBlock(gpufmt::span<typename Traits::NarrowSampleType, Traits::BlockTexelCount> value) noexcept {
            return internal::formatPack<Format::R32_SINT>(value[0]);
        }

        //wide store
        [[nodiscard]]
        static constexpr typename Traits::BlockType storeBlock(gpufmt::span<Traits::WideSampleType, Traits::BlockTexelCount> value) noexcept {
            return internal::formatPack<Format::R32_SINT>({ value[0].r });
        }
    };

    //R32_SFLOAT
    template<>
    class FormatStorage<Format::R32_SFLOAT> : public internal::BaseStandardFormatStorage<Format::R32_SFLOAT> {
    public:
        //narrow load
        [[nodiscard]]
        static constexpr void loadBlock(Traits::BlockType value, gpufmt::span<typename Traits::NarrowSampleType, Traits::BlockTexelCount> samples) noexcept {
            loadBlock(value, samples[0]);
        }

        [[nodiscard]]
        static constexpr void loadBlock(Traits::BlockType value, Traits::NarrowSampleType &sample) noexcept {
            sample = internal::formatUnpack<Format::R32_SFLOAT>(value);
        }

        //wide load
        [[nodiscard]]
        static constexpr void loadBlock(Traits::BlockType value, gpufmt::span<Traits::WideSampleType, Traits::BlockTexelCount> samples) noexcept {
            loadBlock(value, samples[0]);
        }

        [[nodiscard]]
        static constexpr void loadBlock(Traits::BlockType value, Traits::WideSampleType &sample) noexcept {
            sample.r = value;
            sample.g = 0.0f;
            sample.b = 0.0f;
            sample.a = 1.0f;
        }

        //narrow store
        [[nodiscard]]
        static constexpr typename Traits::BlockType storeBlock(gpufmt::span<typename Traits::NarrowSampleType, Traits::BlockTexelCount> value) noexcept {
            return internal::formatPack<Format::R32_SFLOAT>(value[0]);
        }

        //wide store
        [[nodiscard]]
        static constexpr typename Traits::BlockType storeBlock(gpufmt::span<Traits::WideSampleType, Traits::BlockTexelCount> value) noexcept {
            return internal::formatPack<Format::R32_SFLOAT>({ value[0].r });
        }
    };

    //R32G32_UINT
    template<>
    class FormatStorage<Format::R32G32_UINT> : public internal::BaseStandardFormatStorage<Format::R32G32_UINT> {};

    //R32G32_SINT
    template<>
    class FormatStorage<Format::R32G32_SINT> : public internal::BaseStandardFormatStorage<Format::R32G32_SINT> {};

    //R32G32_SFLOAT
    template<>
    class FormatStorage<Format::R32G32_SFLOAT> : public internal::BaseStandardFormatStorage<Format::R32G32_SFLOAT> {};

    //R32G32B32_UINT
    template<>
    class FormatStorage<Format::R32G32B32_UINT> : public internal::BaseStandardFormatStorage<Format::R32G32B32_UINT> {};

    //R32G32B32_SINT
    template<>
    class FormatStorage<Format::R32G32B32_SINT> : public internal::BaseStandardFormatStorage<Format::R32G32B32_SINT> {};

    //R32G32B32_SFLOAT
    template<>
    class FormatStorage<Format::R32G32B32_SFLOAT> : public internal::BaseStandardFormatStorage<Format::R32G32B32_SFLOAT> {};

    //R32G32B32A32_UINT
    template<>
    class FormatStorage<Format::R32G32B32A32_UINT> : public internal::BaseStandardFormatStorage<Format::R32G32B32A32_UINT> {};

    //R32G32B32A32_SINT
    template<>
    class FormatStorage<Format::R32G32B32A32_SINT> : public internal::BaseStandardFormatStorage<Format::R32G32B32A32_SINT> {};

    //R32G32B32A32_SFLOAT
    template<>
    class FormatStorage<Format::R32G32B32A32_SFLOAT> : public internal::BaseStandardFormatStorage<Format::R32G32B32A32_SFLOAT> {};

#ifndef GF_EXCLUDE_64BIT_FORMATS
    //R64_UINT
    template<>
    class FormatStorage<Format::R64_UINT> : public internal::BaseStandardFormatStorage<Format::R64_UINT> {};

    //R64_SINT
    template<>
    class FormatStorage<Format::R64_SINT> : public internal::BaseStandardFormatStorage<Format::R64_SINT> {};

    //R64_SFLOAT
    template<>
    class FormatStorage<Format::R64_SFLOAT> : public internal::BaseStandardFormatStorage<Format::R64_SFLOAT> {};

    //R64G64_UINT
    template<>
    class FormatStorage<Format::R64G64_UINT> : public internal::BaseStandardFormatStorage<Format::R64G64_UINT> {};

    //R64G64_SINT
    template<>
    class FormatStorage<Format::R64G64_SINT> : public internal::BaseStandardFormatStorage<Format::R64G64_SINT> {};

    //R64G64_SFLOAT
    template<>
    class FormatStorage<Format::R64G64_SFLOAT> : public internal::BaseStandardFormatStorage<Format::R64G64_SFLOAT> {};

    //R64G64B64_UINT
    template<>
    class FormatStorage<Format::R64G64B64_UINT> : public internal::BaseStandardFormatStorage<Format::R64G64B64_UINT> {};

    //R64G64B64_SINT
    template<>
    class FormatStorage<Format::R64G64B64_SINT> : public internal::BaseStandardFormatStorage<Format::R64G64B64_SINT> {};

    //R64G64B64_SFLOAT
    template<>
    class FormatStorage<Format::R64G64B64_SFLOAT> : public internal::BaseStandardFormatStorage<Format::R64G64B64_SFLOAT> {};

    //R64G64B64A64_UINT
    template<>
    class FormatStorage<Format::R64G64B64A64_UINT> : public internal::BaseStandardFormatStorage<Format::R64G64B64A64_UINT> {};

    //R64G64B64A64_SINT
    template<>
    class FormatStorage<Format::R64G64B64A64_SINT> : public internal::BaseStandardFormatStorage<Format::R64G64B64A64_SINT> {};

    //R64G64B64A64_SFLOAT
    template<>
    class FormatStorage<Format::R64G64B64A64_SFLOAT> : public internal::BaseStandardFormatStorage<Format::R64G64B64A64_SFLOAT> {};
#endif // GF_EXCLUDE_64BIT_FORMATS

    //B10G11R11_UFLOAT_PACK32
    template<>
    class FormatStorage<Format::B10G11R11_UFLOAT_PACK32> : public internal::LinearBlockStorage {
    public:
        using Traits = FormatTraits<Format::B10G11R11_UFLOAT_PACK32>;
        static constexpr bool Writeable = Traits::info.writeable;
        static constexpr bool Readable = Traits::info.readable;
        static constexpr bool Decompressible = Traits::info.decompressible;

        //narrow load
        static void loadBlock(Traits::BlockType value, gpufmt::span<typename Traits::NarrowSampleType, Traits::BlockTexelCount> samples) noexcept {
            loadBlock(value, samples[0]);
        }

        static void loadBlock(Traits::BlockType value, Traits::NarrowSampleType &sample) noexcept {
            sample = glm::unpackF2x11_1x10(value);
        }

        //wide load
        static void loadBlock(Traits::BlockType value, gpufmt::span<Traits::WideSampleType, Traits::BlockTexelCount> samples) noexcept {
            loadBlock(value, samples[0]);
        }

        static void loadBlock(Traits::BlockType value, Traits::WideSampleType &sample) noexcept {
            glm::vec3 unpackedValue = glm::unpackF2x11_1x10(value);
            sample.r = unpackedValue.r;
            sample.g = unpackedValue.g;
            sample.b = unpackedValue.b;
            sample.a = 1.0f;
        }

        //narrow store
        [[nodiscard]]
        static typename Traits::BlockType storeBlock(gpufmt::span<typename Traits::NarrowSampleType, Traits::BlockTexelCount> value) noexcept {
            return glm::packF2x11_1x10(value[0]);
        }

        //wide store
        [[nodiscard]]
        static typename Traits::BlockType storeBlock(gpufmt::span<Traits::WideSampleType, Traits::BlockTexelCount> value) noexcept {
            Traits::NarrowSampleType narrowSample = glm::xyz(value[0]);
            return glm::packF2x11_1x10(narrowSample);
        }
    };

    //E5B9G9R9_UFLOAT_PACK32
    template<>
    class FormatStorage<Format::E5B9G9R9_UFLOAT_PACK32> : public internal::LinearBlockStorage {
    public:
        using Traits = FormatTraits<Format::E5B9G9R9_UFLOAT_PACK32>;
        static constexpr bool Writeable = Traits::info.writeable;
        static constexpr bool Readable = Traits::info.readable;
        static constexpr bool Decompressible = Traits::info.decompressible;

        //narrow load
        static void loadBlock(Traits::BlockType value, gpufmt::span<typename Traits::NarrowSampleType, Traits::BlockTexelCount> samples) noexcept {
            loadBlock(value, samples[0]);
        }

        static void loadBlock(Traits::BlockType value, Traits::NarrowSampleType &sample) noexcept {
            sample = glm::unpackF3x9_E1x5(value);
        }

        //wide load
        static void loadBlock(Traits::BlockType value, gpufmt::span<Traits::WideSampleType, Traits::BlockTexelCount> samples) noexcept {
            loadBlock(value, samples[0]);
        }

        static void loadBlock(Traits::BlockType value, Traits::WideSampleType &sample) noexcept {
            glm::vec3 unpackedSample = glm::unpackF3x9_E1x5(value);
            sample.a = 1.0f;
        }

        //narrow store
        [[nodiscard]]
        static typename Traits::BlockType storeBlock(gpufmt::span<typename Traits::NarrowSampleType, Traits::BlockTexelCount> value) noexcept {
            return glm::packF3x9_E1x5(value[0]);
        }

        //load store
        [[nodiscard]]
        static typename Traits::BlockType storeBlock(gpufmt::span<Traits::WideSampleType, Traits::BlockTexelCount> value) noexcept {
            Traits::NarrowSampleType narrowSample = glm::xyz(value[0]);
            return glm::packF3x9_E1x5(narrowSample);
        }
    };

    //D16_UNORM
    template<>
    class FormatStorage<Format::D16_UNORM> {
    public:
        using Traits = FormatTraits<Format::D16_UNORM>;
        static constexpr bool Writeable = Traits::info.writeable;
        static constexpr bool Readable = Traits::info.readable;
        static constexpr bool Decompressible = Traits::info.decompressible;
    };

    //X8_D24_UNORM_PACK32
    template<>
    class FormatStorage<Format::X8_D24_UNORM_PACK32> {
    public:
        using Traits = FormatTraits<Format::X8_D24_UNORM_PACK32>;
        static constexpr bool Writeable = Traits::info.writeable;
        static constexpr bool Readable = Traits::info.readable;
        static constexpr bool Decompressible = Traits::info.decompressible;
    };

    //D32_SFLOAT
    template<>
    class FormatStorage<Format::D32_SFLOAT> {
    public:
        using Traits = FormatTraits<Format::D32_SFLOAT>;
        static constexpr bool Writeable = Traits::info.writeable;
        static constexpr bool Readable = Traits::info.readable;
        static constexpr bool Decompressible = Traits::info.decompressible;
    };

    //S8_UINT
    template<>
    class FormatStorage<Format::S8_UINT> {
    public:
        using Traits = FormatTraits<Format::S8_UINT>;
        static constexpr bool Writeable = Traits::info.writeable;
        static constexpr bool Readable = Traits::info.readable;
        static constexpr bool Decompressible = Traits::info.decompressible;
    };

    //D16_UNORM_S8_UINT
    template<>
    class FormatStorage<Format::D16_UNORM_S8_UINT> {
    public:
        using Traits = FormatTraits<Format::D16_UNORM_S8_UINT>;
        static constexpr bool Writeable = Traits::info.writeable;
        static constexpr bool Readable = Traits::info.readable;
        static constexpr bool Decompressible = Traits::info.decompressible;
    };

    //D24_UNORM_S8_UINT
    template<>
    class FormatStorage<Format::D24_UNORM_S8_UINT> {
    public:
        using Traits = FormatTraits<Format::D24_UNORM_S8_UINT>;
        static constexpr bool Writeable = Traits::info.writeable;
        static constexpr bool Readable = Traits::info.readable;
        static constexpr bool Decompressible = Traits::info.decompressible;
    };

    //D32_SFLOAT_S8_UINT
    template<>
    class FormatStorage<Format::D32_SFLOAT_S8_UINT> {
    public:
        using Traits = FormatTraits<Format::D32_SFLOAT_S8_UINT>;
        static constexpr bool Writeable = Traits::info.writeable;
        static constexpr bool Readable = Traits::info.readable;
        static constexpr bool Decompressible = Traits::info.decompressible;
    };

#ifndef GF_EXCLUDE_COMPRESSED_FORMATS
#ifndef GF_EXCLUDE_BC_COMPRESSED_FORMATS
    namespace internal {
        template<bool sRGB>
        class BC1RGBDecompress {
        public:
            static constexpr gpufmt::Format FormatV = FormatConditional<sRGB, Format::BC1_RGB_SRGB_BLOCK, Format::BC1_RGB_UNORM_BLOCK>::format;
            using Traits = FormatTraits<FormatV>;
            using DecompressedTraits = FormatTraits<Traits::info.decompressedFormat>;
            using DecompressedTraitsAlt = FormatTraits<Traits::info.decompressedFormatAlt>;

            static gpufmt::DecompressError decompress(const Surface<const typename Traits::BlockType> &compressedBlocks, const gpufmt::Surface<glm::u8vec3> &decompressedTexels) noexcept {
                static_assert(std::is_same_v<typename DecompressedTraits::BlockType, glm::u8vec3>);

                return internal::decompressBlocks<FormatV>(compressedBlocks, decompressedTexels, [](const typename Traits::BlockType &block, gpufmt::span<glm::u8vec3> texels) {
                    std::array<glm::vec3, 16> decompressedFloats;

                    internal::decompressBC1Block_RGB(block, decompressedFloats);

                    std::transform(decompressedFloats.cbegin(), decompressedFloats.cend(), texels.begin(), [](const glm::vec3 &floatTexel) {
                        glm::u8vec3 outTexel;
                        outTexel.r = internal::packUNorm<uint8_t>(floatTexel.r);
                        outTexel.g = internal::packUNorm<uint8_t>(floatTexel.g);
                        outTexel.b = internal::packUNorm<uint8_t>(floatTexel.b);

                        return outTexel;
                    });
                });
            }

            static gpufmt::DecompressError decompress(const Surface<const typename Traits::BlockType> &compressedBlocks, const gpufmt::Surface<typename DecompressedTraitsAlt::BlockType> &decompressedTexels) noexcept {
                static_assert(std::is_same_v<typename DecompressedTraitsAlt::BlockType, glm::u8vec4>);

                return internal::decompressBlocks<FormatV>(compressedBlocks, decompressedTexels, [](const typename Traits::BlockType &block, gpufmt::span<typename DecompressedTraitsAlt::BlockType> texels) {
                    std::array<glm::vec3, 16> decompressedFloats;

                    internal::decompressBC1Block_RGB(block, decompressedFloats);

                    std::transform(decompressedFloats.cbegin(), decompressedFloats.cend(), texels.begin(), [](const glm::vec3 &floatTexel) {
                        glm::u8vec4 outTexel;
                        outTexel.r = internal::packUNorm<uint8_t>(floatTexel.r);
                        outTexel.g = internal::packUNorm<uint8_t>(floatTexel.g);
                        outTexel.b = internal::packUNorm<uint8_t>(floatTexel.b);
                        outTexel.a = 255;

                        return outTexel;
                    });
                });
            }

        protected:
            virtual ~BC1RGBDecompress() = default;
        };

        template<bool sRGB>
        class BC1RGBADecompress {
        public:
            static constexpr gpufmt::Format FormatV = FormatConditional<sRGB, Format::BC1_RGBA_SRGB_BLOCK, Format::BC1_RGBA_UNORM_BLOCK>::format;
            using Traits = FormatTraits<FormatV>;
            using DecompressedTraits = FormatTraits<Traits::info.decompressedFormat>;

            static gpufmt::DecompressError decompress(const Surface<const typename Traits::BlockType> &compressedBlocks, const Surface<typename DecompressedTraits::BlockType> &decompressedTexels) noexcept {
                return internal::decompressBlocks<FormatV>(compressedBlocks, decompressedTexels, [](const typename Traits::BlockType &block, gpufmt::span<typename DecompressedTraits::BlockType> texels) {
                    std::array<glm::vec4, 16> decompressedFloats;

                    internal::decompressBC1Block_RGBA(block, decompressedFloats);

                    std::transform(decompressedFloats.cbegin(), decompressedFloats.cend(), texels.begin(), [](const glm::vec4 &floatTexel) {
                        glm::u8vec4 outTexel;
                        outTexel.r = internal::packUNorm<uint8_t>(floatTexel.r);
                        outTexel.g = internal::packUNorm<uint8_t>(floatTexel.g);
                        outTexel.b = internal::packUNorm<uint8_t>(floatTexel.b);
                        outTexel.a = internal::packUNorm<uint8_t>(floatTexel.a);

                        return outTexel;
                    });
                });
            }

        protected:
            virtual ~BC1RGBADecompress() = default;
        };

        template<bool sRGB>
        class BC2Decompress {
        public:
            static constexpr gpufmt::Format FormatV = FormatConditional<sRGB, Format::BC2_SRGB_BLOCK, Format::BC2_UNORM_BLOCK>::format;
            using Traits = FormatTraits<FormatV>;
            using DecompressedTraits = FormatTraits<Traits::info.decompressedFormat>;

            static gpufmt::DecompressError decompress(const Surface<const typename Traits::BlockType> &compressedBlocks, const gpufmt::Surface<typename DecompressedTraits::BlockType> &decompressedTexels) noexcept {
                return internal::decompressBlocks<FormatV>(compressedBlocks, decompressedTexels, [](const typename Traits::BlockType &block, gpufmt::span<typename DecompressedTraits::BlockType> texels) {
                    std::array<glm::vec4, 16> decompressedFloats;

                    internal::decompressBC2Block(block, decompressedFloats);

                    std::transform(decompressedFloats.cbegin(), decompressedFloats.cend(), texels.begin(), [](const glm::vec4 &floatTexel) {
                        glm::u8vec4 outTexel;
                        outTexel.r = internal::packUNorm<uint8_t>(floatTexel.r);
                        outTexel.g = internal::packUNorm<uint8_t>(floatTexel.g);
                        outTexel.b = internal::packUNorm<uint8_t>(floatTexel.b);
                        outTexel.a = internal::packUNorm<uint8_t>(floatTexel.a);

                        return outTexel;
                    });
                });
            }

        protected:
            virtual ~BC2Decompress() = default;
        };

        template<bool sRGB>
        class BC3Decompress {
        public:
            static constexpr gpufmt::Format FormatV = FormatConditional<sRGB, Format::BC3_SRGB_BLOCK, Format::BC3_UNORM_BLOCK>::format;
            using Traits = FormatTraits<FormatV>;
            using DecompressedTraits = FormatTraits<Traits::info.decompressedFormat>;

            static gpufmt::DecompressError decompress(const Surface<const typename Traits::BlockType> &compressedBlocks, const gpufmt::Surface<typename DecompressedTraits::BlockType> &decompressedTexels) noexcept {
                return internal::decompressBlocks<FormatV>(compressedBlocks, decompressedTexels, [](const typename Traits::BlockType &block, gpufmt::span<typename DecompressedTraits::BlockType> texels) {
                    std::array<glm::vec4, 16> decompressedFloats;

                    internal::decompressBC3Block(block, decompressedFloats);

                    std::transform(decompressedFloats.cbegin(), decompressedFloats.cend(), texels.begin(), [](const glm::vec4 &floatTexel) {
                        glm::u8vec4 outTexel;
                        outTexel.r = internal::packUNorm<uint8_t>(floatTexel.r);
                        outTexel.g = internal::packUNorm<uint8_t>(floatTexel.g);
                        outTexel.b = internal::packUNorm<uint8_t>(floatTexel.b);
                        outTexel.a = internal::packUNorm<uint8_t>(floatTexel.a);

                        return outTexel;
                    });
                });
            }

        protected:
            virtual ~BC3Decompress() = default;
        };

        template<bool sRGB>
        class BC7Decompress {
        public:
            static constexpr gpufmt::Format FormatV = FormatConditional<sRGB, Format::BC7_SRGB_BLOCK, Format::BC7_UNORM_BLOCK>::format;
            using Traits = FormatTraits<FormatV>;
            using DecompressedTraits = FormatTraits<Traits::info.decompressedFormat>;

            static gpufmt::DecompressError decompress(const Surface<const typename Traits::BlockType> &compressedBlocks, const gpufmt::Surface<typename DecompressedTraits::BlockType> &decompressedTexels) noexcept {
                return internal::decompressBlocks<FormatV>(compressedBlocks, decompressedTexels, [](const typename Traits::BlockType &block, gpufmt::span<typename DecompressedTraits::BlockType> texels) {
                    internal::decompressBC7Block(block, gpufmt::span<typename DecompressedTraits::BlockType, 16>(texels.data(), 16));
                });
            }
        };
    }

    //BC1_RGB_UNORM_BLOCK
    template<>
    class FormatStorage<Format::BC1_RGB_UNORM_BLOCK> : public internal::LinearBlockStorage, public internal::BC1RGBDecompress<false> {
    public:
        using Traits = FormatTraits<Format::BC1_RGB_UNORM_BLOCK>;
        static constexpr bool Writeable = Traits::info.writeable;
        static constexpr bool Readable = Traits::info.readable;
        static constexpr bool Decompressible = Traits::info.decompressible;

        static void loadBlock(Traits::BlockType value, gpufmt::span<typename Traits::NarrowSampleType, Traits::BlockTexelCount> samples) noexcept {
            internal::decompressBC1Block_RGB(value, samples);
        }

        static void loadBlock(Traits::BlockType value, gpufmt::span<Traits::WideSampleType, Traits::BlockTexelCount> samples) noexcept {
            std::array<Traits::NarrowSampleType, Traits::BlockTexelCount> narrowSamples;
            internal::decompressBC1Block_RGB(value, narrowSamples);

            std::transform(narrowSamples.cbegin(), narrowSamples.cend(), samples.begin(), [](const Traits::NarrowSampleType &narrowSample) {
                return Traits::WideSampleType{ narrowSample, 1.0f };
            });
        }
    };

    //BC1_RGB_SRGB_BLOCK
    template<>
    class FormatStorage<Format::BC1_RGB_SRGB_BLOCK> : public internal::LinearBlockStorage, public internal::BC1RGBDecompress<true> {
    public:
        using Traits = FormatTraits<Format::BC1_RGB_SRGB_BLOCK>;
        static constexpr bool Writeable = Traits::info.writeable;
        static constexpr bool Readable = Traits::info.readable;
        static constexpr bool Decompressible = Traits::info.decompressible;

        static void loadBlock(Traits::BlockType value, gpufmt::span<typename Traits::NarrowSampleType, Traits::BlockTexelCount> samples) noexcept {
            internal::decompressBC1Block_RGB(value, samples);
        }

        static void loadBlock(Traits::BlockType value, gpufmt::span<Traits::WideSampleType, Traits::BlockTexelCount> samples) noexcept {
            std::array<Traits::NarrowSampleType, Traits::BlockTexelCount> narrowSamples;
            internal::decompressBC1Block_RGB(value, narrowSamples);

            std::transform(narrowSamples.cbegin(), narrowSamples.cend(), samples.begin(), [](const Traits::NarrowSampleType &narrowSample) {
                return Traits::WideSampleType{ narrowSample, 1.0f };
            });
        }
    };

    //BC1_RGBA_UNORM_BLOCK
    template<>
    class FormatStorage<Format::BC1_RGBA_UNORM_BLOCK> : public internal::LinearBlockStorage, public internal::BC1RGBADecompress<false> {
    public:
        using Traits = FormatTraits<Format::BC1_RGBA_UNORM_BLOCK>;
        static constexpr bool Writeable = Traits::info.writeable;
        static constexpr bool Readable = Traits::info.readable;
        static constexpr bool Decompressible = Traits::info.decompressible;

        static void loadBlock(Traits::BlockType value, gpufmt::span<typename Traits::NarrowSampleType, Traits::BlockTexelCount> samples) noexcept {
            internal::decompressBC1Block_RGBA(value, samples);
        }
    };

    //BC1_RGBA_SRGB_BLOCK
    template<>
    class FormatStorage<Format::BC1_RGBA_SRGB_BLOCK> : public internal::LinearBlockStorage, public internal::BC1RGBADecompress<true> {
    public:
        using Traits = FormatTraits<Format::BC1_RGBA_SRGB_BLOCK>;
        static constexpr bool Writeable = Traits::info.writeable;
        static constexpr bool Readable = Traits::info.readable;
        static constexpr bool Decompressible = Traits::info.decompressible;

        static void loadBlock(Traits::BlockType value, gpufmt::span<typename Traits::NarrowSampleType, Traits::BlockTexelCount> samples) noexcept {
            internal::decompressBC1Block_RGBA(value, samples);
        }
    };

    //BC2_UNORM_BLOCK
    template<>
    class FormatStorage<Format::BC2_UNORM_BLOCK> : public internal::LinearBlockStorage, public internal::BC2Decompress<false> {
    public:
        using Traits = FormatTraits<Format::BC2_UNORM_BLOCK>;
        static constexpr bool Writeable = Traits::info.writeable;
        static constexpr bool Readable = Traits::info.readable;
        static constexpr bool Decompressible = Traits::info.decompressible;

        static void loadBlock(const typename Traits::BlockType &value, gpufmt::span<typename Traits::NarrowSampleType, Traits::BlockTexelCount> samples) noexcept {
            internal::decompressBC2Block(value, samples);
        }
    };

    //BC2_SRGB_BLOCK
    template<>
    class FormatStorage<Format::BC2_SRGB_BLOCK> : public internal::LinearBlockStorage, public internal::BC2Decompress<true> {
    public:
        using Traits = FormatTraits<Format::BC2_SRGB_BLOCK>;
        static constexpr bool Writeable = Traits::info.writeable;
        static constexpr bool Readable = Traits::info.readable;
        static constexpr bool Decompressible = Traits::info.decompressible;

        static void loadBlock(const typename Traits::BlockType &value, gpufmt::span<typename Traits::NarrowSampleType, Traits::BlockTexelCount> samples) noexcept {
            internal::decompressBC2Block(value, samples);
        }
    };

    //BC3_UNORM_BLOCK
    template<>
    class FormatStorage<Format::BC3_UNORM_BLOCK> : public internal::LinearBlockStorage, public internal::BC3Decompress<false> {
    public:
        using Traits = FormatTraits<Format::BC3_UNORM_BLOCK>;
        static constexpr bool Writeable = Traits::info.writeable;
        static constexpr bool Readable = Traits::info.readable;
        static constexpr bool Decompressible = Traits::info.decompressible;

        static void loadBlock(const typename Traits::BlockType &value, gpufmt::span<typename Traits::NarrowSampleType, Traits::BlockTexelCount> samples) noexcept {
            internal::decompressBC3Block(value, samples);
        }
    };

    //BC3_SRGB_BLOCK
    template<>
    class FormatStorage<Format::BC3_SRGB_BLOCK> : public internal::LinearBlockStorage, public internal::BC3Decompress<true> {
    public:
        using Traits = FormatTraits<Format::BC3_SRGB_BLOCK>;
        static constexpr bool Writeable = Traits::info.writeable;
        static constexpr bool Readable = Traits::info.readable;
        static constexpr bool Decompressible = Traits::info.decompressible;

        static void loadBlock(const typename Traits::BlockType &value, gpufmt::span<typename Traits::NarrowSampleType, Traits::BlockTexelCount> samples) noexcept {
            internal::decompressBC3Block(value, samples);
        }
    };

    //BC4_UNORM_BLOCK
    template<>
    class FormatStorage<Format::BC4_UNORM_BLOCK> : public internal::LinearBlockStorage {
    public:
        using Traits = FormatTraits<Format::BC4_UNORM_BLOCK>;
        using DecompressedTraits = FormatTraits<Traits::info.decompressedFormat>;
        static constexpr bool Writeable = Traits::info.writeable;
        static constexpr bool Readable = Traits::info.readable;
        static constexpr bool Decompressible = Traits::info.decompressible;

        static void loadBlock(const typename Traits::BlockType &value, gpufmt::span<typename Traits::NarrowSampleType, Traits::BlockTexelCount> samples) noexcept {
            internal::decompressBC4Block_UNorm1(value, samples);
        }

        static void loadBlock(const typename Traits::BlockType &value, gpufmt::span<Traits::WideSampleType, Traits::BlockTexelCount> samples) noexcept {
            std::array<Traits::NarrowSampleType, Traits::BlockTexelCount> narrowSamples;
            internal::decompressBC4Block_UNorm1(value, narrowSamples);

            std::transform(narrowSamples.cbegin(), narrowSamples.cend(), samples.begin(), [](Traits::NarrowSampleType narrowSample) {
                return Traits::WideSampleType{ narrowSample, 0.0f, 0.0f, 1.0f };
            });
        }

        static gpufmt::DecompressError decompress(const Surface<const typename Traits::BlockType> &compressedBlocks, const gpufmt::Surface<typename DecompressedTraits::BlockType> &decompressedTexels) noexcept {
            return internal::decompressBlocks<Format::BC4_UNORM_BLOCK>(compressedBlocks, decompressedTexels, [](const typename Traits::BlockType &block, gpufmt::span<typename DecompressedTraits::BlockType> texels) {
                std::array<float, 16> decompressedFloats;

                internal::decompressBC4Block_UNorm1(block, decompressedFloats);

                std::transform(decompressedFloats.cbegin(), decompressedFloats.cend(), texels.begin(), [](float floatTexel) {
                    return internal::packUNorm<uint8_t>(floatTexel);
                });
            });
        }
    };

    //BC4_SNORM_BLOCK
    template<>
    class FormatStorage<Format::BC4_SNORM_BLOCK> : public internal::LinearBlockStorage {
    public:
        using Traits = FormatTraits<Format::BC4_SNORM_BLOCK>;
        using DecompressedTraits = FormatTraits<Traits::info.decompressedFormat>;
        static constexpr bool Writeable = Traits::info.writeable;
        static constexpr bool Readable = Traits::info.readable;
        static constexpr bool Decompressible = Traits::info.decompressible;

        static void loadBlock(const typename Traits::BlockType &value, gpufmt::span<typename Traits::NarrowSampleType, Traits::BlockTexelCount> samples) noexcept {
            internal::decompressBC4Block_SNorm1(value, samples);
        }

        static void loadBlock(const typename Traits::BlockType &value, gpufmt::span<Traits::WideSampleType, Traits::BlockTexelCount> samples) noexcept {
            std::array<Traits::NarrowSampleType, Traits::BlockTexelCount> narrowSamples;
            internal::decompressBC4Block_SNorm1(value, narrowSamples);

            std::transform(narrowSamples.cbegin(), narrowSamples.cend(), samples.begin(), [](Traits::NarrowSampleType narrowSample) {
                return Traits::WideSampleType{ narrowSample, 0.0f, 0.0f, 1.0f };
            });
        }

        static gpufmt::DecompressError decompress(const Surface<const typename Traits::BlockType> &compressedBlocks, const gpufmt::Surface<typename DecompressedTraits::BlockType> &decompressedTexels) noexcept {
            return internal::decompressBlocks<Format::BC4_SNORM_BLOCK>(compressedBlocks, decompressedTexels, [](const typename Traits::BlockType &block, gpufmt::span<typename DecompressedTraits::BlockType> texels) {
                std::array<float, 16> decompressedFloats;

                internal::decompressBC4Block_SNorm1(block, decompressedFloats);

                std::transform(decompressedFloats.cbegin(), decompressedFloats.cend(), texels.begin(), [](float floatTexel) {
                    return internal::packSNorm<int8_t>(floatTexel);
                });
            });
        }
    };

    //BC5_UNORM_BLOCK
    template<>
    class FormatStorage<Format::BC5_UNORM_BLOCK> : public internal::LinearBlockStorage {
    public:
        using Traits = FormatTraits<Format::BC5_UNORM_BLOCK>;
        using DecompressedTraits = FormatTraits<Traits::info.decompressedFormat>;
        static constexpr bool Writeable = Traits::info.writeable;
        static constexpr bool Readable = Traits::info.readable;
        static constexpr bool Decompressible = Traits::info.decompressible;

        static void loadBlock(const typename Traits::BlockType &value, gpufmt::span<typename Traits::NarrowSampleType, Traits::BlockTexelCount> samples) noexcept {
            internal::decompressBC5Block_UNorm2(value, samples);
        }

        static void loadBlock(const typename Traits::BlockType &value, gpufmt::span<Traits::WideSampleType, Traits::BlockTexelCount> samples) noexcept {
            std::array<Traits::NarrowSampleType, Traits::BlockTexelCount> narrowSamples;
            internal::decompressBC5Block_UNorm2(value, narrowSamples);

            std::transform(narrowSamples.cbegin(), narrowSamples.cend(), samples.begin(), [](Traits::NarrowSampleType narrowSample) {
                return Traits::WideSampleType{ narrowSample, 0.0f, 1.0f };
            });
        }

        static gpufmt::DecompressError decompress(const Surface<const typename Traits::BlockType> &compressedBlocks, const gpufmt::Surface<typename DecompressedTraits::BlockType> &decompressedTexels) noexcept {
            return internal::decompressBlocks<Format::BC5_UNORM_BLOCK>(compressedBlocks, decompressedTexels, [](const typename Traits::BlockType &block, gpufmt::span<typename DecompressedTraits::BlockType> texels) {
                std::array<glm::vec2, 16> decompressedFloats;

                internal::decompressBC5Block_UNorm2(block, decompressedFloats);

                std::transform(decompressedFloats.cbegin(), decompressedFloats.cend(), texels.begin(), [](glm::vec2 floatTexel) {
                    glm::u8vec2 outTexel;
                    outTexel.r = internal::packUNorm<uint8_t>(floatTexel.r);
                    outTexel.g = internal::packUNorm<uint8_t>(floatTexel.g);

                    return outTexel;
                });
            });
        }
    };

    //BC5_SNORM_BLOCK
    template<>
    class FormatStorage<Format::BC5_SNORM_BLOCK> : public internal::LinearBlockStorage {
    public:
        using Traits = FormatTraits<Format::BC5_SNORM_BLOCK>;
        using DecompressedTraits = FormatTraits<Traits::info.decompressedFormat>;
        static constexpr bool Writeable = Traits::info.writeable;
        static constexpr bool Readable = Traits::info.readable;
        static constexpr bool Decompressible = Traits::info.decompressible;

        static void loadBlock(const typename Traits::BlockType &value, gpufmt::span<typename Traits::NarrowSampleType, Traits::BlockTexelCount> samples) noexcept {
            internal::decompressBC5Block_SNorm2(value, samples);
        }

        static void loadBlock(const typename Traits::BlockType &value, gpufmt::span<Traits::WideSampleType, Traits::BlockTexelCount> samples) noexcept {
            std::array<Traits::NarrowSampleType, Traits::BlockTexelCount> narrowSamples;
            internal::decompressBC5Block_SNorm2(value, narrowSamples);

            std::transform(narrowSamples.cbegin(), narrowSamples.cend(), samples.begin(), [](Traits::NarrowSampleType narrowSample) {
                return Traits::WideSampleType{ narrowSample, 0.0f, 1.0f };
            });
        }

        static gpufmt::DecompressError decompress(const Surface<const typename Traits::BlockType> &compressedBlocks, const gpufmt::Surface<typename DecompressedTraits::BlockType> &decompressedTexels) noexcept {
            return internal::decompressBlocks<Format::BC5_SNORM_BLOCK>(compressedBlocks, decompressedTexels, [](const typename Traits::BlockType &block, gpufmt::span<typename DecompressedTraits::BlockType> texels) {
                std::array<glm::vec2, 16> decompressedFloats;

                internal::decompressBC5Block_SNorm2(block, decompressedFloats);

                std::transform(decompressedFloats.cbegin(), decompressedFloats.cend(), texels.begin(), [](glm::vec2 floatTexel) {
                    glm::u8vec2 outTexel;
                    outTexel.r = internal::packSNorm<int8_t>(floatTexel.r);
                    outTexel.g = internal::packSNorm<int8_t>(floatTexel.g);

                    return outTexel;
                });
            });
        }
    };

    //BC6H_UFLOAT_BLOCK
    template<>
    class FormatStorage<Format::BC6H_UFLOAT_BLOCK> : public internal::LinearBlockStorage {
    public:
        using Traits = FormatTraits<Format::BC6H_UFLOAT_BLOCK>;
        using DecompressedTraits = FormatTraits<Traits::info.decompressedFormat>;
        using DecompressedAltTraits = FormatTraits<Traits::info.decompressedFormatAlt>;
        static constexpr bool Writeable = Traits::info.writeable;
        static constexpr bool Readable = Traits::info.readable;
        static constexpr bool Decompressible = Traits::info.decompressible;

        static void loadBlock(const typename Traits::BlockType &value, gpufmt::span<typename Traits::NarrowSampleType, Traits::BlockTexelCount> samples) noexcept {
            internal::decompressBC6hBlock_UFloat3(value, samples);
        }

        static void loadBlock(const typename Traits::BlockType &value, gpufmt::span<Traits::WideSampleType, Traits::BlockTexelCount> samples) noexcept {
            std::array<Traits::NarrowSampleType, Traits::BlockTexelCount> narrowSamples;
            internal::decompressBC6hBlock_UFloat3(value, narrowSamples);

            std::transform(narrowSamples.cbegin(), narrowSamples.cend(), samples.begin(), [](const Traits::NarrowSampleType &narrowSample) {
                return Traits::WideSampleType{ narrowSample, 1.0f };
            });
        }

        static gpufmt::DecompressError decompress(const Surface<const typename Traits::BlockType> &compressedBlocks, const gpufmt::Surface<typename DecompressedTraits::BlockType> decompressedTexels) noexcept {
            return internal::decompressBlocks<Format::BC6H_UFLOAT_BLOCK>(compressedBlocks, decompressedTexels, [](const typename Traits::BlockType &block, gpufmt::span<typename DecompressedTraits::BlockType> texels) {
                internal::decompressBC6hBlock_UFloat3(block, gpufmt::span<typename DecompressedTraits::BlockType, 16>(texels.data(), 16));
            });
        }

        static gpufmt::DecompressError decompress(const Surface<const typename Traits::BlockType> &compressedBlocks, const gpufmt::Surface<typename DecompressedAltTraits::BlockType> decompressedTexels) noexcept {
            return internal::decompressBlocks<Format::BC6H_UFLOAT_BLOCK>(compressedBlocks, decompressedTexels, [](const typename Traits::BlockType &block, gpufmt::span<typename DecompressedAltTraits::BlockType> texels) {
                internal::decompressBC6hBlock_UFloat4(block, gpufmt::span<typename DecompressedAltTraits::BlockType, 16>(texels.data(), 16));
            });
        }
    };

    //BC6H_SFLOAT_BLOCK
    template<>
    class FormatStorage<Format::BC6H_SFLOAT_BLOCK> : public internal::LinearBlockStorage {
    public:
        using Traits = FormatTraits<Format::BC6H_SFLOAT_BLOCK>;
        using DecompressedTraits = FormatTraits<Traits::info.decompressedFormat>;
        using DecompressedAltTraits = FormatTraits<Traits::info.decompressedFormatAlt>;
        static constexpr bool Writeable = Traits::info.writeable;
        static constexpr bool Readable = Traits::info.readable;
        static constexpr bool Decompressible = Traits::info.decompressible;
        
        static void loadBlock(const typename Traits::BlockType &value, gpufmt::span<typename Traits::NarrowSampleType, Traits::BlockTexelCount> samples) noexcept {
            internal::decompressBC6hBlock_SFloat3(value, samples);
        }

        static void loadBlock(const typename Traits::BlockType &value, gpufmt::span<Traits::WideSampleType, Traits::BlockTexelCount> samples) noexcept {
            std::array<Traits::NarrowSampleType, Traits::BlockTexelCount> narrowSamples;
            internal::decompressBC6hBlock_SFloat3(value, narrowSamples);

            std::transform(narrowSamples.cbegin(), narrowSamples.cend(), samples.begin(), [](const Traits::NarrowSampleType &narrowSample) {
                return Traits::WideSampleType{ narrowSample, 1.0f };
            });
        }

        static gpufmt::DecompressError decompress(const Surface<const typename Traits::BlockType> &compressedBlocks, const gpufmt::Surface<typename DecompressedTraits::BlockType> &decompressedTexels) noexcept {
            return internal::decompressBlocks<Format::BC6H_SFLOAT_BLOCK>(compressedBlocks, decompressedTexels, [](const typename Traits::BlockType &block, gpufmt::span<typename DecompressedTraits::BlockType> texels) {
                internal::decompressBC6hBlock_SFloat3(block, gpufmt::span<typename DecompressedTraits::BlockType, 16>(texels.data(), 16));
            });
        }

        static gpufmt::DecompressError decompress(const Surface<const typename Traits::BlockType> &compressedBlocks, const gpufmt::Surface<typename DecompressedAltTraits::BlockType> decompressedTexels) noexcept {
            return internal::decompressBlocks<Format::BC6H_SFLOAT_BLOCK>(compressedBlocks, decompressedTexels, [](const typename Traits::BlockType &block, gpufmt::span<typename DecompressedAltTraits::BlockType> texels) {
                internal::decompressBC6hBlock_SFloat4(block, gpufmt::span<typename DecompressedAltTraits::BlockType, 16>(texels.data(), 16));
            });
        }
    };

    //BC7_UNORM_BLOCK
    template<>
    class FormatStorage<Format::BC7_UNORM_BLOCK> : public internal::LinearBlockStorage, public internal::BC7Decompress<false> {
    public:
        using Traits = FormatTraits<Format::BC7_UNORM_BLOCK>;
        static constexpr bool Writeable = Traits::info.writeable;
        static constexpr bool Readable = Traits::info.readable;
        static constexpr bool Decompressible = Traits::info.decompressible;
        
        static void loadBlock(const typename Traits::BlockType &value, gpufmt::span<typename Traits::NarrowSampleType, Traits::BlockTexelCount> samples) noexcept {
            internal::decompressBC7Block(value, samples);
        }
    };

    //BC7_SRGB_BLOCK
    template<>
    class FormatStorage<Format::BC7_SRGB_BLOCK> : public internal::LinearBlockStorage, public internal::BC7Decompress<true> {
    public:
        using Traits = FormatTraits<Format::BC7_SRGB_BLOCK>;
        static constexpr bool Writeable = Traits::info.writeable;
        static constexpr bool Readable = Traits::info.readable;
        static constexpr bool Decompressible = Traits::info.decompressible;
        
        static void loadBlock(const typename Traits::BlockType &value, gpufmt::span<typename Traits::NarrowSampleType, Traits::BlockTexelCount> samples) noexcept {
            internal::decompressBC7Block(value, samples);
        }
    };

#endif //GF_EXCLUDE_BC_COMPRESSED_FORMATS

#ifndef GF_EXCLUDE_ETC_COMPRESSED_FORMATS
    namespace internal {
        template<bool sRGB>
        class ETC2_R8G8B8_Decompress {
        public:
            static constexpr gpufmt::Format FormatV = FormatConditional<sRGB, Format::ETC2_R8G8B8_SRGB_BLOCK, Format::ETC2_R8G8B8_UNORM_BLOCK>::format;
            using Traits = FormatTraits<FormatV>;
            using DecompressedTraits = FormatTraits<Traits::info.decompressedFormat>;
            using DecompressedTraitsAlt = FormatTraits<Traits::info.decompressedFormatAlt>;

            static DecompressError decompress(const Surface<const typename Traits::BlockType> &compressedSurface, const gpufmt::Surface<glm::u8vec3> &decompressedSurface) noexcept {
                static_assert(std::is_same_v<typename DecompressedTraits::BlockType, glm::u8vec3>);

                return internal::decompressSurface<FormatV>(compressedSurface, decompressedSurface, [](const Surface<const typename Traits::BlockType> &compressedSurface, const gpufmt::Surface<glm::u8vec3> &decompressedSurface) {
                    internal::decompressETC2_RGB(compressedSurface, decompressedSurface);
                });
            }

            static DecompressError decompress(const Surface<const typename Traits::BlockType> &compressedSurface, gpufmt::span<glm::u8vec4> decompressedSurface) noexcept {
                static_assert(std::is_same_v<typename DecompressedTraitsAlt::BlockType, glm::u8vec4>);

                return internal::decompressSurface<FormatV>(compressedSurface, decompressedSurface, [](const Surface<const typename Traits::BlockType> &compressedSurface, const gpufmt::Surface<glm::u8vec4> &decompressedSurface) {
                    internal::decompressETC2_RGB(compressedSurface, decompressedSurface);
                });
            }
        };

        template<bool sRGB>
        class ETC2_R8G8B8A1_Decompress {
        public:
            static constexpr gpufmt::Format FormatV = FormatConditional<sRGB, Format::ETC2_R8G8B8A1_SRGB_BLOCK, Format::ETC2_R8G8B8A1_UNORM_BLOCK>::format;
            using Traits = FormatTraits<FormatV>;
            using DecompressedTraits = FormatTraits<Traits::info.decompressedFormat>;

            static DecompressError decompress(const Surface<const typename Traits::BlockType> &compressedSurface, const gpufmt::Surface<typename DecompressedTraits::BlockType> &decompressedSurface) noexcept {
                return internal::decompressSurface<FormatV>(compressedSurface, decompressedSurface, [](const Surface<const typename Traits::BlockType> &compressedSurface, const gpufmt::Surface<typename DecompressedTraits::BlockType> &decompressedSurface) {
                    internal::decompressETC2_RGB8A1(compressedSurface, decompressedSurface);
                });
            }
        };

        template<bool sRGB>
        class ETC2_R8G8B8A8_Decompress {
        public:
            static constexpr gpufmt::Format FormatV = FormatConditional<sRGB, Format::ETC2_R8G8B8A8_SRGB_BLOCK, Format::ETC2_R8G8B8A8_UNORM_BLOCK>::format;
            using Traits = FormatTraits<FormatV>;
            using DecompressedTraits = FormatTraits<Traits::info.decompressedFormat>;

            static DecompressError decompress(const Surface<const typename Traits::BlockType> &compressedSurface, const gpufmt::Surface<typename DecompressedTraits::BlockType> &decompressedSurface) noexcept {
                return internal::decompressSurface<FormatV>(compressedSurface, decompressedSurface, [](const Surface<const typename Traits::BlockType> &compressedSurface, const gpufmt::Surface<typename DecompressedTraits::BlockType> &decompressedSurface) {
                    internal::decompressETC2_RGBA(compressedSurface, decompressedSurface);
                });
            }
        };
    }

    //ETC2_R8G8B8_UNORM_BLOCK
    template<>
    class FormatStorage<Format::ETC2_R8G8B8_UNORM_BLOCK> : public internal::LinearBlockStorage, public internal::ETC2_R8G8B8_Decompress<false> {
    public:
        using Traits = FormatTraits<Format::ETC2_R8G8B8_UNORM_BLOCK>;
        using DecompressedTraits = FormatTraits<Traits::info.decompressedFormat>;
        using DecompressedAltTraits = FormatTraits<Traits::info.decompressedFormatAlt>;
        static constexpr bool Writeable = Traits::info.writeable;
        static constexpr bool Readable = Traits::info.readable;
        static constexpr bool Decompressible = Traits::info.decompressible;
        
        static void loadBlock(const typename Traits::BlockType &value, gpufmt::span<typename Traits::NarrowSampleType, Traits::BlockTexelCount> samples) noexcept {
            internal::decompressETC2_RGB(value, samples);
        }

        static void loadBlock(const typename Traits::BlockType &value, gpufmt::span<Traits::WideSampleType, Traits::BlockTexelCount> samples) noexcept {
            std::array<Traits::NarrowSampleType, Traits::BlockTexelCount> narrowSamples;
            internal::decompressETC2_RGB(value, narrowSamples);

            std::transform(narrowSamples.cbegin(), narrowSamples.cend(), samples.begin(), [](const Traits::NarrowSampleType &narrowSample) {
                return Traits::WideSampleType{ narrowSample, 1.0f };
            });
        }
    };

    //ETC2_R8G8B8_SRGB_BLOCK
    template<>
    class FormatStorage<Format::ETC2_R8G8B8_SRGB_BLOCK> : public internal::LinearBlockStorage, public internal::ETC2_R8G8B8_Decompress<true> {
    public:
        using Traits = FormatTraits<Format::ETC2_R8G8B8_SRGB_BLOCK>;
        static constexpr bool Writeable = Traits::info.writeable;
        static constexpr bool Readable = Traits::info.readable;
        static constexpr bool Decompressible = Traits::info.decompressible;
        
        static void loadBlock(const typename Traits::BlockType &value, gpufmt::span<typename Traits::NarrowSampleType, Traits::BlockTexelCount> samples) noexcept {
            internal::decompressETC2_RGB(value, samples);
        }

        static void loadBlock(const typename Traits::BlockType &value, gpufmt::span<Traits::WideSampleType, Traits::BlockTexelCount> samples) noexcept {
            std::array<Traits::NarrowSampleType, Traits::BlockTexelCount> narrowSamples;
            internal::decompressETC2_RGB(value, narrowSamples);

            std::transform(narrowSamples.cbegin(), narrowSamples.cend(), samples.begin(), [](const Traits::NarrowSampleType &narrowSample) {
                return Traits::WideSampleType{ narrowSample, 1.0f };
            });
        }
    };

    //ETC2_R8G8B8A1_UNORM_BLOCK
    template<>
    class FormatStorage<Format::ETC2_R8G8B8A1_UNORM_BLOCK> : public internal::LinearBlockStorage, public internal::ETC2_R8G8B8A1_Decompress<false> {
    public:
        using Traits = FormatTraits<Format::ETC2_R8G8B8A1_UNORM_BLOCK>;
        static constexpr bool Writeable = Traits::info.writeable;
        static constexpr bool Readable = Traits::info.readable;
        static constexpr bool Decompressible = Traits::info.decompressible;
        
        static void loadBlock(const typename Traits::BlockType &value, gpufmt::span<typename Traits::NarrowSampleType, Traits::BlockTexelCount> samples) noexcept {
            internal::decompressETC2_RGB8A1(value, samples);
        }
    };

    //ETC2_R8G8B8A1_SRGB_BLOCK
    template<>
    class FormatStorage<Format::ETC2_R8G8B8A1_SRGB_BLOCK> : public internal::LinearBlockStorage, public internal::ETC2_R8G8B8A1_Decompress<true> {
    public:
        using Traits = FormatTraits<Format::ETC2_R8G8B8A1_SRGB_BLOCK>;
        static constexpr bool Writeable = Traits::info.writeable;
        static constexpr bool Readable = Traits::info.readable;
        static constexpr bool Decompressible = Traits::info.decompressible;
        
        static void loadBlock(const typename Traits::BlockType &value, gpufmt::span<typename Traits::NarrowSampleType, Traits::BlockTexelCount> samples) noexcept {
            internal::decompressETC2_RGB8A1(value, samples);
        }
    };

    //ETC2_R8G8B8A8_UNORM_BLOCK
    template<>
    class FormatStorage<Format::ETC2_R8G8B8A8_UNORM_BLOCK> : public internal::LinearBlockStorage, public internal::ETC2_R8G8B8A8_Decompress<false> {
    public:
        using Traits = FormatTraits<Format::ETC2_R8G8B8A8_UNORM_BLOCK>;
        static constexpr bool Writeable = Traits::info.writeable;
        static constexpr bool Readable = Traits::info.readable;
        static constexpr bool Decompressible = Traits::info.decompressible;
        
        static void loadBlock(const typename Traits::BlockType &value, gpufmt::span<typename Traits::NarrowSampleType, Traits::BlockTexelCount> samples) noexcept {
            internal::decompressETC2_RGBA(value, samples);
        }
    };

    //ETC2_R8G8B8A8_SRGB_BLOCK
    template<>
    class FormatStorage<Format::ETC2_R8G8B8A8_SRGB_BLOCK> : public internal::LinearBlockStorage, public internal::ETC2_R8G8B8A8_Decompress<true> {
    public:
        using Traits = FormatTraits<Format::ETC2_R8G8B8A8_SRGB_BLOCK>;
        static constexpr bool Writeable = Traits::info.writeable;
        static constexpr bool Readable = Traits::info.readable;
        static constexpr bool Decompressible = Traits::info.decompressible;
        
        static void loadBlock(const typename Traits::BlockType &value, gpufmt::span<typename Traits::NarrowSampleType, Traits::BlockTexelCount> samples) noexcept {
            internal::decompressETC2_RGBA(value, samples);
        }
    };

    //EAC_R11_UNORM_BLOCK
    template<>
    class FormatStorage<Format::EAC_R11_UNORM_BLOCK> : public internal::LinearBlockStorage {
    public:
        using Traits = FormatTraits<Format::EAC_R11_UNORM_BLOCK>;
        using DecompressedTraits = FormatTraits<Traits::info.decompressedFormat>;
        static constexpr bool Writeable = Traits::info.writeable;
        static constexpr bool Readable = Traits::info.readable;
        static constexpr bool Decompressible = Traits::info.decompressible;
        
        static void loadBlock(const typename Traits::BlockType &value, gpufmt::span<typename Traits::NarrowSampleType, Traits::BlockTexelCount> samples) noexcept {
            internal::decompressEAC_R_UNorm(value, samples);
        }

        static void loadBlock(const typename Traits::BlockType &value, gpufmt::span<Traits::WideSampleType, Traits::BlockTexelCount> samples) noexcept {
            std::array<Traits::NarrowSampleType, Traits::BlockTexelCount> narrowSamples;
            internal::decompressEAC_R_UNorm(value, narrowSamples);

            std::transform(narrowSamples.cbegin(), narrowSamples.cend(), samples.begin(), [](const Traits::NarrowSampleType &narrowSample) {
                return Traits::WideSampleType{ narrowSample, 0.0f, 0.0f, 1.0f };
            });
        }

        static DecompressError decompress(const Surface<const typename Traits::BlockType> &compressedSurface, const gpufmt::Surface<typename DecompressedTraits::BlockType> &decompressedSurface) noexcept {
            return internal::decompressSurface<Format::EAC_R11_UNORM_BLOCK>(compressedSurface, decompressedSurface, [](const Surface<const typename Traits::BlockType> &compressedSurface, const gpufmt::Surface<typename DecompressedTraits::BlockType> &decompressedSurface) {
                internal::decompressEAC_R_UNorm(compressedSurface, decompressedSurface);
            });
        }
    };

    //EAC_R11_SNORM_BLOCK
    template<>
    class FormatStorage<Format::EAC_R11_SNORM_BLOCK> : public internal::LinearBlockStorage {
    public:
        using Traits = FormatTraits<Format::EAC_R11_SNORM_BLOCK>;
        using DecompressedTraits = FormatTraits<Traits::info.decompressedFormat>;
        static constexpr bool Writeable = Traits::info.writeable;
        static constexpr bool Readable = Traits::info.readable;
        static constexpr bool Decompressible = Traits::info.decompressible;
        
        static void loadBlock(const typename Traits::BlockType &value, gpufmt::span<typename Traits::NarrowSampleType, Traits::BlockTexelCount> samples) noexcept {
            internal::decompressEAC_R_SNorm(value, samples);
        }

        static void loadBlock(const typename Traits::BlockType &value, gpufmt::span<Traits::WideSampleType, Traits::BlockTexelCount> samples) noexcept {
            std::array<Traits::NarrowSampleType, Traits::BlockTexelCount> narrowSamples;
            internal::decompressEAC_R_SNorm(value, narrowSamples);

            std::transform(narrowSamples.cbegin(), narrowSamples.cend(), samples.begin(), [](const Traits::NarrowSampleType &narrowSample) {
                return Traits::WideSampleType{ narrowSample, 0.0f, 0.0f, 1.0f };
            });
        }

        static DecompressError decompress(const Surface<const typename Traits::BlockType> &compressedSurface, const gpufmt::Surface<typename DecompressedTraits::BlockType> &decompressedSurface) noexcept {
            return internal::decompressSurface<Format::EAC_R11_SNORM_BLOCK>(compressedSurface, decompressedSurface, [](const Surface<const typename Traits::BlockType> &compressedSurface, const gpufmt::Surface<typename DecompressedTraits::BlockType> &decompressedSurface) {
                internal::decompressEAC_R_SNorm(compressedSurface, decompressedSurface);
            });
        }
    };

    //EAC_R11G11_UNORM_BLOCK
    template<>
    class FormatStorage<Format::EAC_R11G11_UNORM_BLOCK> : public internal::LinearBlockStorage {
    public:
        using Traits = FormatTraits<Format::EAC_R11G11_UNORM_BLOCK>;
        using DecompressedTraits = FormatTraits<Traits::info.decompressedFormat>;
        static constexpr bool Writeable = Traits::info.writeable;
        static constexpr bool Readable = Traits::info.readable;
        static constexpr bool Decompressible = Traits::info.decompressible;
        
        static void loadBlock(const typename Traits::BlockType &value, gpufmt::span<typename Traits::NarrowSampleType, Traits::BlockTexelCount> samples) noexcept {
            internal::decompressEAC_RG_UNorm(value, samples);
        }

        static void loadBlock(const typename Traits::BlockType &value, gpufmt::span<Traits::WideSampleType, Traits::BlockTexelCount> samples) noexcept {
            std::array<Traits::NarrowSampleType, Traits::BlockTexelCount> narrowSamples;
            internal::decompressEAC_RG_UNorm(value, narrowSamples);

            std::transform(narrowSamples.cbegin(), narrowSamples.cend(), samples.begin(), [](const Traits::NarrowSampleType &narrowSample) {
                return Traits::WideSampleType{ narrowSample, 0.0f, 1.0f };
            });
        }

        static DecompressError decompress(const Surface<const typename Traits::BlockType> &compressedSurface, const gpufmt::Surface<typename DecompressedTraits::BlockType> &decompressedSurface) noexcept {
            return internal::decompressSurface<Format::EAC_R11G11_UNORM_BLOCK>(compressedSurface, decompressedSurface, [](const Surface<const typename Traits::BlockType> &compressedSurface, const gpufmt::Surface<typename DecompressedTraits::BlockType> &decompressedSurface) {
                internal::decompressEAC_RG_UNorm(compressedSurface, decompressedSurface);
            });
        }
    };

    //EAC_R11G11_SNORM_BLOCK
    template<>
    class FormatStorage<Format::EAC_R11G11_SNORM_BLOCK> : public internal::LinearBlockStorage {
    public:
        using Traits = FormatTraits<Format::EAC_R11G11_SNORM_BLOCK>;
        using DecompressedTraits = FormatTraits<Traits::info.decompressedFormat>;
        static constexpr bool Writeable = Traits::info.writeable;
        static constexpr bool Readable = Traits::info.readable;
        static constexpr bool Decompressible = Traits::info.decompressible;
        
        static void loadBlock(const typename Traits::BlockType &value, gpufmt::span<typename Traits::NarrowSampleType, Traits::BlockTexelCount> samples) noexcept {
            internal::decompressEAC_RG_SNorm(value, samples);
        }

        static void loadBlock(const typename Traits::BlockType &value, gpufmt::span<Traits::WideSampleType, Traits::BlockTexelCount> samples) noexcept {
            std::array<Traits::NarrowSampleType, Traits::BlockTexelCount> narrowSamples;
            internal::decompressEAC_RG_SNorm(value, narrowSamples);

            std::transform(narrowSamples.cbegin(), narrowSamples.cend(), samples.begin(), [](const Traits::NarrowSampleType &narrowSample) {
                return Traits::WideSampleType{ narrowSample, 0.0f, 1.0f };
            });
        }

        static DecompressError decompress(const Surface<const typename Traits::BlockType> &compressedSurface, const gpufmt::Surface<typename DecompressedTraits::BlockType> &decompressedSurface) noexcept {
            return internal::decompressSurface<Format::EAC_R11G11_SNORM_BLOCK>(compressedSurface, decompressedSurface, [](const Surface<const typename Traits::BlockType> &compressedSurface, const gpufmt::Surface<typename DecompressedTraits::BlockType> &decompressedSurface) {
                internal::decompressEAC_RG_SNorm(compressedSurface, decompressedSurface);
            });
        }
    };

#endif //GF_EXCLUDE_ETC_COMPRESSED_FORMATS

#ifndef GF_EXCLUDE_ASTC_COMPRESSED_FORMATS
    namespace internal {
        template<gpufmt::Format FormatV>
        class BaseASTCFormatStorage : public internal::LinearBlockStorage {
        public:
            using Traits = FormatTraits<FormatV>;
            using DecompressedTraits = FormatTraits<Traits::info.decompressedFormat>;
            static constexpr bool Writeable = Traits::info.writeable;
            static constexpr bool Readable = Traits::info.readable;
            static constexpr bool Decompressible = Traits::info.decompressible;
            
            static void loadBlock(const typename Traits::BlockType &value, gpufmt::span<typename Traits::NarrowSampleType, Traits::BlockTexelCount> samples) noexcept {              
                std::array<glm::u8vec4, Traits::BlockTexelCount> samples8bit;

                internal::decompressASTCBlock(gpufmt::asWritableBytes(samples8bit),
                                              gpufmt::asBytes(value), Traits::info.srgb, Traits::BlockExtent.x, Traits::BlockExtent.y);

                std::transform(samples8bit.cbegin(), samples8bit.cend(), samples.begin(), [](glm::u8vec4 texel) {
                    return internal::unpackUNorm4(texel);
                });
            }

            static DecompressError decompress(const Surface<const typename Traits::BlockType> &compressedSurface, const Surface<typename DecompressedTraits::BlockType> &decompressedSurface) noexcept {
                return internal::decompressBlocks<FormatV>(compressedSurface, decompressedSurface, [](const typename Traits::BlockType &block, const gpufmt::span<typename DecompressedTraits::BlockType> &decompressedTexels) {
                    internal::decompressASTCBlock(gpufmt::asWritableBytes(decompressedTexels), gpufmt::asBytes(block), Traits::info.srgb, Traits::BlockExtent.x, Traits::BlockExtent.y);
                });
            }
        };
    }
    
    //ASTC_4x4_UNORM_BLOCK
    template<>
    class FormatStorage<Format::ASTC_4x4_UNORM_BLOCK> : public internal::BaseASTCFormatStorage<Format::ASTC_4x4_UNORM_BLOCK> {};

    //ASTC_4x4_SRGB_BLOCK
    template<>
    class FormatStorage<Format::ASTC_4x4_SRGB_BLOCK> : public internal::BaseASTCFormatStorage<Format::ASTC_4x4_SRGB_BLOCK> {};

    //ASTC_5x4_UNORM_BLOCK
    template<>
    class FormatStorage<Format::ASTC_5x4_UNORM_BLOCK> : public internal::BaseASTCFormatStorage<Format::ASTC_5x4_UNORM_BLOCK> {};

    //ASTC_5x4_SRGB_BLOCK
    template<>
    class FormatStorage<Format::ASTC_5x4_SRGB_BLOCK> : public internal::BaseASTCFormatStorage<Format::ASTC_5x4_SRGB_BLOCK> {};

    //ASTC_5x5_UNORM_BLOCK
    template<>
    class FormatStorage<Format::ASTC_5x5_UNORM_BLOCK> : public internal::BaseASTCFormatStorage<Format::ASTC_5x5_UNORM_BLOCK> {};

    //ASTC_5x5_SRGB_BLOCK
    template<>
    class FormatStorage<Format::ASTC_5x5_SRGB_BLOCK> : public internal::BaseASTCFormatStorage<Format::ASTC_5x5_SRGB_BLOCK> {};

    //ASTC_6x5_UNORM_BLOCK
    template<>
    class FormatStorage<Format::ASTC_6x5_UNORM_BLOCK> : public internal::BaseASTCFormatStorage<Format::ASTC_6x5_UNORM_BLOCK> {};

    //ASTC_6x5_SRGB_BLOCK
    template<>
    class FormatStorage<Format::ASTC_6x5_SRGB_BLOCK> : public internal::BaseASTCFormatStorage<Format::ASTC_6x5_SRGB_BLOCK> {};

    //ASTC_6x6_UNORM_BLOCK
    template<>
    class FormatStorage<Format::ASTC_6x6_UNORM_BLOCK> : public internal::BaseASTCFormatStorage<Format::ASTC_6x6_UNORM_BLOCK> {};

    //ASTC_6x6_SRGB_BLOCK
    template<>
    class FormatStorage<Format::ASTC_6x6_SRGB_BLOCK> : public internal::BaseASTCFormatStorage<Format::ASTC_6x6_SRGB_BLOCK> {};

    //ASTC_8x5_UNORM_BLOCK
    template<>
    class FormatStorage<Format::ASTC_8x5_UNORM_BLOCK> : public internal::BaseASTCFormatStorage<Format::ASTC_8x5_UNORM_BLOCK> {};

    //ASTC_8x5_SRGB_BLOCK
    template<>
    class FormatStorage<Format::ASTC_8x5_SRGB_BLOCK> : public internal::BaseASTCFormatStorage<Format::ASTC_8x5_SRGB_BLOCK> {};

    //ASTC_8x6_UNORM_BLOCK
    template<>
    class FormatStorage<Format::ASTC_8x6_UNORM_BLOCK> : public internal::BaseASTCFormatStorage<Format::ASTC_8x6_UNORM_BLOCK> {};

    //ASTC_8x6_SRGB_BLOCK
    template<>
    class FormatStorage<Format::ASTC_8x6_SRGB_BLOCK> : public internal::BaseASTCFormatStorage<Format::ASTC_8x6_SRGB_BLOCK> {};

    //ASTC_8x8_UNORM_BLOCK
    template<>
    class FormatStorage<Format::ASTC_8x8_UNORM_BLOCK> : public internal::BaseASTCFormatStorage<Format::ASTC_8x8_UNORM_BLOCK> {};

    //ASTC_8x8_SRGB_BLOCK
    template<>
    class FormatStorage<Format::ASTC_8x8_SRGB_BLOCK> : public internal::BaseASTCFormatStorage<Format::ASTC_8x8_SRGB_BLOCK> {};

    //ASTC_10x5_UNORM_BLOCK
    template<>
    class FormatStorage<Format::ASTC_10x5_UNORM_BLOCK> : public internal::BaseASTCFormatStorage<Format::ASTC_10x5_UNORM_BLOCK> {};

    //ASTC_10x5_SRGB_BLOCK
    template<>
    class FormatStorage<Format::ASTC_10x5_SRGB_BLOCK> : public internal::BaseASTCFormatStorage<Format::ASTC_10x5_SRGB_BLOCK> {};

    //ASTC_10x6_UNORM_BLOCK
    template<>
    class FormatStorage<Format::ASTC_10x6_UNORM_BLOCK> : public internal::BaseASTCFormatStorage<Format::ASTC_10x6_UNORM_BLOCK> {};

    //ASTC_10x6_SRGB_BLOCK
    template<>
    class FormatStorage<Format::ASTC_10x6_SRGB_BLOCK> : public internal::BaseASTCFormatStorage<Format::ASTC_10x6_SRGB_BLOCK> {};

    //ASTC_10x8_UNORM_BLOCK
    template<>
    class FormatStorage<Format::ASTC_10x8_UNORM_BLOCK> : public internal::BaseASTCFormatStorage<Format::ASTC_10x8_UNORM_BLOCK> {};

    //ASTC_10x8_SRGB_BLOCK
    template<>
    class FormatStorage<Format::ASTC_10x8_SRGB_BLOCK> : public internal::BaseASTCFormatStorage<Format::ASTC_10x8_SRGB_BLOCK> {};

    //ASTC_10x10_UNORM_BLOCK
    template<>
    class FormatStorage<Format::ASTC_10x10_UNORM_BLOCK> : public internal::BaseASTCFormatStorage<Format::ASTC_10x10_UNORM_BLOCK> {};

    //ASTC_10x10_SRGB_BLOCK
    template<>
    class FormatStorage<Format::ASTC_10x10_SRGB_BLOCK> : public internal::BaseASTCFormatStorage<Format::ASTC_10x10_SRGB_BLOCK> {};

    //ASTC_12x10_UNORM_BLOCK
    template<>
    class FormatStorage<Format::ASTC_12x10_UNORM_BLOCK> : public internal::BaseASTCFormatStorage<Format::ASTC_12x10_UNORM_BLOCK> {};

    //ASTC_12x10_SRGB_BLOCK
    template<>
    class FormatStorage<Format::ASTC_12x10_SRGB_BLOCK> : public internal::BaseASTCFormatStorage<Format::ASTC_12x10_SRGB_BLOCK> {};

    //ASTC_12x12_UNORM_BLOCK
    template<>
    class FormatStorage<Format::ASTC_12x12_UNORM_BLOCK> : public internal::BaseASTCFormatStorage<Format::ASTC_12x12_UNORM_BLOCK> {};

    //ASTC_12x12_SRGB_BLOCK
    template<>
    class FormatStorage<Format::ASTC_12x12_SRGB_BLOCK> : public internal::BaseASTCFormatStorage<Format::ASTC_12x12_SRGB_BLOCK> {};

#endif //GF_EXCLUDE_ASTC_COMPRESSED_FORMATS

#ifndef GF_EXCLUDE_PVRTC_COMPRESSED_FORMATS
    namespace internal {
        class PVRTCReversedMortonOrderStorage {
        public:
            [[nodiscard]]
            static constexpr size_t getBlockIndex(uint32_t blockX, uint32_t blockY, [[maybe_unused]] uint32_t blockZ, [[maybe_unused]] const gpufmt::Extent &surfaceBlockExtent) noexcept {
                constexpr uint32_t B[4] = { 0x55555555, 0x33333333, 0x0F0F0F0F, 0x00FF00FF };
                constexpr uint32_t S[4] = { 1, 2, 4, 8 };

                // Interleave lower 16 bits of blockX and blockY, so the bits of blockY
                // are in the even positions and bits from y in the odd;
                // blockX and blockY must initially be less than 65536.

                blockY = (blockY | (blockY << S[3])) & B[3];
                blockY = (blockY | (blockY << S[2])) & B[2];
                blockY = (blockY | (blockY << S[1])) & B[1];
                blockY = (blockY | (blockY << S[0])) & B[0];

                blockX = (blockX | (blockX << S[3])) & B[3];
                blockX = (blockX | (blockX << S[2])) & B[2];
                blockX = (blockX | (blockX << S[1])) & B[1];
                blockX = (blockX | (blockX << S[0])) & B[0];

                return blockY | (blockX << 1);
            }
        };
    }

    //PVRTC1_2BPP_UNORM_BLOCK_IMG
    template<>
    class FormatStorage<Format::PVRTC1_2BPP_UNORM_BLOCK_IMG> : public internal::PVRTCReversedMortonOrderStorage {
    public:
        using Traits = FormatTraits<Format::PVRTC1_2BPP_UNORM_BLOCK_IMG>;
        using DecompressedTraits = FormatTraits<Traits::info.decompressedFormat>;
        static constexpr bool Writeable = Traits::info.writeable;
        static constexpr bool Readable = Traits::info.readable;
        static constexpr bool Decompressible = Traits::info.decompressible;

        static DecompressError decompress(const Surface<const typename Traits::BlockType> &compressedSurface, const Surface<typename DecompressedTraits::BlockType> &decompressedSurface) noexcept {
            return internal::decompressSurface<Format::PVRTC1_2BPP_UNORM_BLOCK_IMG>(compressedSurface, decompressedSurface, [](const Surface<const typename Traits::BlockType> &compressedSurface, const Surface<typename DecompressedTraits::BlockType> &decompressedSurface) {
                internal::decompressPVRTCBlock_2BPP(compressedSurface, decompressedSurface);
            });
        }
    };

    //PVRTC1_4BPP_UNORM_BLOCK_IMG
    template<>
    class FormatStorage<Format::PVRTC1_4BPP_UNORM_BLOCK_IMG> : public internal::PVRTCReversedMortonOrderStorage {
    public:
        using Traits = FormatTraits<Format::PVRTC1_4BPP_UNORM_BLOCK_IMG>;
        using DecompressedTraits = FormatTraits<Traits::info.decompressedFormat>;
        static constexpr bool Writeable = Traits::info.writeable;
        static constexpr bool Readable = Traits::info.readable;
        static constexpr bool Decompressible = Traits::info.decompressible;

        static DecompressError decompress(const Surface<const typename Traits::BlockType> &compressedSurface, const Surface<typename DecompressedTraits::BlockType> &decompressedSurface) noexcept {
            return internal::decompressSurface<Format::PVRTC1_4BPP_UNORM_BLOCK_IMG>(compressedSurface, decompressedSurface, [](const Surface<const typename Traits::BlockType> &compressedSurface, const Surface<typename DecompressedTraits::BlockType> &decompressedSurface) {
                internal::decompressPVRTCBlock_4BPP(compressedSurface, decompressedSurface);
            });
        }
    };

    //PVRTC2_2BPP_UNORM_BLOCK_IMG
    template<>
    class FormatStorage<Format::PVRTC2_2BPP_UNORM_BLOCK_IMG> : public internal::PVRTCReversedMortonOrderStorage {
    public:
        using Traits = FormatTraits<Format::PVRTC2_2BPP_UNORM_BLOCK_IMG>;
        using DecompressedTraits = FormatTraits<Traits::info.decompressedFormat>;
        static constexpr bool Writeable = Traits::info.writeable;
        static constexpr bool Readable = Traits::info.readable;
        static constexpr bool Decompressible = Traits::info.decompressible;

        static DecompressError decompress(const Surface<const typename Traits::BlockType> &compressedSurface, const Surface<typename DecompressedTraits::BlockType> &decompressedSurface) noexcept {
            return internal::decompressSurface<Format::PVRTC2_2BPP_UNORM_BLOCK_IMG>(compressedSurface, decompressedSurface, [](const Surface<const typename Traits::BlockType> &compressedSurface, const Surface<typename DecompressedTraits::BlockType> &decompressedSurface) {
                internal::decompressPVRTCBlock_2BPP(compressedSurface, decompressedSurface);
            });
        }
    };

    //PVRTC2_4BPP_UNORM_BLOCK_IMG
    template<>
    class FormatStorage<Format::PVRTC2_4BPP_UNORM_BLOCK_IMG> : public internal::PVRTCReversedMortonOrderStorage {
    public:
        using Traits = FormatTraits<Format::PVRTC2_4BPP_UNORM_BLOCK_IMG>;
        using DecompressedTraits = FormatTraits<Traits::info.decompressedFormat>;
        static constexpr bool Writeable = Traits::info.writeable;
        static constexpr bool Readable = Traits::info.readable;
        static constexpr bool Decompressible = Traits::info.decompressible;

        static DecompressError decompress(const Surface<const typename Traits::BlockType> &compressedSurface, const Surface<typename DecompressedTraits::BlockType> &decompressedSurface) noexcept {
            return internal::decompressSurface<Format::PVRTC2_4BPP_UNORM_BLOCK_IMG>(compressedSurface, decompressedSurface, [](const Surface<const typename Traits::BlockType> &compressedSurface, const Surface<typename DecompressedTraits::BlockType> &decompressedSurface) {
                internal::decompressPVRTCBlock_4BPP(compressedSurface, decompressedSurface);
            });
        }
    };

    //PVRTC1_2BPP_SRGB_BLOCK_IMG
    template<>
    class FormatStorage<Format::PVRTC1_2BPP_SRGB_BLOCK_IMG> : public internal::PVRTCReversedMortonOrderStorage {
    public:
        using Traits = FormatTraits<Format::PVRTC1_2BPP_SRGB_BLOCK_IMG>;
        using DecompressedTraits = FormatTraits<Traits::info.decompressedFormat>;
        static constexpr bool Writeable = Traits::info.writeable;
        static constexpr bool Readable = Traits::info.readable;
        static constexpr bool Decompressible = Traits::info.decompressible;

        static DecompressError decompress(const Surface<const typename Traits::BlockType> &compressedSurface, const Surface<typename DecompressedTraits::BlockType> &decompressedSurface) noexcept {
            return internal::decompressSurface<Format::PVRTC1_2BPP_SRGB_BLOCK_IMG>(compressedSurface, decompressedSurface, [](const Surface<const typename Traits::BlockType> &compressedSurface, const Surface<typename DecompressedTraits::BlockType> &decompressedSurface) {
                internal::decompressPVRTCBlock_2BPP(compressedSurface, decompressedSurface);
            });
        }
    };

    //PVRTC1_4BPP_SRGB_BLOCK_IMG
    template<>
    class FormatStorage<Format::PVRTC1_4BPP_SRGB_BLOCK_IMG> : public internal::PVRTCReversedMortonOrderStorage {
    public:
        using Traits = FormatTraits<Format::PVRTC1_4BPP_SRGB_BLOCK_IMG>;
        using DecompressedTraits = FormatTraits<Traits::info.decompressedFormat>;
        static constexpr bool Writeable = Traits::info.writeable;
        static constexpr bool Readable = Traits::info.readable;
        static constexpr bool Decompressible = Traits::info.decompressible;

        static DecompressError decompress(const Surface<const typename Traits::BlockType> &compressedSurface, const Surface<typename DecompressedTraits::BlockType> &decompressedSurface) noexcept {
            return internal::decompressSurface<Format::PVRTC1_4BPP_SRGB_BLOCK_IMG>(compressedSurface, decompressedSurface, [](const Surface<const typename Traits::BlockType> &compressedSurface, const Surface<typename DecompressedTraits::BlockType> &decompressedSurface) {
                internal::decompressPVRTCBlock_4BPP(compressedSurface, decompressedSurface);
            });
        }
    };

    //PVRTC2_2BPP_SRGB_BLOCK_IMG
    template<>
    class FormatStorage<Format::PVRTC2_2BPP_SRGB_BLOCK_IMG> : public internal::PVRTCReversedMortonOrderStorage {
    public:
        using Traits = FormatTraits<Format::PVRTC2_2BPP_SRGB_BLOCK_IMG>;
        using DecompressedTraits = FormatTraits<Traits::info.decompressedFormat>;
        static constexpr bool Writeable = Traits::info.writeable;
        static constexpr bool Readable = Traits::info.readable;
        static constexpr bool Decompressible = Traits::info.decompressible;

        static DecompressError decompress(const Surface<const typename Traits::BlockType> &compressedSurface, const Surface<typename DecompressedTraits::BlockType> &decompressedSurface) noexcept {
            return internal::decompressSurface<Format::PVRTC2_2BPP_SRGB_BLOCK_IMG>(compressedSurface, decompressedSurface, [](const Surface<const typename Traits::BlockType> &compressedSurface, const Surface<typename DecompressedTraits::BlockType> &decompressedSurface) {
                internal::decompressPVRTCBlock_2BPP(compressedSurface, decompressedSurface);
            });
        }
    };

    //PVRTC2_4BPP_SRGB_BLOCK_IMG
    template<>
    class FormatStorage<Format::PVRTC2_4BPP_SRGB_BLOCK_IMG> : public internal::PVRTCReversedMortonOrderStorage {
    public:
        using Traits = FormatTraits<Format::PVRTC2_4BPP_SRGB_BLOCK_IMG>;
        using DecompressedTraits = FormatTraits<Traits::info.decompressedFormat>;
        static constexpr bool Writeable = Traits::info.writeable;
        static constexpr bool Readable = Traits::info.readable;
        static constexpr bool Decompressible = Traits::info.decompressible;

        static DecompressError decompress(const Surface<const typename Traits::BlockType> &compressedSurface, const Surface<typename DecompressedTraits::BlockType> &decompressedSurface) noexcept {
            return internal::decompressSurface<Format::PVRTC2_4BPP_SRGB_BLOCK_IMG>(compressedSurface, decompressedSurface, [](const Surface<const typename Traits::BlockType> &compressedSurface, const Surface<typename DecompressedTraits::BlockType> &decompressedSurface) {
                internal::decompressPVRTCBlock_4BPP(compressedSurface, decompressedSurface);
            });
        }
    };

#endif //GF_EXCLUDE_PVRTC_COMPRESSED_FORMATS
#endif //GF_EXCLUDE_COMPRESSED_FORMATS
}