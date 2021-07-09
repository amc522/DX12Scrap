#pragma once

#include <gpufmt/definitions.h>
#include <gpufmt/format.h>
#ifndef GF_EXCLUDE_BC_COMPRESSED_FORMATS
#include <gpufmt/internal/bc.h>
#endif
#include <gpufmt/internal/format_traits_common.h>
#include <gpufmt/visit.h>

#include <glm/vec4.hpp>

namespace gpufmt {
    //UNDEFINED
    template<>
    class FormatTraits<Format::UNDEFINED> : public internal::BaseFormatTraits<void, 0, 0, 0, void, 0> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize = ) 0,
            GF_DI(.blockExtent = ) {0, 0, 0},
            GF_DI(.componentCount = ) 0,
            GF_DI(.narrowSampleByteSize = ) 0,
            GF_DI(.narrowSampleBlockByteSize = ) 0,
            GF_DI(.wideSampleByteSize = ) 0,
            GF_DI(.wideSampleBlockByteSize = ) 0,
            GF_DI(.narrowIsWide = ) true,
            GF_DI(.isSigned = ) false,
            GF_DI(.normalized = ) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled = ) false,
#endif
            GF_DI(.integer = ) false,
            GF_DI(.floatingPoint = ) false,
            GF_DI(.srgb = ) false,
            GF_DI(.packed = ) false,
            GF_DI(.depth = ) false,
            GF_DI(.stencil = ) false,
            GF_DI(.compression = ) CompressionType::None,
            GF_DI(.decompressedFormat = ) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt = ) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis = ) GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::Vulkan,
            GF_DI(.channels = ) Channels::None,
            GF_DI(.redBitMask = ) 0,
            GF_DI(.greenBitMask = ) 0,
            GF_DI(.writeable = ) false,
            GF_DI(.readable = ) false,
            GF_DI(.decompressible = ) false
        };
    };

    //R4G4_UNORM_PACK8
    template<>
    class FormatTraits<Format::R4G4_UNORM_PACK8> : public internal::BaseStandardFormatTraits<uint8_t, float, 2u> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RG,
            GF_DI(.redBitMask    =) { 0b11110000 },
            GF_DI(.greenBitMask  =) { 0b00001111 },
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R4G4B4A4_UNORM_PACK16
    template<>
    class FormatTraits<Format::R4G4B4A4_UNORM_PACK16> : public internal::BaseStandardFormatTraits<uint16_t, float, 4u> {
    public:
        static constexpr FormatInfo info = FormatInfo {
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redBitMask    =) { 0xf000 },
            GF_DI(.greenBitMask  =) { 0x0f00 },
            GF_DI(.blueBitMask   =) { 0x00f0 },
            GF_DI(.alphaBitMask  =) { 0x000f },
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //B4G4R4A4_UNORM_PACK16
    template<>
    class FormatTraits<Format::B4G4R4A4_UNORM_PACK16> : public internal::BaseStandardFormatTraits<uint16_t, float, 4u> {
    public:
        static constexpr FormatInfo info = FormatInfo {
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redBitMask    =) { 0x00f0 },
            GF_DI(.greenBitMask  =) { 0x0f00 },
            GF_DI(.blueBitMask   =) { 0xf000 },
            GF_DI(.alphaBitMask  =) { 0x000f },
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //A4R4G4B4_UNORM_PACK16
    template<>
    class FormatTraits<Format::A4R4G4B4_UNORM_PACK16> : public internal::BaseStandardFormatTraits<uint16_t, float, 4u> {
    public:
        static constexpr FormatInfo info = FormatInfo {
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redBitMask    =) { 0x0f00 },
            GF_DI(.greenBitMask  =) { 0x00f0 },
            GF_DI(.blueBitMask   =) { 0x000f },
            GF_DI(.alphaBitMask  =) { 0xf000 },
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R5G6B5_UNORM_PACK16
    template<>
    class FormatTraits<Format::R5G6B5_UNORM_PACK16> : public internal::BaseStandardFormatTraits<uint16_t, float, 3u> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGB,
            GF_DI(.redBitMask    =) { 0b1111100000000000 },
            GF_DI(.greenBitMask  =) { 0b0000011111100000 },
            GF_DI(.blueBitMask   =) { 0b0000000000011111 },
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //B5G6R5_UNORM_PACK16
    template<>
    class FormatTraits<Format::B5G6R5_UNORM_PACK16> : public internal::BaseStandardFormatTraits<uint16_t, float, 3u> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGB,
            GF_DI(.redBitMask    =) { 0b0000000000011111 },
            GF_DI(.greenBitMask  =) { 0b0000011111100000 },
            GF_DI(.blueBitMask   =) { 0b1111100000000000 },
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R5G5B5A1_UNORM_PACK16
    template<>
    class FormatTraits<Format::R5G5B5A1_UNORM_PACK16> : public internal::BaseStandardFormatTraits<uint16_t, float, 4u> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redBitMask    =) { 0b1111100000000000 },
            GF_DI(.greenBitMask  =) { 0b0000011111000000 },
            GF_DI(.blueBitMask   =) { 0b0000000000111110 },
            GF_DI(.alphaBitMask  =) { 0b0000000000000001 },
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //B5G5R5A1_UNORM_PACK16
    template<>
    class FormatTraits<Format::B5G5R5A1_UNORM_PACK16> : public internal::BaseStandardFormatTraits<uint16_t, float, 4u> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redBitMask    =) { 0b0000000000111110 },
            GF_DI(.greenBitMask  =) { 0b0000011111000000 },
            GF_DI(.blueBitMask   =) { 0b1111100000000000 },
            GF_DI(.alphaBitMask  =) { 0b0000000000000001 },
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //A1R5G5B5_UNORM_PACK16
    template<>
    class FormatTraits<Format::A1R5G5B5_UNORM_PACK16> : public internal::BaseStandardFormatTraits<uint16_t, float, 4u> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redBitMask    =) { 0b0111110000000000 },
            GF_DI(.greenBitMask  =) { 0b0000001111100000 },
            GF_DI(.blueBitMask   =) { 0b0000000000011111 },
            GF_DI(.alphaBitMask  =) { 0b1000000000000000 },
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R8_UNORM
    template<>
    class FormatTraits<Format::R8_UNORM> : public internal::BaseStandardFormatTraits<uint8_t, float, 1> {
    public:
        static constexpr FormatInfo info = FormatInfo {
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::Red,
            GF_DI(.redBitMask =) { 0xff },
            GF_DI(.redIndex =) 0u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R8_SNORM
    template<>
    class FormatTraits<Format::R8_SNORM> : public internal::BaseStandardFormatTraits<int8_t, float, 1> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::Red,
            GF_DI(.redBitMask =) { 0xff },
            GF_DI(.redIndex =) 0u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

#ifndef GF_EXCLUDE_SCALED_FORMATS
    //R8_USCALED
    template<>
    class FormatTraits<Format::R8_USCALED> : public internal::BaseStandardFormatTraits<uint8_t, float, 1> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) true,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::Red,
            GF_DI(.redBitMask =) { 0xff },
            GF_DI(.redIndex =) 0u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R8_SSCALED
    template<>
    class FormatTraits<Format::R8_SSCALED> : public internal::BaseStandardFormatTraits<int8_t, float, 1> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) true,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::Red,
            GF_DI(.redBitMask =) { 0xff },
            GF_DI(.redIndex =) 0u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };
#endif // GF_EXCLUDE_SCALED_FORMATS

    //R8_UINT
    template<>
    class FormatTraits<Format::R8_UINT> : public internal::BaseStandardFormatTraits<uint8_t, uint32_t, 1> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) true,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::Red,
            GF_DI(.redBitMask =) { 0xff },
            GF_DI(.redIndex =) 0u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R8_SINT
    template<>
    class FormatTraits<Format::R8_SINT> : public internal::BaseStandardFormatTraits<int8_t, int32_t, 1> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) true,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::Red,
            GF_DI(.redBitMask =) { 0xff },
            GF_DI(.redIndex =) 0u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R8_SRGB
    template<>
    class FormatTraits<Format::R8_SRGB> : public internal::BaseStandardFormatTraits<uint8_t, float, 1> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) true,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::Red,
            GF_DI(.redBitMask =) { 0xff },
            GF_DI(.redIndex =) 0u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //A8_UNORM
    template<>
    class FormatTraits<Format::A8_UNORM> : public internal::BaseFormatTraits<uint8_t, 1, 1, 1, float, 1> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL,
            GF_DI(.channels =) Channels::Alpha,
            GF_DI(.alphaBitMask =) { 0xff },
            GF_DI(.alphaIndex =) 0u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R8G8_UNORM
    template<>
    class FormatTraits<Format::R8G8_UNORM> : public internal::BaseStandardFormatTraits<glm::u8vec2, float, 2> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RG,
            GF_DI(.redBitMask    =) { 0x00ff },
            GF_DI(.greenBitMask  =) { 0xff00 },
            GF_DI(.redIndex =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R8G8_SNORM
    template<>
    class FormatTraits<Format::R8G8_SNORM> : public internal::BaseStandardFormatTraits<glm::i8vec2, float, 2> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RG,
            GF_DI(.redBitMask    =) { 0x00ff },
            GF_DI(.greenBitMask  =) { 0xff00 },
            GF_DI(.redIndex =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

#ifndef GF_EXCLUDE_SCALED_FORMATS
    //R8G8_USCALED
    template<>
    class FormatTraits<Format::R8G8_USCALED> : public internal::BaseStandardFormatTraits<glm::u8vec2, float, 2> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) true,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RG,
            GF_DI(.redBitMask    =) { 0x00ff },
            GF_DI(.greenBitMask  =) { 0xff00 },
            GF_DI(.redIndex =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R8G8_SSCALED
    template<>
    class FormatTraits<Format::R8G8_SSCALED> : public internal::BaseStandardFormatTraits<glm::i8vec2, float, 2> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) true,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RG,
            GF_DI(.redBitMask    =) { 0x00ff },
            GF_DI(.greenBitMask  =) { 0xff00 },
            GF_DI(.redIndex =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };
#endif // GF_EXCLUDE_SCALED_FORMATS

    //R8G8_UINT
    template<>
    class FormatTraits<Format::R8G8_UINT> : public internal::BaseStandardFormatTraits<glm::u8vec2, uint32_t, 2> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) true,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RG,
            GF_DI(.redBitMask    =) { 0x00ff },
            GF_DI(.greenBitMask  =) { 0xff00 },
            GF_DI(.redIndex =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R8G8_SINT
    template<>
    class FormatTraits<Format::R8G8_SINT> : public internal::BaseStandardFormatTraits<glm::i8vec2, int32_t, 2> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) true,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RG,
            GF_DI(.redBitMask    =) { 0x00ff },
            GF_DI(.greenBitMask  =) { 0xff00 },
            GF_DI(.redIndex =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R8G8_SRGB
    template<>
    class FormatTraits<Format::R8G8_SRGB> : public internal::BaseStandardFormatTraits<glm::u8vec2, float, 2> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) true,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RG,
            GF_DI(.redBitMask    =) { 0x00ff },
            GF_DI(.greenBitMask  =) { 0xff00 },
            GF_DI(.redIndex =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R8G8B8_UNORM
    template<>
    class FormatTraits<Format::R8G8B8_UNORM> : public internal::BaseStandardFormatTraits<glm::u8vec3, float, 3> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGB,
            GF_DI(.redBitMask    =) { 0x0000ff },
            GF_DI(.greenBitMask  =) { 0x00ff00 },
            GF_DI(.blueBitMask   =) { 0xff0000 },
            GF_DI(.redIndex   =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 2u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R8G8B8_SNORM
    template<>
    class FormatTraits<Format::R8G8B8_SNORM> : public internal::BaseStandardFormatTraits<glm::i8vec3, float, 3> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGB,
            GF_DI(.redBitMask    =) { 0x0000ff },
            GF_DI(.greenBitMask  =) { 0x00ff00 },
            GF_DI(.blueBitMask   =) { 0xff0000 },
            GF_DI(.redIndex   =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 2u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

#ifndef GF_EXCLUDE_SCALED_FORMATS
    //R8G8B8_USCALED
    template<>
    class FormatTraits<Format::R8G8B8_USCALED> : public internal::BaseStandardFormatTraits<glm::u8vec3, float, 3> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) true,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGB,
            GF_DI(.redBitMask    =) { 0x0000ff },
            GF_DI(.greenBitMask  =) { 0x00ff00 },
            GF_DI(.blueBitMask   =) { 0xff0000 },
            GF_DI(.redIndex   =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 2u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R8G8B8_SSCALED
    template<>
    class FormatTraits<Format::R8G8B8_SSCALED> : public internal::BaseStandardFormatTraits<glm::i8vec3, float, 3> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) true,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGB,
            GF_DI(.redBitMask    =) { 0x0000ff },
            GF_DI(.greenBitMask  =) { 0x00ff00 },
            GF_DI(.blueBitMask   =) { 0xff0000 },
            GF_DI(.redIndex   =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 2u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };
#endif // GF_EXCLUDE_SCALED_FORMATS

    //R8G8B8_UINT
    template<>
    class FormatTraits<Format::R8G8B8_UINT> : public internal::BaseStandardFormatTraits<glm::u8vec3, uint32_t, 3> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) true,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGB,
            GF_DI(.redBitMask    =) { 0x0000ff },
            GF_DI(.greenBitMask  =) { 0x00ff00 },
            GF_DI(.blueBitMask   =) { 0xff0000 },
            GF_DI(.redIndex   =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 2u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R8G8B8_SINT
    template<>
    class FormatTraits<Format::R8G8B8_SINT> : public internal::BaseStandardFormatTraits<glm::i8vec3, int32_t, 3> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) true,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGB,
            GF_DI(.redBitMask    =) { 0x0000ff },
            GF_DI(.greenBitMask  =) { 0x00ff00 },
            GF_DI(.blueBitMask   =) { 0xff0000 },
            GF_DI(.redIndex   =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 2u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R8G8B8_SRGB
    template<>
    class FormatTraits<Format::R8G8B8_SRGB> : public internal::BaseStandardFormatTraits<glm::u8vec3, float, 3> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) true,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGB,
            GF_DI(.redBitMask    =) { 0x0000ff },
            GF_DI(.greenBitMask  =) { 0x00ff00 },
            GF_DI(.blueBitMask   =) { 0xff0000 },
            GF_DI(.redIndex   =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 2u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //B8G8R8_UNORM
    template<>
    class FormatTraits<Format::B8G8R8_UNORM> : public internal::BaseStandardFormatTraits<glm::u8vec3, float, 3> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGB,
            GF_DI(.redBitMask    =) { 0xff0000 },
            GF_DI(.greenBitMask  =) { 0x00ff00 },
            GF_DI(.blueBitMask   =) { 0x0000ff },
            GF_DI(.redIndex   =) 2u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 0u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //B8G8R8_SNORM
    template<>
    class FormatTraits<Format::B8G8R8_SNORM> : public internal::BaseStandardFormatTraits<glm::i8vec3, float, 3> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGB,
            GF_DI(.redBitMask    =) { 0xff0000 },
            GF_DI(.greenBitMask  =) { 0x00ff00 },
            GF_DI(.blueBitMask   =) { 0x0000ff },
            GF_DI(.redIndex   =) 2u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 0u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

#ifndef GF_EXCLUDE_SCALED_FORMATS
    //B8G8R8_USCALED
    template<>
    class FormatTraits<Format::B8G8R8_USCALED> : public internal::BaseStandardFormatTraits<glm::u8vec3, float, 3> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) true,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGB,
            GF_DI(.redBitMask    =) { 0xff0000 },
            GF_DI(.greenBitMask  =) { 0x00ff00 },
            GF_DI(.blueBitMask   =) { 0x0000ff },
            GF_DI(.redIndex   =) 2u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 0u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //B8G8R8_SSCALED
    template<>
    class FormatTraits<Format::B8G8R8_SSCALED> : public internal::BaseStandardFormatTraits<glm::i8vec3, float, 3> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) true,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGB,
            GF_DI(.redBitMask    =) { 0xff0000 },
            GF_DI(.greenBitMask  =) { 0x00ff00 },
            GF_DI(.blueBitMask   =) { 0x0000ff },
            GF_DI(.redIndex   =) 2u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 0u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };
#endif // GF_EXCLUDE_SCALED_FORMATS

    //B8G8R8_UINT
    template<>
    class FormatTraits<Format::B8G8R8_UINT> : public internal::BaseStandardFormatTraits<glm::u8vec3, uint32_t, 3> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) true,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGB,
            GF_DI(.redBitMask    =) { 0xff0000 },
            GF_DI(.greenBitMask  =) { 0x00ff00 },
            GF_DI(.blueBitMask   =) { 0x0000ff },
            GF_DI(.redIndex   =) 2u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 0u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //B8G8R8_SINT
    template<>
    class FormatTraits<Format::B8G8R8_SINT> : public internal::BaseStandardFormatTraits<glm::i8vec3, int32_t, 3> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) true,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGB,
            GF_DI(.redBitMask    =) { 0xff0000 },
            GF_DI(.greenBitMask  =) { 0x00ff00 },
            GF_DI(.blueBitMask   =) { 0x0000ff },
            GF_DI(.redIndex   =) 2u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 0u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //B8G8R8_SRGB
    template<>
    class FormatTraits<Format::B8G8R8_SRGB> : public internal::BaseStandardFormatTraits<glm::u8vec3, float, 3> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) true,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGB,
            GF_DI(.redBitMask    =) { 0xff0000 },
            GF_DI(.greenBitMask  =) { 0x00ff00 },
            GF_DI(.blueBitMask   =) { 0x0000ff },
            GF_DI(.redIndex   =) 2u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 0u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R8G8B8A8_UNORM
    template<>
    class FormatTraits<Format::R8G8B8A8_UNORM> : public internal::BaseStandardFormatTraits<glm::u8vec4, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redBitMask    =) { 0x000000ff },
            GF_DI(.greenBitMask  =) { 0x0000ff00 },
            GF_DI(.blueBitMask   =) { 0x00ff0000 },
            GF_DI(.alphaBitMask  =) { 0xff000000 },
            GF_DI(.redIndex   =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 2u,
            GF_DI(.alphaIndex =) 3u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R8G8B8A8_SNORM
    template<>
    class FormatTraits<Format::R8G8B8A8_SNORM> : public internal::BaseStandardFormatTraits<glm::i8vec4, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redBitMask    =) { 0x000000ff },
            GF_DI(.greenBitMask  =) { 0x0000ff00 },
            GF_DI(.blueBitMask   =) { 0x00ff0000 },
            GF_DI(.alphaBitMask  =) { 0xff000000 },
            GF_DI(.redIndex   =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 2u,
            GF_DI(.alphaIndex =) 3u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

#ifndef GF_EXCLUDE_SCALED_FORMATS
    //R8G8B8A8_USCALED
    template<>
    class FormatTraits<Format::R8G8B8A8_USCALED> : public internal::BaseStandardFormatTraits<glm::u8vec4, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) true,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redBitMask    =) { 0x000000ff },
            GF_DI(.greenBitMask  =) { 0x0000ff00 },
            GF_DI(.blueBitMask   =) { 0x00ff0000 },
            GF_DI(.alphaBitMask  =) { 0xff000000 },
            GF_DI(.redIndex   =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 2u,
            GF_DI(.alphaIndex =) 3u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R8G8B8A8_SSCALED
    template<>
    class FormatTraits<Format::R8G8B8A8_SSCALED> : public internal::BaseStandardFormatTraits<glm::i8vec4, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) true,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redBitMask    =) { 0x000000ff },
            GF_DI(.greenBitMask  =) { 0x0000ff00 },
            GF_DI(.blueBitMask   =) { 0x00ff0000 },
            GF_DI(.alphaBitMask  =) { 0xff000000 },
            GF_DI(.redIndex   =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 2u,
            GF_DI(.alphaIndex =) 3u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };
#endif // GF_EXCLUDE_SCALED_FORMATS

    //R8G8B8A8_UINT
    template<>
    class FormatTraits<Format::R8G8B8A8_UINT> : public internal::BaseStandardFormatTraits<glm::u8vec4, uint32_t, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) true,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redBitMask    =) { 0x000000ff },
            GF_DI(.greenBitMask  =) { 0x0000ff00 },
            GF_DI(.blueBitMask   =) { 0x00ff0000 },
            GF_DI(.alphaBitMask  =) { 0xff000000 },
            GF_DI(.redIndex   =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 2u,
            GF_DI(.alphaIndex =) 3u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R8G8B8A8_SINT
    template<>
    class FormatTraits<Format::R8G8B8A8_SINT> : public internal::BaseStandardFormatTraits<glm::i8vec4, int32_t, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) true,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redBitMask    =) { 0x000000ff },
            GF_DI(.greenBitMask  =) { 0x0000ff00 },
            GF_DI(.blueBitMask   =) { 0x00ff0000 },
            GF_DI(.alphaBitMask  =) { 0xff000000 },
            GF_DI(.redIndex   =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 2u,
            GF_DI(.alphaIndex =) 3u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R8G8B8A8_SRGB
    template<>
    class FormatTraits<Format::R8G8B8A8_SRGB> : public internal::BaseStandardFormatTraits<glm::u8vec4, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) true,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redBitMask    =) { 0x000000ff },
            GF_DI(.greenBitMask  =) { 0x0000ff00 },
            GF_DI(.blueBitMask   =) { 0x00ff0000 },
            GF_DI(.alphaBitMask  =) { 0xff000000 },
            GF_DI(.redIndex   =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 2u,
            GF_DI(.alphaIndex =) 3u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //B8G8R8A8_UNORM
    template<>
    class FormatTraits<Format::B8G8R8A8_UNORM> : public internal::BaseStandardFormatTraits<glm::u8vec4, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redBitMask    =) { 0x00ff0000 },
            GF_DI(.greenBitMask  =) { 0x0000ff00 },
            GF_DI(.blueBitMask   =) { 0x000000ff },
            GF_DI(.alphaBitMask  =) { 0xff000000 },
            GF_DI(.redIndex   =) 2u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 0u,
            GF_DI(.alphaIndex =) 3u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //B8G8R8A8_SNORM
    template<>
    class FormatTraits<Format::B8G8R8A8_SNORM> : public internal::BaseStandardFormatTraits<glm::i8vec4, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redBitMask    =) { 0x00ff0000 },
            GF_DI(.greenBitMask  =) { 0x0000ff00 },
            GF_DI(.blueBitMask   =) { 0x000000ff },
            GF_DI(.alphaBitMask  =) { 0xff000000 },
            GF_DI(.redIndex   =) 2u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 0u,
            GF_DI(.alphaIndex =) 3u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

#ifndef GF_EXCLUDE_SCALED_FORMATS
    //B8G8R8A8_USCALED
    template<>
    class FormatTraits<Format::B8G8R8A8_USCALED> : public internal::BaseStandardFormatTraits<glm::u8vec4, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) true,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redBitMask    =) { 0x00ff0000 },
            GF_DI(.greenBitMask  =) { 0x0000ff00 },
            GF_DI(.blueBitMask   =) { 0x000000ff },
            GF_DI(.alphaBitMask  =) { 0xff000000 },
            GF_DI(.redIndex   =) 2u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 0u,
            GF_DI(.alphaIndex =) 3u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //B8G8R8A8_SSCALED
    template<>
    class FormatTraits<Format::B8G8R8A8_SSCALED> : public internal::BaseStandardFormatTraits<glm::i8vec4, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) true,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redBitMask    =) { 0x00ff0000 },
            GF_DI(.greenBitMask  =) { 0x0000ff00 },
            GF_DI(.blueBitMask   =) { 0x000000ff },
            GF_DI(.alphaBitMask  =) { 0xff000000 },
            GF_DI(.redIndex   =) 2u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 0u,
            GF_DI(.alphaIndex =) 3u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };
#endif // GF_EXCLUDE_SCALED_FORMATS

    //B8G8R8A8_UINT
    template<>
    class FormatTraits<Format::B8G8R8A8_UINT> : public internal::BaseStandardFormatTraits<glm::u8vec4, uint32_t, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) true,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redBitMask    =) { 0x00ff0000 },
            GF_DI(.greenBitMask  =) { 0x0000ff00 },
            GF_DI(.blueBitMask   =) { 0x000000ff },
            GF_DI(.alphaBitMask  =) { 0xff000000 },
            GF_DI(.redIndex   =) 2u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 0u,
            GF_DI(.alphaIndex =) 3u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //B8G8R8A8_SINT
    template<>
    class FormatTraits<Format::B8G8R8A8_SINT> : public internal::BaseStandardFormatTraits<glm::i8vec4, int32_t, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) true,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redBitMask    =) { 0x00ff0000 },
            GF_DI(.greenBitMask  =) { 0x0000ff00 },
            GF_DI(.blueBitMask   =) { 0x000000ff },
            GF_DI(.alphaBitMask  =) { 0xff000000 },
            GF_DI(.redIndex   =) 2u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 0u,
            GF_DI(.alphaIndex =) 3u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //B8G8R8A8_SRGB
    template<>
    class FormatTraits<Format::B8G8R8A8_SRGB> : public internal::BaseStandardFormatTraits<glm::u8vec4, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) true,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redBitMask    =) { 0x00ff0000 },
            GF_DI(.greenBitMask  =) { 0x0000ff00 },
            GF_DI(.blueBitMask   =) { 0x000000ff },
            GF_DI(.alphaBitMask  =) { 0xff000000 },
            GF_DI(.redIndex   =) 2u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 0u,
            GF_DI(.alphaIndex =) 3u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //B8G8R8X8_UNORM
    template<>
    class FormatTraits<Format::B8G8R8X8_UNORM> : public internal::BaseStandardFormatTraits<glm::u8vec4, float, 3> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL,
            GF_DI(.channels =) Channels::RGB,
            GF_DI(.redBitMask    =) { 0x00ff0000 },
            GF_DI(.greenBitMask  =) { 0x0000ff00 },
            GF_DI(.blueBitMask   =) { 0x000000ff },
            GF_DI(.redIndex   =) 2u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 0u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };
    
    //B8G8R8X8_SRGB
    template<>
    class FormatTraits<Format::B8G8R8X8_SRGB> : public internal::BaseStandardFormatTraits<glm::u8vec4, float, 3> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) true,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL,
            GF_DI(.channels =) Channels::RGB,
            GF_DI(.redBitMask    =) { 0x00ff0000 },
            GF_DI(.greenBitMask  =) { 0x0000ff00 },
            GF_DI(.blueBitMask   =) { 0x000000ff },
            GF_DI(.redIndex   =) 2u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 0u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //A8B8G8R8_UNORM_PACK32
    template<>
    class FormatTraits<Format::A8B8G8R8_UNORM_PACK32> : public internal::BaseStandardFormatTraits<uint32_t, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redBitMask    =) { 0x000000ff },
            GF_DI(.greenBitMask  =) { 0x0000ff00 },
            GF_DI(.blueBitMask   =) { 0x00ff0000 },
            GF_DI(.alphaBitMask  =) { 0xff000000 },
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //A8B8G8R8_SNORM_PACK32
    template<>
    class FormatTraits<Format::A8B8G8R8_SNORM_PACK32> : public internal::BaseStandardFormatTraits<uint32_t, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redBitMask    =) { 0x000000ff },
            GF_DI(.greenBitMask  =) { 0x0000ff00 },
            GF_DI(.blueBitMask   =) { 0x00ff0000 },
            GF_DI(.alphaBitMask  =) { 0xff000000 },
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

#ifndef GF_EXCLUDE_SCALED_FORMATS
    //A8B8G8R8_USCALED_PACK32
    template<>
    class FormatTraits<Format::A8B8G8R8_USCALED_PACK32> : public internal::BaseStandardFormatTraits<uint32_t, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) true,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redBitMask    =) { 0x000000ff },
            GF_DI(.greenBitMask  =) { 0x0000ff00 },
            GF_DI(.blueBitMask   =) { 0x00ff0000 },
            GF_DI(.alphaBitMask  =) { 0xff000000 },
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //A8B8G8R8_SSCALED_PACK32
    template<>
    class FormatTraits<Format::A8B8G8R8_SSCALED_PACK32> : public internal::BaseStandardFormatTraits<uint32_t, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) true,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redBitMask    =) { 0x000000ff },
            GF_DI(.greenBitMask  =) { 0x0000ff00 },
            GF_DI(.blueBitMask   =) { 0x00ff0000 },
            GF_DI(.alphaBitMask  =) { 0xff000000 },
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };
#endif // GF_EXCLUDE_SCALED_FORMATS

    //A8B8G8R8_UINT_PACK32
    template<>
    class FormatTraits<Format::A8B8G8R8_UINT_PACK32> : public internal::BaseStandardFormatTraits<uint32_t, uint32_t, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) true,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redBitMask    =) { 0x000000ff },
            GF_DI(.greenBitMask  =) { 0x0000ff00 },
            GF_DI(.blueBitMask   =) { 0x00ff0000 },
            GF_DI(.alphaBitMask  =) { 0xff000000 },
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //A8B8G8R8_SINT_PACK32
    template<>
    class FormatTraits<Format::A8B8G8R8_SINT_PACK32> : public internal::BaseStandardFormatTraits<uint32_t, int32_t, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) true,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redBitMask    =) { 0x000000ff },
            GF_DI(.greenBitMask  =) { 0x0000ff00 },
            GF_DI(.blueBitMask   =) { 0x00ff0000 },
            GF_DI(.alphaBitMask  =) { 0xff000000 },
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //A8B8G8R8_SRGB_PACK32
    template<>
    class FormatTraits<Format::A8B8G8R8_SRGB_PACK32> : public internal::BaseStandardFormatTraits<uint32_t, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) true,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redBitMask    =) { 0x000000ff },
            GF_DI(.greenBitMask  =) { 0x0000ff00 },
            GF_DI(.blueBitMask   =) { 0x00ff0000 },
            GF_DI(.alphaBitMask  =) { 0xff000000 },
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //A2R10G10B10_UNORM_PACK32
    template<>
    class FormatTraits<Format::A2R10G10B10_UNORM_PACK32> : public internal::BaseStandardFormatTraits<uint32_t, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redBitMask    =) { 0b00111111111100000000000000000000 },
            GF_DI(.greenBitMask  =) { 0b00000000000011111111110000000000 },
            GF_DI(.blueBitMask   =) { 0b00000000000000000000001111111111 },
            GF_DI(.alphaBitMask  =) { 0b11000000000000000000000000000000 },
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //A2R10G10B10_SNORM_PACK32
    template<>
    class FormatTraits<Format::A2R10G10B10_SNORM_PACK32> : public internal::BaseStandardFormatTraits<uint32_t, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redBitMask    =) { 0b00111111111100000000000000000000 },
            GF_DI(.greenBitMask  =) { 0b00000000000011111111110000000000 },
            GF_DI(.blueBitMask   =) { 0b00000000000000000000001111111111 },
            GF_DI(.alphaBitMask  =) { 0b11000000000000000000000000000000 },
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

#ifndef GF_EXCLUDE_SCALED_FORMATS
    //A2R10G10B10_USCALED_PACK32
    template<>
    class FormatTraits<Format::A2R10G10B10_USCALED_PACK32> : public internal::BaseStandardFormatTraits<uint32_t, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) true,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redBitMask    =) { 0b00111111111100000000000000000000 },
            GF_DI(.greenBitMask  =) { 0b00000000000011111111110000000000 },
            GF_DI(.blueBitMask   =) { 0b00000000000000000000001111111111 },
            GF_DI(.alphaBitMask  =) { 0b11000000000000000000000000000000 },
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //A2R10G10B10_SSCALED_PACK32
    template<>
    class FormatTraits<Format::A2R10G10B10_SSCALED_PACK32> : public internal::BaseStandardFormatTraits<uint32_t, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) true,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redBitMask    =) { 0b00111111111100000000000000000000 },
            GF_DI(.greenBitMask  =) { 0b00000000000011111111110000000000 },
            GF_DI(.blueBitMask   =) { 0b00000000000000000000001111111111 },
            GF_DI(.alphaBitMask  =) { 0b11000000000000000000000000000000 },
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };
#endif // GF_EXCLUDE_SCALED_FORMATS

    //A2R10G10B10_UINT_PACK32
    template<>
    class FormatTraits<Format::A2R10G10B10_UINT_PACK32> : public internal::BaseStandardFormatTraits<uint32_t, uint32_t, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) true,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redBitMask    =) { 0b00111111111100000000000000000000 },
            GF_DI(.greenBitMask  =) { 0b00000000000011111111110000000000 },
            GF_DI(.blueBitMask   =) { 0b00000000000000000000001111111111 },
            GF_DI(.alphaBitMask  =) { 0b11000000000000000000000000000000 },
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //A2R10G10B10_SINT_PACK32
    template<>
    class FormatTraits<Format::A2R10G10B10_SINT_PACK32> : public internal::BaseStandardFormatTraits<uint32_t, int32_t, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) true,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redBitMask    =) { 0b00111111111100000000000000000000 },
            GF_DI(.greenBitMask  =) { 0b00000000000011111111110000000000 },
            GF_DI(.blueBitMask   =) { 0b00000000000000000000001111111111 },
            GF_DI(.alphaBitMask  =) { 0b11000000000000000000000000000000 },
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //A2B10G10R10_UNORM_PACK32
    template<>
    class FormatTraits<Format::A2B10G10R10_UNORM_PACK32> : public internal::BaseStandardFormatTraits<uint32_t, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redBitMask    =) { 0b00000000000000000000001111111111 },
            GF_DI(.greenBitMask  =) { 0b00000000000011111111110000000000 },
            GF_DI(.blueBitMask   =) { 0b00111111111100000000000000000000 },
            GF_DI(.alphaBitMask  =) { 0b11000000000000000000000000000000 },
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //A2B10G10R10_SNORM_PACK32
    template<>
    class FormatTraits<Format::A2B10G10R10_SNORM_PACK32> : public internal::BaseStandardFormatTraits<uint32_t, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redBitMask    =) { 0b00000000000000000000001111111111 },
            GF_DI(.greenBitMask  =) { 0b00000000000011111111110000000000 },
            GF_DI(.blueBitMask   =) { 0b00111111111100000000000000000000 },
            GF_DI(.alphaBitMask  =) { 0b11000000000000000000000000000000 },
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

#ifndef GF_EXCLUDE_SCALED_FORMATS
    //A2B10G10R10_USCALED_PACK32
    template<>
    class FormatTraits<Format::A2B10G10R10_USCALED_PACK32> : public internal::BaseStandardFormatTraits<uint32_t, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) true,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redBitMask    =) { 0b00000000000000000000001111111111 },
            GF_DI(.greenBitMask  =) { 0b00000000000011111111110000000000 },
            GF_DI(.blueBitMask   =) { 0b00111111111100000000000000000000 },
            GF_DI(.alphaBitMask  =) { 0b11000000000000000000000000000000 },
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //A2B10G10R10_SSCALED_PACK32
    template<>
    class FormatTraits<Format::A2B10G10R10_SSCALED_PACK32> : public internal::BaseStandardFormatTraits<uint32_t, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) true,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redBitMask    =) { 0b00000000000000000000001111111111 },
            GF_DI(.greenBitMask  =) { 0b00000000000011111111110000000000 },
            GF_DI(.blueBitMask   =) { 0b00111111111100000000000000000000 },
            GF_DI(.alphaBitMask  =) { 0b11000000000000000000000000000000 },
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };
#endif // GF_EXCLUDE_SCALED_FORMATS

    //A2B10G10R10_UINT_PACK32
    template<>
    class FormatTraits<Format::A2B10G10R10_UINT_PACK32> : public internal::BaseStandardFormatTraits<uint32_t, uint32_t, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) true,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redBitMask    =) { 0b00000000000000000000001111111111 },
            GF_DI(.greenBitMask  =) { 0b00000000000011111111110000000000 },
            GF_DI(.blueBitMask   =) { 0b00111111111100000000000000000000 },
            GF_DI(.alphaBitMask  =) { 0b11000000000000000000000000000000 },
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //A2B10G10R10_SINT_PACK32
    template<>
    class FormatTraits<Format::A2B10G10R10_SINT_PACK32> : public internal::BaseStandardFormatTraits<uint32_t, int32_t, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) true,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redBitMask    =) { 0b00000000000000000000001111111111 },
            GF_DI(.greenBitMask  =) { 0b00000000000011111111110000000000 },
            GF_DI(.blueBitMask   =) { 0b00111111111100000000000000000000 },
            GF_DI(.alphaBitMask  =) { 0b11000000000000000000000000000000 },
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R16_UNORM
    template<>
    class FormatTraits<Format::R16_UNORM> : public internal::BaseStandardFormatTraits<uint16_t, float, 1> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::Red,
            GF_DI(.redBitMask =) { 0xffff },            
            GF_DI(.redIndex =) 0u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R16_SNORM
    template<>
    class FormatTraits<Format::R16_SNORM> : public internal::BaseStandardFormatTraits<int16_t, float, 1> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::Red,
            GF_DI(.redBitMask =) { 0xffff },
            GF_DI(.redIndex =) 0u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

#ifndef GF_EXCLUDE_SCALED_FORMATS
    //R16_USCALED
    template<>
    class FormatTraits<Format::R16_USCALED> : public internal::BaseStandardFormatTraits<uint16_t, float, 1> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) true,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::Red,
            GF_DI(.redBitMask =) { 0xffff },
            GF_DI(.redIndex =) 0u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R16_SSCALED
    template<>
    class FormatTraits<Format::R16_SSCALED> : public internal::BaseStandardFormatTraits<int16_t, float, 1> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) true,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::Red,
            GF_DI(.redBitMask =) { 0xffff },
            GF_DI(.redIndex =) 0u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };
#endif // GF_EXCLUDE_SCALED_FORMATS

    //R16_UINT
    template<>
    class FormatTraits<Format::R16_UINT> : public internal::BaseStandardFormatTraits<uint16_t, uint32_t, 1> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) true,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::Red,
            GF_DI(.redBitMask =) { 0xffff },
            GF_DI(.redIndex =) 0u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R16_SINT
    template<>
    class FormatTraits<Format::R16_SINT> : public internal::BaseStandardFormatTraits<int16_t, int32_t, 1> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) true,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::Red,
            GF_DI(.redBitMask =) { 0xffff },
            GF_DI(.redIndex =) 0u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R16_SFLOAT
    template<>
    class FormatTraits<Format::R16_SFLOAT> : public internal::BaseStandardFormatTraits<uint16_t, float, 1> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) true,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::Red,
            GF_DI(.redBitMask =) { 0xffff },
            GF_DI(.redIndex =) 0u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R16G16_UNORM
    template<>
    class FormatTraits<Format::R16G16_UNORM> : public internal::BaseStandardFormatTraits<glm::u16vec2, float, 2> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RG,
            GF_DI(.redBitMask   =) { 0x0000ffff },
            GF_DI(.greenBitMask =) { 0xffff0000 },
            GF_DI(.redIndex =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R16G16_SNORM
    template<>
    class FormatTraits<Format::R16G16_SNORM> : public internal::BaseStandardFormatTraits<glm::i16vec2, float, 2> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RG,
            GF_DI(.redBitMask   =) { 0x0000ffff },
            GF_DI(.greenBitMask =) { 0xffff0000 },
            GF_DI(.redIndex =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

#ifndef GF_EXCLUDE_SCALED_FORMATS
    //R16G16_USCALED
    template<>
    class FormatTraits<Format::R16G16_USCALED> : public internal::BaseStandardFormatTraits<glm::u16vec2, float, 2> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) true,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RG,
            GF_DI(.redBitMask   =) { 0x0000ffff },
            GF_DI(.greenBitMask =) { 0xffff0000 },
            GF_DI(.redIndex =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R16G16_SSCALED
    template<>
    class FormatTraits<Format::R16G16_SSCALED> : public internal::BaseStandardFormatTraits<glm::i16vec2, float, 2> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) true,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RG,
            GF_DI(.redBitMask   =) { 0x0000ffff },
            GF_DI(.greenBitMask =) { 0xffff0000 },
            GF_DI(.redIndex =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };
#endif // GF_EXCLUDE_SCALED_FORMATS

    //R16G16_UINT
    template<>
    class FormatTraits<Format::R16G16_UINT> : public internal::BaseStandardFormatTraits<glm::u16vec2, uint32_t, 2> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) true,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RG,
            GF_DI(.redBitMask   =) { 0x0000ffff },
            GF_DI(.greenBitMask =) { 0xffff0000 },
            GF_DI(.redIndex =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R16G16_SINT
    template<>
    class FormatTraits<Format::R16G16_SINT> : public internal::BaseStandardFormatTraits<glm::i16vec2, int32_t, 2> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) true,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RG,
            GF_DI(.redBitMask   =) { 0x0000ffff },
            GF_DI(.greenBitMask =) { 0xffff0000 },
            GF_DI(.redIndex =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R16G16_SFLOAT
    template<>
    class FormatTraits<Format::R16G16_SFLOAT> : public internal::BaseStandardFormatTraits<glm::u16vec2, float, 2> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) true,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RG,
            GF_DI(.redBitMask   =) { 0x0000ffff },
            GF_DI(.greenBitMask =) { 0xffff0000 },
            GF_DI(.redIndex =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R16G16B16_UNORM
    template<>
    class FormatTraits<Format::R16G16B16_UNORM> : public internal::BaseStandardFormatTraits<glm::u16vec3, float, 3> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGB,
            GF_DI(.redIndex   =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 2u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R16G16B16_SNORM
    template<>
    class FormatTraits<Format::R16G16B16_SNORM> : public internal::BaseStandardFormatTraits<glm::i16vec3, float, 3> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGB,
            GF_DI(.redIndex   =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 2u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

#ifndef GF_EXCLUDE_SCALED_FORMATS
    //R16G16B16_USCALED
    template<>
    class FormatTraits<Format::R16G16B16_USCALED> : public internal::BaseStandardFormatTraits<glm::u16vec3, float, 3> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) true,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGB,
            GF_DI(.redIndex   =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 2u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R16G16B16_SSCALED
    template<>
    class FormatTraits<Format::R16G16B16_SSCALED> : public internal::BaseStandardFormatTraits<glm::i16vec3, float, 3> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) true,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGB,
            GF_DI(.redIndex   =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 2u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };
#endif // GF_EXCLUDE_SCALED_FORMATS

    //R16G16B16_UINT
    template<>
    class FormatTraits<Format::R16G16B16_UINT> : public internal::BaseStandardFormatTraits<glm::u16vec3, uint32_t, 3> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) true,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGB,
            GF_DI(.redIndex   =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 2u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R16G16B16_SINT
    template<>
    class FormatTraits<Format::R16G16B16_SINT> : public internal::BaseStandardFormatTraits<glm::i16vec3, int32_t, 3> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) true,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGB,
            GF_DI(.redIndex   =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 2u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R16G16B16_SFLOAT
    template<>
    class FormatTraits<Format::R16G16B16_SFLOAT> : public internal::BaseStandardFormatTraits<glm::u16vec3, float, 3> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) true,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGB,
            GF_DI(.redIndex   =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 2u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R16G16B16A16_UNORM
    template<>
    class FormatTraits<Format::R16G16B16A16_UNORM> : public internal::BaseStandardFormatTraits<glm::u16vec4, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redIndex   =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 2u,
            GF_DI(.alphaIndex =) 3u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R16G16B16A16_SNORM
    template<>
    class FormatTraits<Format::R16G16B16A16_SNORM> : public internal::BaseStandardFormatTraits<glm::i16vec4, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redIndex   =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 2u,
            GF_DI(.alphaIndex =) 3u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

#ifndef GF_EXCLUDE_SCALED_FORMATS
    //R16G16B16A16_USCALED
    template<>
    class FormatTraits<Format::R16G16B16A16_USCALED> : public internal::BaseStandardFormatTraits<glm::u16vec4, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) true,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redIndex   =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 2u,
            GF_DI(.alphaIndex =) 3u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R16G16B16A16_SSCALED
    template<>
    class FormatTraits<Format::R16G16B16A16_SSCALED> : public internal::BaseStandardFormatTraits<glm::i16vec4, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) true,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redIndex   =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 2u,
            GF_DI(.alphaIndex =) 3u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };
#endif // GF_EXCLUDE_SCALED_FORMATS

    //R16G16B16A16_UINT
    template<>
    class FormatTraits<Format::R16G16B16A16_UINT> : public internal::BaseStandardFormatTraits<glm::u16vec4, uint32_t, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) true,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redIndex   =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 2u,
            GF_DI(.alphaIndex =) 3u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R16G16B16A16_SINT
    template<>
    class FormatTraits<Format::R16G16B16A16_SINT> : public internal::BaseStandardFormatTraits<glm::i16vec4, int32_t, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) true,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redIndex   =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 2u,
            GF_DI(.alphaIndex =) 3u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R16G16B16A16_SFLOAT
    template<>
    class FormatTraits<Format::R16G16B16A16_SFLOAT> : public internal::BaseStandardFormatTraits<glm::u16vec4, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) true,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redIndex   =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 2u,
            GF_DI(.alphaIndex =) 3u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R32_UINT
    template<>
    class FormatTraits<Format::R32_UINT> : public internal::BaseStandardFormatTraits<uint32_t, uint32_t, 1> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) true,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::Red,
            GF_DI(.redBitMask =) { 0xffffffff },
            GF_DI(.redIndex =) 0u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R32_SINT
    template<>
    class FormatTraits<Format::R32_SINT> : public internal::BaseStandardFormatTraits<int32_t, int32_t, 1> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) true,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::Red,
            GF_DI(.redBitMask = ) { 0xffffffff },
            GF_DI(.redIndex =) 0u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R32_SFLOAT
    template<>
    class FormatTraits<Format::R32_SFLOAT> : public internal::BaseStandardFormatTraits<float, float, 1> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) true,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::Red,
            GF_DI(.redBitMask =) { 0xffffffff },
            GF_DI(.redIndex =) 0u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R32G32_UINT
    template<>
    class FormatTraits<Format::R32G32_UINT> : public internal::BaseStandardFormatTraits<glm::u32vec2, uint32_t, 2> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) true,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RG,
            GF_DI(.redIndex =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R32G32_SINT
    template<>
    class FormatTraits<Format::R32G32_SINT> : public internal::BaseStandardFormatTraits<glm::i32vec2, int32_t, 2> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) true,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RG,
            GF_DI(.redIndex =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R32G32_SFLOAT
    template<>
    class FormatTraits<Format::R32G32_SFLOAT> : public internal::BaseStandardFormatTraits<glm::vec2, float, 2> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) true,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RG,
            GF_DI(.redIndex =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R32G32B32_UINT
    template<>
    class FormatTraits<Format::R32G32B32_UINT> : public internal::BaseStandardFormatTraits<glm::u32vec3, uint32_t, 3> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) true,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGB,
            GF_DI(.redIndex   =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 2u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R32G32B32_SINT
    template<>
    class FormatTraits<Format::R32G32B32_SINT> : public internal::BaseStandardFormatTraits<glm::i32vec3, int32_t, 3> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) true,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGB,
            GF_DI(.redIndex   =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 2u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R32G32B32_SFLOAT
    template<>
    class FormatTraits<Format::R32G32B32_SFLOAT> : public internal::BaseStandardFormatTraits<glm::vec3, float, 3> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) true,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGB,
            GF_DI(.redIndex   =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 2u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R32G32B32A32_UINT
    template<>
    class FormatTraits<Format::R32G32B32A32_UINT> : public internal::BaseStandardFormatTraits<glm::u32vec4, uint32_t, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) true,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redIndex   =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 2u,
            GF_DI(.alphaIndex =) 3u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R32G32B32A32_SINT
    template<>
    class FormatTraits<Format::R32G32B32A32_SINT> : public internal::BaseStandardFormatTraits<glm::i32vec4, int32_t, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) true,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redIndex   =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 2u,
            GF_DI(.alphaIndex =) 3u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R32G32B32A32_SFLOAT
    template<>
    class FormatTraits<Format::R32G32B32A32_SFLOAT> : public internal::BaseStandardFormatTraits<glm::vec4, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) true,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redIndex   =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 2u,
            GF_DI(.alphaIndex =) 3u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

#ifndef GF_EXCLUDE_64BIT_FORMATS
    //R64_UINT
    template<>
    class FormatTraits<Format::R64_UINT> : public internal::BaseStandardFormatTraits<uint64_t, uint64_t, 1> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) true,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::Red,
            GF_DI(.redIndex =) 0u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R64_SINT
    template<>
    class FormatTraits<Format::R64_SINT> : public internal::BaseStandardFormatTraits<int64_t, int64_t, 1> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) true,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::Red,
            GF_DI(.redIndex =) 0u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R64_SFLOAT
    template<>
    class FormatTraits<Format::R64_SFLOAT> : public internal::BaseStandardFormatTraits<double, double, 1> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) true,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::Red,
            GF_DI(.redIndex =) 0u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R64G64_UINT
    template<>
    class FormatTraits<Format::R64G64_UINT> : public internal::BaseStandardFormatTraits<glm::u64vec2, uint64_t, 2> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) true,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RG,
            GF_DI(.redIndex =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R64G64_SINT
    template<>
    class FormatTraits<Format::R64G64_SINT> : public internal::BaseStandardFormatTraits<glm::i64vec2, int64_t, 2> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) true,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RG,
            GF_DI(.redIndex =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R64G64_SFLOAT
    template<>
    class FormatTraits<Format::R64G64_SFLOAT> : public internal::BaseStandardFormatTraits<glm::dvec2, double, 2> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) true,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RG,
            GF_DI(.redIndex =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R64G64B64_UINT
    template<>
    class FormatTraits<Format::R64G64B64_UINT> : public internal::BaseStandardFormatTraits<glm::u64vec3, uint64_t, 3> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) true,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGB,
            GF_DI(.redIndex   =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 2u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R64G64B64_SINT
    template<>
    class FormatTraits<Format::R64G64B64_SINT> : public internal::BaseStandardFormatTraits<glm::i64vec3, int64_t, 3> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) true,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGB,
            GF_DI(.redIndex   =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 2u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R64G64B64_SFLOAT
    template<>
    class FormatTraits<Format::R64G64B64_SFLOAT> : public internal::BaseStandardFormatTraits<glm::dvec3, double, 3> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) true,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGB,
            GF_DI(.redIndex   =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 2u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R64G64B64A64_UINT
    template<>
    class FormatTraits<Format::R64G64B64A64_UINT> : public internal::BaseStandardFormatTraits<glm::u64vec4, uint64_t, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) true,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redIndex   =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 2u,
            GF_DI(.alphaIndex =) 3u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R64G64B64A64_SINT
    template<>
    class FormatTraits<Format::R64G64B64A64_SINT> : public internal::BaseStandardFormatTraits<glm::i64vec4, int64_t, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) true,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redIndex   =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 2u,
            GF_DI(.alphaIndex =) 3u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //R64G64B64A64_SFLOAT
    template<>
    class FormatTraits<Format::R64G64B64A64_SFLOAT> : public internal::BaseStandardFormatTraits<glm::dvec4, double, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) true,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.redIndex   =) 0u,
            GF_DI(.greenIndex =) 1u,
            GF_DI(.blueIndex  =) 2u,
            GF_DI(.alphaIndex =) 3u,
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };
#endif // GF_EXCLUDE_64BIT_FORMATS

    //B10G11R11_UFLOAT_PACK32
    template<>
    class FormatTraits<Format::B10G11R11_UFLOAT_PACK32> : public internal::BaseFormatTraits<uint32_t, 1, 1, 1, float, 3> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) true,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGB,
            GF_DI(.redBitMask    =) { 0b00000000000000000000011111111111 },
            GF_DI(.greenBitMask  =) { 0b00000000001111111111100000000000 },
            GF_DI(.blueBitMask   =) { 0b11111111110000000000000000000000 },
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //E5B9G9R9_UFLOAT_PACK32
    template<>
    class FormatTraits<Format::E5B9G9R9_UFLOAT_PACK32> : public internal::BaseFormatTraits<uint32_t, 1, 1, 1, float, 3> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) true,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGB,
            GF_DI(.redBitMask    =) { 0b00000000000000000000000111111111 },
            GF_DI(.greenBitMask  =) { 0b00000000000000111111111000000000 },
            GF_DI(.blueBitMask   =) { 0b00000111111111000000000000000000 },
            GF_DI(.writeable =) true,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) false
        };
    };

    //D16_UNORM
    template<>
    class FormatTraits<Format::D16_UNORM> : public internal::BaseFormatTraits<uint16_t, 1, 1, 1, float, 1> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) true,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::Depth,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) false,
            GF_DI(.decompressible =) false
        };
    };

    //X8_D24_UNORM_PACK32
    template<>
    class FormatTraits<Format::X8_D24_UNORM_PACK32> : public internal::BaseFormatTraits<uint32_t, 1, 1, 1, float, 1> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) true,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::Depth,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) false,
            GF_DI(.decompressible =) false
        };
    };

    //D32_SFLOAT
    template<>
    class FormatTraits<Format::D32_SFLOAT> : public internal::BaseFormatTraits<uint32_t, 1, 1, 1, float, 1> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) true,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) true,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::Depth,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) false,
            GF_DI(.decompressible =) false
        };
    };

    //S8_UINT
    template<>
    class FormatTraits<Format::S8_UINT> : public internal::BaseFormatTraits<uint8_t, 1, 1, 1, uint32_t, 1> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) true,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) true,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::Stencil,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) false,
            GF_DI(.decompressible =) false
        };
    };

    //D16_UNORM_S8_UINT
    template<>
    class FormatTraits<Format::D16_UNORM_S8_UINT> : public internal::BaseFormatTraits<uint32_t, 1, 1, 1, float, 2> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) true,
            GF_DI(.stencil =) true,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::DepthStencil,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) false,
            GF_DI(.decompressible =) false
        };
    };

    //D24_UNORM_S8_UINT
    template<>
    class FormatTraits<Format::D24_UNORM_S8_UINT> : public internal::BaseFormatTraits<uint32_t, 1, 1, 1, float, 2> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) true,
            GF_DI(.stencil =) true,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::DepthStencil,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) false,
            GF_DI(.decompressible =) false
        };
    };

    //D32_SFLOAT_S8_UINT
    template<>
    class FormatTraits<Format::D32_SFLOAT_S8_UINT> : public internal::BaseFormatTraits<uint64_t, 1, 1, 1, float, 2> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) true,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) false,
            GF_DI(.depth =) true,
            GF_DI(.stencil =) true,
            GF_DI(.compression =) CompressionType::None,
            GF_DI(.decompressedFormat =) gpufmt::Format::UNDEFINED,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::DepthStencil,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) false,
            GF_DI(.decompressible =) false
        };
    };

#ifndef GF_EXCLUDE_COMPRESSED_FORMATS
#ifndef GF_EXCLUDE_BC_COMPRESSED_FORMATS
    //BC1_RGB_UNORM_BLOCK
    template<>
    class FormatTraits<Format::BC1_RGB_UNORM_BLOCK> : public internal::BaseFormatTraits<BC1Block, 4, 4, 1, float, 3> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::BC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8_UNORM,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::R8G8B8A8_UNORM,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGB,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //BC1_RGB_SRGB_BLOCK
    template<>
    class FormatTraits<Format::BC1_RGB_SRGB_BLOCK> : public internal::BaseFormatTraits<BC1Block, 4, 4, 1, float, 3> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) true,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::BC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8_SRGB,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::R8G8B8A8_SRGB,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGB,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //BC1_RGBA_UNORM_BLOCK
    template<>
    class FormatTraits<Format::BC1_RGBA_UNORM_BLOCK> : public internal::BaseFormatTraits<BC1Block, 4, 4, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false ,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::BC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_SRGB,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //BC1_RGBA_SRGB_BLOCK
    template<>
    class FormatTraits<Format::BC1_RGBA_SRGB_BLOCK> : public internal::BaseFormatTraits<BC1Block, 4, 4, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) true,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::BC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_SRGB,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //BC2_UNORM_BLOCK
    template<>
    class FormatTraits<Format::BC2_UNORM_BLOCK> : public internal::BaseFormatTraits<BC2Block, 4, 4, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::BC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_UNORM,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //BC2_SRGB_BLOCK
    template<>
    class FormatTraits<Format::BC2_SRGB_BLOCK> : public internal::BaseFormatTraits<BC2Block, 4, 4, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) true,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::BC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_SRGB,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //BC3_UNORM_BLOCK
    template<>
    class FormatTraits<Format::BC3_UNORM_BLOCK> : public internal::BaseFormatTraits<BC3Block, 4, 4, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::BC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_UNORM,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //BC3_SRGB_BLOCK
    template<>
    class FormatTraits<Format::BC3_SRGB_BLOCK> : public internal::BaseFormatTraits<BC3Block, 4, 4, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) true,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::BC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_SRGB,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //BC4_UNORM_BLOCK
    template<>
    class FormatTraits<Format::BC4_UNORM_BLOCK> : public internal::BaseFormatTraits<BC4Block, 4, 4, 1, float, 1> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::BC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8_UNORM,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::Red,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //BC4_SNORM_BLOCK
    template<>
    class FormatTraits<Format::BC4_SNORM_BLOCK> : public internal::BaseFormatTraits<BC4Block, 4, 4, 1, float, 1> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::BC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8_SNORM,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::Red,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //BC5_UNORM_BLOCK
    template<>
    class FormatTraits<Format::BC5_UNORM_BLOCK> : public internal::BaseFormatTraits<BC5Block, 4, 4, 1, float, 2> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::BC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8_UNORM,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RG,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //BC5_SNORM_BLOCK
    template<>
    class FormatTraits<Format::BC5_SNORM_BLOCK> : public internal::BaseFormatTraits<BC5Block, 4, 4, 1, float, 2> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::BC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8_SNORM,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RG,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //BC6H_UFLOAT_BLOCK
    template<>
    class FormatTraits<Format::BC6H_UFLOAT_BLOCK> : public internal::BaseFormatTraits<BC6hBlock, 4, 4, 1, float, 3> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) true,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::BC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R16G16B16_SFLOAT,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::R16G16B16A16_SFLOAT,
            GF_DI(.supportedApis =)  GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGB,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //BC6H_SFLOAT_BLOCK
    template<>
    class FormatTraits<Format::BC6H_SFLOAT_BLOCK> : public internal::BaseFormatTraits<BC6hBlock, 4, 4, 1, float, 3> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) false,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) true,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::BC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R16G16B16_SFLOAT,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::R16G16B16A16_SFLOAT,
            GF_DI(.supportedApis =)  GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGB,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //BC7_UNORM_BLOCK
    template<>
    class FormatTraits<Format::BC7_UNORM_BLOCK> : public internal::BaseFormatTraits<BC7Block, 4, 4, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::BC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_UNORM,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //BC7_SRGB_BLOCK
    template<>
    class FormatTraits<Format::BC7_SRGB_BLOCK> : public internal::BaseFormatTraits<BC7Block, 4, 4, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) true,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::BC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_SRGB,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::DirectX11 | GraphicsApi::DirectX12 | GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

#endif //GF_EXCLUDE_BC_COMPRESSED_FORMATS

#ifndef GF_EXCLUDE_ETC_COMPRESSED_FORMATS
    //ETC2_R8G8B8_UNORM_BLOCK
    template<>
    class FormatTraits<Format::ETC2_R8G8B8_UNORM_BLOCK> : public internal::BaseFormatTraits<uint64_t, 4, 4, 1, float, 3> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::ETC2,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8_UNORM,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::R8G8B8A8_UNORM,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGB,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //ETC2_R8G8B8_SRGB_BLOCK
    template<>
    class FormatTraits<Format::ETC2_R8G8B8_SRGB_BLOCK> : public internal::BaseFormatTraits<uint64_t, 4, 4, 1, float, 3> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) true,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::ETC2,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8_SRGB,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::R8G8B8A8_SRGB,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGB,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //ETC2_R8G8B8A1_UNORM_BLOCK
    template<>
    class FormatTraits<Format::ETC2_R8G8B8A1_UNORM_BLOCK> : public internal::BaseFormatTraits<uint64_t, 4, 4, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::ETC2,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_UNORM,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //ETC2_R8G8B8A1_SRGB_BLOCK
    template<>
    class FormatTraits<Format::ETC2_R8G8B8A1_SRGB_BLOCK> : public internal::BaseFormatTraits<uint64_t, 4, 4, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) true,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::ETC2,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_SRGB,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //ETC2_R8G8B8A8_UNORM_BLOCK
    template<>
    class FormatTraits<Format::ETC2_R8G8B8A8_UNORM_BLOCK> : public internal::BaseFormatTraits<glm::u64vec2, 4, 4, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::ETC2,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_UNORM,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //ETC2_R8G8B8A8_SRGB_BLOCK
    template<>
    class FormatTraits<Format::ETC2_R8G8B8A8_SRGB_BLOCK> : public internal::BaseFormatTraits<glm::u64vec2, 4, 4, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) true,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::ETC2,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_SRGB,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //EAC_R11_UNORM_BLOCK
    template<>
    class FormatTraits<Format::EAC_R11_UNORM_BLOCK> : public internal::BaseFormatTraits<uint64_t, 4, 4, 1, float, 1> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::EAC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R16_UNORM,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::Red,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //EAC_R11_SNORM_BLOCK
    template<>
    class FormatTraits<Format::EAC_R11_SNORM_BLOCK> : public internal::BaseFormatTraits<uint64_t, 4, 4, 1, float, 1> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::EAC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R16_SNORM,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::Red,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //EAC_R11G11_UNORM_BLOCK
    template<>
    class FormatTraits<Format::EAC_R11G11_UNORM_BLOCK> : public internal::BaseFormatTraits<glm::u64vec2, 4, 4, 1, float, 2> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::EAC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R16G16_UNORM,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RG,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //EAC_R11G11_SNORM_BLOCK
    template<>
    class FormatTraits<Format::EAC_R11G11_SNORM_BLOCK> : public internal::BaseFormatTraits<glm::u64vec2, 4, 4, 1, float, 2> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) true,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::EAC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R16G16_SNORM,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =) GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RG,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

#endif //GF_EXCLUDE_ETC_COMPRESSED_FORMATS

#ifndef GF_EXCLUDE_ASTC_COMPRESSED_FORMATS
    //ASTC_4x4_UNORM_BLOCK
    template<>
    class FormatTraits<Format::ASTC_4x4_UNORM_BLOCK> : public internal::BaseFormatTraits<glm::u64vec2, 4, 4, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::ASTC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_UNORM,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //ASTC_4x4_SRGB_BLOCK
    template<>
    class FormatTraits<Format::ASTC_4x4_SRGB_BLOCK> : public internal::BaseFormatTraits<glm::u64vec2, 4, 4, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) true,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::ASTC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_SRGB,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //ASTC_5x4_UNORM_BLOCK
    template<>
    class FormatTraits<Format::ASTC_5x4_UNORM_BLOCK> : public internal::BaseFormatTraits<glm::u64vec2, 5, 4, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::ASTC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_UNORM,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //ASTC_5x4_SRGB_BLOCK
    template<>
    class FormatTraits<Format::ASTC_5x4_SRGB_BLOCK> : public internal::BaseFormatTraits<glm::u64vec2, 5, 4, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) true,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::ASTC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_SRGB,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //ASTC_5x5_UNORM_BLOCK
    template<>
    class FormatTraits<Format::ASTC_5x5_UNORM_BLOCK> : public internal::BaseFormatTraits<glm::u64vec2, 5, 5, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::ASTC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_UNORM,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //ASTC_5x5_SRGB_BLOCK
    template<>
    class FormatTraits<Format::ASTC_5x5_SRGB_BLOCK> : public internal::BaseFormatTraits<glm::u64vec2, 5, 5, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) true,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::ASTC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_SRGB,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //ASTC_6x5_UNORM_BLOCK
    template<>
    class FormatTraits<Format::ASTC_6x5_UNORM_BLOCK> : public internal::BaseFormatTraits<glm::u64vec2, 6, 5, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::ASTC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_UNORM,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //ASTC_6x5_SRGB_BLOCK
    template<>
    class FormatTraits<Format::ASTC_6x5_SRGB_BLOCK> : public internal::BaseFormatTraits<glm::u64vec2, 6, 5, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) true,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::ASTC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_SRGB,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //ASTC_6x6_UNORM_BLOCK
    template<>
    class FormatTraits<Format::ASTC_6x6_UNORM_BLOCK> : public internal::BaseFormatTraits<glm::u64vec2, 6, 6, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::ASTC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_UNORM,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //ASTC_6x6_SRGB_BLOCK
    template<>
    class FormatTraits<Format::ASTC_6x6_SRGB_BLOCK> : public internal::BaseFormatTraits<glm::u64vec2, 6, 6, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) true,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::ASTC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_SRGB,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //ASTC_8x5_UNORM_BLOCK
    template<>
    class FormatTraits<Format::ASTC_8x5_UNORM_BLOCK> : public internal::BaseFormatTraits<glm::u64vec2, 8, 5, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::ASTC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_UNORM,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //ASTC_8x5_SRGB_BLOCK
    template<>
    class FormatTraits<Format::ASTC_8x5_SRGB_BLOCK> : public internal::BaseFormatTraits<glm::u64vec2, 8, 5, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) true,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::ASTC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_SRGB,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //ASTC_8x6_UNORM_BLOCK
    template<>
    class FormatTraits<Format::ASTC_8x6_UNORM_BLOCK> : public internal::BaseFormatTraits<glm::u64vec2, 8, 6, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::ASTC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_UNORM,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //ASTC_8x6_SRGB_BLOCK
    template<>
    class FormatTraits<Format::ASTC_8x6_SRGB_BLOCK> : public internal::BaseFormatTraits<glm::u64vec2, 8, 6, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) true,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::ASTC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_SRGB,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //ASTC_8x8_UNORM_BLOCK
    template<>
    class FormatTraits<Format::ASTC_8x8_UNORM_BLOCK> : public internal::BaseFormatTraits<glm::u64vec2, 8, 8, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::ASTC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_UNORM,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //ASTC_8x8_SRGB_BLOCK
    template<>
    class FormatTraits<Format::ASTC_8x8_SRGB_BLOCK> : public internal::BaseFormatTraits<glm::u64vec2, 8, 8, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) true,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::ASTC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_SRGB,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //ASTC_10x5_UNORM_BLOCK
    template<>
    class FormatTraits<Format::ASTC_10x5_UNORM_BLOCK> : public internal::BaseFormatTraits<glm::u64vec2, 10, 5, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::ASTC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_UNORM,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //ASTC_10x5_SRGB_BLOCK
    template<>
    class FormatTraits<Format::ASTC_10x5_SRGB_BLOCK> : public internal::BaseFormatTraits<glm::u64vec2, 10, 5, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) true,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::ASTC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_SRGB,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //ASTC_10x6_UNORM_BLOCK
    template<>
    class FormatTraits<Format::ASTC_10x6_UNORM_BLOCK> : public internal::BaseFormatTraits<glm::u64vec2, 10, 6, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::ASTC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_UNORM,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //ASTC_10x6_SRGB_BLOCK
    template<>
    class FormatTraits<Format::ASTC_10x6_SRGB_BLOCK> : public internal::BaseFormatTraits<glm::u64vec2, 10, 6, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) true,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::ASTC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_SRGB,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //ASTC_10x8_UNORM_BLOCK
    template<>
    class FormatTraits<Format::ASTC_10x8_UNORM_BLOCK> : public internal::BaseFormatTraits<glm::u64vec2, 10, 8, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::ASTC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_UNORM,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //ASTC_10x8_SRGB_BLOCK
    template<>
    class FormatTraits<Format::ASTC_10x8_SRGB_BLOCK> : public internal::BaseFormatTraits<glm::u64vec2, 10, 8, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) true,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::ASTC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_SRGB,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //ASTC_10x10_UNORM_BLOCK
    template<>
    class FormatTraits<Format::ASTC_10x10_UNORM_BLOCK> : public internal::BaseFormatTraits<glm::u64vec2, 10, 10, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::ASTC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_UNORM,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //ASTC_10x10_SRGB_BLOCK
    template<>
    class FormatTraits<Format::ASTC_10x10_SRGB_BLOCK> : public internal::BaseFormatTraits<glm::u64vec2, 10, 10, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) true,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::ASTC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_SRGB,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //ASTC_12x10_UNORM_BLOCK
    template<>
    class FormatTraits<Format::ASTC_12x10_UNORM_BLOCK> : public internal::BaseFormatTraits<glm::u64vec2, 12, 10, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::ASTC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_UNORM,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //ASTC_12x10_SRGB_BLOCK
    template<>
    class FormatTraits<Format::ASTC_12x10_SRGB_BLOCK> : public internal::BaseFormatTraits<glm::u64vec2, 12, 10, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) true,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::ASTC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_SRGB,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //ASTC_12x12_UNORM_BLOCK
    template<>
    class FormatTraits<Format::ASTC_12x12_UNORM_BLOCK> : public internal::BaseFormatTraits<glm::u64vec2, 12, 12, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::ASTC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_UNORM,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

    //ASTC_12x12_SRGB_BLOCK
    template<>
    class FormatTraits<Format::ASTC_12x12_SRGB_BLOCK> : public internal::BaseFormatTraits<glm::u64vec2, 12, 12, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) true,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::ASTC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_SRGB,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) true,
            GF_DI(.decompressible =) true
        };
    };

#endif //GF_EXCLUDE_ASTC_COMPRESSED_FORMATS

#ifndef GF_EXCLUDE_PVRTC_COMPRESSED_FORMATS
    //PVRTC1_2BPP_UNORM_BLOCK_IMG
    template<>
    class FormatTraits<Format::PVRTC1_2BPP_UNORM_BLOCK_IMG> : public internal::BaseFormatTraits<uint64_t, 8, 4, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::PVRTC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_UNORM,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) false,
            GF_DI(.decompressible =) true
        };
    };

    //PVRTC1_4BPP_UNORM_BLOCK_IMG
    template<>
    class FormatTraits<Format::PVRTC1_4BPP_UNORM_BLOCK_IMG> : public internal::BaseFormatTraits<uint64_t, 4, 4, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::PVRTC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_UNORM,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) false,
            GF_DI(.decompressible =) true
        };
    };

    //PVRTC2_2BPP_UNORM_BLOCK_IMG
    template<>
    class FormatTraits<Format::PVRTC2_2BPP_UNORM_BLOCK_IMG> : public internal::BaseFormatTraits<uint64_t, 8, 4, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::PVRTC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_UNORM,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) false,
            GF_DI(.decompressible =) true
        };
    };

    //PVRTC2_4BPP_UNORM_BLOCK_IMG
    template<>
    class FormatTraits<Format::PVRTC2_4BPP_UNORM_BLOCK_IMG> : public internal::BaseFormatTraits<uint64_t, 4, 4, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) false,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::PVRTC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_UNORM,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) false,
            GF_DI(.decompressible =) true
        };
    };

    //PVRTC1_2BPP_SRGB_BLOCK_IMG
    template<>
    class FormatTraits<Format::PVRTC1_2BPP_SRGB_BLOCK_IMG> : public internal::BaseFormatTraits<uint64_t, 8, 4, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) true,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::PVRTC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_SRGB,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) false,
            GF_DI(.decompressible =) true
        };
    };

    //PVRTC1_4BPP_SRGB_BLOCK_IMG
    template<>
    class FormatTraits<Format::PVRTC1_4BPP_SRGB_BLOCK_IMG> : public internal::BaseFormatTraits<uint64_t, 4, 4, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) true,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::PVRTC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_SRGB,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) false,
            GF_DI(.decompressible =) true
        };
    };

    //PVRTC2_2BPP_SRGB_BLOCK_IMG
    template<>
    class FormatTraits<Format::PVRTC2_2BPP_SRGB_BLOCK_IMG> : public internal::BaseFormatTraits<uint64_t, 8, 4, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) true,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::PVRTC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_SRGB,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) false,
            GF_DI(.decompressible =) true
        };
    };

    //PVRTC2_4BPP_SRGB_BLOCK_IMG
    template<>
    class FormatTraits<Format::PVRTC2_4BPP_SRGB_BLOCK_IMG> : public internal::BaseFormatTraits<uint64_t, 4, 4, 1, float, 4> {
    public:
        static constexpr FormatInfo info = FormatInfo{
            GF_DI(.blockByteSize =) BlockByteSize,
            GF_DI(.blockExtent =) BlockExtent,
            GF_DI(.componentCount =) ComponentCount,
            GF_DI(.narrowSampleByteSize =) NarrowSampleByteSize,
            GF_DI(.narrowSampleBlockByteSize =) NarrowSampleBlockByteSize,
            GF_DI(.wideSampleByteSize =) WideSampleByteSize,
            GF_DI(.wideSampleBlockByteSize =) WideSampleBlockByteSize,
            GF_DI(.narrowIsWide =) NarrowIsWide,
            GF_DI(.isSigned =) false,
            GF_DI(.normalized =) true,
#ifndef GF_EXCLUDE_SCALED_FORMATS            
            GF_DI(.scaled =) false,
#endif
            GF_DI(.integer =) false,
            GF_DI(.floatingPoint =) false,
            GF_DI(.srgb =) true,
            GF_DI(.packed =) true,
            GF_DI(.depth =) false,
            GF_DI(.stencil =) false,
            GF_DI(.compression =) CompressionType::PVRTC,
            GF_DI(.decompressedFormat =) gpufmt::Format::R8G8B8A8_SRGB,
            GF_DI(.decompressedFormatAlt =) gpufmt::Format::UNDEFINED,
            GF_DI(.supportedApis =)  GraphicsApi::OpenGL | GraphicsApi::Vulkan,
            GF_DI(.channels =) Channels::RGBA,
            GF_DI(.writeable =) false,
            GF_DI(.readable = ) false,
            GF_DI(.decompressible =) true
        };
    };

#endif //GF_EXCLUDE_PVRTC_COMPRESSED_FORMATS
#endif //GF_EXCLUDE_COMPRESSED_FORMATS

    template<Format FormatV>
    class FormatLimits {
    public:
        using Traits = FormatTraits<FormatV>;

        [[nodiscard]]
        static constexpr typename Traits::ValueType lowest() noexcept {
            if constexpr(Traits::info.normalized) {
                if constexpr(Traits::info.isSigned) {
                    return -1.0f;
                } else {
                    return 0.0f;
                }
            } else if constexpr(Traits::info.floatingPoint && (sizeof(Traits::BlockType) / Traits::ComponentCount) == 2u) {
                // half
                return -65504.0f;
            } else {
                return std::numeric_limits<Traits::ValueType>::lowest();
            }
        }

        [[nodiscard]]
        static constexpr typename Traits::ValueType max() noexcept {
            if constexpr(Traits::info.normalized) {
                return 1.0f;
            } else if constexpr(Traits::info.floatingPoint && (sizeof(Traits::BlockType) / Traits::ComponentCount) == 2u) {
                // half
                return 65504.0f;
            } else {
                return std::numeric_limits<Traits::ValueType>::max();
            }
        }
    };

    template<>
    class FormatLimits<gpufmt::Format::E5B9G9R9_UFLOAT_PACK32> {
    public:
        using Traits = FormatTraits<gpufmt::Format::E5B9G9R9_UFLOAT_PACK32>;

        [[nodiscard]]
        static constexpr Traits::ValueType lowest() noexcept {
            return 0.0f;
        }

        [[nodiscard]]
        static constexpr Traits::ValueType max() noexcept {
            return 65408.0f;
        }
    };

    namespace internal {
        template<Format FormatValue>
        class GetFormatInfoHelper {
        public:
            [[nodiscard]]
            constexpr const FormatInfo &operator()() const noexcept {
                return FormatTraits<FormatValue>().info;
            }
        };
    }

    [[nodiscard]]
    static constexpr const FormatInfo& formatInfo(Format format) noexcept {
        return visitFormat<internal::GetFormatInfoHelper>(format);
    }
}