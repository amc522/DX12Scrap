#pragma once

#include <gpufmt/config.h>
#include <gpufmt/definitions.h>
#include <gpufmt/format.h>
#include <gpufmt/internal/bit_packing.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace gpufmt {
    namespace internal {
        template<class T, size_t ExtentX, size_t ExtentY, size_t ExtentZ>
        class FormatBlockTraits {
        public:
            static_assert(ExtentX > 0 && ExtentY > 0 && ExtentZ > 0);

            using BlockType = T;
            static constexpr size_t BlockByteSize = sizeof(BlockType);
            static constexpr Extent BlockExtent = { ExtentX, ExtentY, ExtentZ };
            static constexpr size_t BlockTexelCount = BlockExtent.x * BlockExtent.y * BlockExtent.z;
        };

        template<>
        class FormatBlockTraits<void, 0, 0, 0> {
        public:
            using BlockType = void;
            static constexpr size_t BlockByteSize = 0;
            static constexpr Extent BlockExtent = { 0, 0, 0 };
            static constexpr size_t BlockTexelCount = 0;
        };

        template<class T, size_t ComponentCountV>
        class FormatSampleTraits {
        public:
            static_assert(std::is_same_v<T, uint32_t> ||
                          std::is_same_v<T, int32_t> ||
                          std::is_same_v<T, float> ||
                          std::is_same_v<T, uint64_t> ||
                          std::is_same_v<T, int64_t> ||
                          std::is_same_v<T, double>);

            static_assert(ComponentCountV > 0 && ComponentCountV <= 4);

            using ValueType = T;
            static constexpr size_t ComponentCount = ComponentCountV;

            using NarrowSampleType = glm::vec<ComponentCount, ValueType>;
            static constexpr size_t NarrowSampleByteSize = sizeof(NarrowSampleType);

            using WideSampleType = glm::vec<4, ValueType>;
            static constexpr size_t WideSampleByteSize = sizeof(WideSampleType);

            static constexpr bool NarrowIsWide = std::is_same_v<NarrowSampleType, WideSampleType>;
        };

        template<>
        class FormatSampleTraits<void, 0> {
        public:
            using ValueType = void;
            static constexpr size_t ComponentCount = 0;

            using NarrowSampleType = void;
            static constexpr size_t NarrowSampleByteSize = 0;

            using WideSampleType = void;
            static constexpr size_t WideSampleByteSize = 0;

            static constexpr bool NarrowIsWide = true;
        };

        template<class T>
        class FormatSampleTraits<T, 1> {
        public:
            static_assert(std::is_same_v<T, uint32_t> ||
                          std::is_same_v<T, int32_t> ||
                          std::is_same_v<T, float> ||
                          std::is_same_v<T, uint64_t> ||
                          std::is_same_v<T, int64_t> ||
                          std::is_same_v<T, double>);

            using ValueType = T;
            static constexpr size_t ComponentCount = 1u;

            using NarrowSampleType = ValueType;
            static constexpr size_t NarrowSampleByteSize = sizeof(NarrowSampleType);

            using WideSampleType = glm::vec<4, ValueType>;
            static constexpr size_t WideSampleByteSize = sizeof(WideSampleType);

            static constexpr bool NarrowIsWide = false;
        };

        template<class BlockT, size_t ExtentX, size_t ExtentY, size_t ExtentZ, class SampleT, size_t ComponentCountV>
        class BaseFormatTraits : public FormatBlockTraits<BlockT, ExtentX, ExtentY, ExtentZ>, public FormatSampleTraits<SampleT, ComponentCountV> {
        public:
            static constexpr size_t NarrowSampleBlockByteSize = sizeof(typename BaseFormatTraits::NarrowSampleType) * BaseFormatTraits::BlockTexelCount;
            static constexpr size_t WideSampleBlockByteSize = sizeof(typename BaseFormatTraits::WideSampleType) * BaseFormatTraits::BlockTexelCount;
        };

        template<>
        class BaseFormatTraits<void, 0, 0, 0, void, 0> : public FormatBlockTraits<void, 0, 0, 0>, public FormatSampleTraits<void, 0> {
        public:
            static constexpr size_t NarrowSampleBlockByteSize = 0;
            static constexpr size_t WideSampleBlockByteSize = 0;
        };
    }

    enum class CompressionType {
        None,
        ASTC,
        BC,
        EAC,
        ETC2,
        PVRTC,
    };

    template<class T>
    class ChannelMask {
    public:
        static_assert(std::is_integral_v<T>);
        static_assert(std::is_unsigned_v<T>);

        constexpr ChannelMask(uint32_t mask) noexcept
            : mask{ mask }
            , width(internal::bitCount(mask))
            , offset(internal::leadingZeroes(mask))
        {}

        uint32_t mask;
        uint8_t width;
        uint8_t offset;
    };

    struct FormatInfo {
        uint32_t blockByteSize = 0u;
        Extent blockExtent;
        uint32_t componentCount = 0;
        uint32_t narrowSampleByteSize = 0;
        uint32_t narrowSampleBlockByteSize = 0;
        uint32_t wideSampleByteSize = 0;
        uint32_t wideSampleBlockByteSize = 0;
        bool narrowIsWide = false;
        bool isSigned = false;
        bool normalized = false;
#ifndef GF_EXCLUDE_SCALED_FORMATS
        bool scaled = false;
#endif
        bool integer = false;
        bool floatingPoint = false;
        bool srgb = false;
        bool packed = false;
        bool depth = false;
        bool stencil = false;
        gpufmt::CompressionType compression = gpufmt::CompressionType::None;
        gpufmt::Format decompressedFormat = gpufmt::Format::UNDEFINED;
        gpufmt::Format decompressedFormatAlt = gpufmt::Format::UNDEFINED;
        gpufmt::GraphicsApi supportedApis = gpufmt::GraphicsApi::None;
        gpufmt::Channels channels = gpufmt::Channels::None;
        gpufmt::ChannelMask<uint32_t> redBitMask{ 0u };
        gpufmt::ChannelMask<uint32_t> greenBitMask{ 0u };
        gpufmt::ChannelMask<uint32_t> blueBitMask{ 0u };
        gpufmt::ChannelMask<uint32_t> alphaBitMask{ 0u };
        uint8_t redIndex{ 255u };
        uint8_t greenIndex{ 255u };
        uint8_t blueIndex{ 255u };
        uint8_t alphaIndex{ 255u };
        bool writeable = false;
        bool readable = false;
        bool decompressible = false;

        [[nodiscard]]
        constexpr bool isCompressed() const noexcept { return compression != gpufmt::CompressionType::None; }
    };

    template<gpufmt::Format>
    class FormatTraits : public internal::BaseFormatTraits<uint8_t, 1, 1, 1, uint32_t, 1> {
    public:
        static constexpr FormatInfo info{};
    };

    namespace internal {
        template<class BlockT, class SampleT, size_t ComponentCountV>
        class BaseStandardFormatTraits : public BaseFormatTraits<BlockT, 1, 1, 1, SampleT, ComponentCountV> {};
    }
}