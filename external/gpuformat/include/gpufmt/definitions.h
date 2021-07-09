#pragma once

#include <gpufmt/config.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <type_traits>

#if defined(_MSVC_LANG)
#define GF_COMPILER_MSVC
#endif

#if !defined(GF_AVX2) && defined(__AVX2__)
#define GF_AVX2
#endif

#if !defined(GF_F16_INTRINSICS) && (defined(__F16C__) || defined(GF_AVX2))
#   define GF_F16C_INTRINSICS
#endif

//designated initializers guard
#if defined(_HAS_CXX20) && _HAS_CXX20
#   ifndef GF_DI
#       define GF_DI(x) x
#   endif
#else
#   define GF_DI(x)
#endif

#define GPUFORMAT_ENUM_BITWISE_OPERATORS(EnumClassName) \
    [[nodiscard]] constexpr inline EnumClassName operator|(EnumClassName left, EnumClassName right) noexcept { \
        return static_cast<EnumClassName>(static_cast<std::underlying_type_t<EnumClassName>>(left) | static_cast<std::underlying_type<EnumClassName>::type>(right)); \
    } \
    [[nodiscard]] constexpr inline EnumClassName operator&(EnumClassName left, EnumClassName right) noexcept { \
        return static_cast<EnumClassName>(static_cast<std::underlying_type_t<EnumClassName>>(left) & static_cast<std::underlying_type<EnumClassName>::type>(right)); \
    } \
    [[nodiscard]] constexpr inline EnumClassName operator^(EnumClassName left, EnumClassName right) noexcept { \
        return static_cast<EnumClassName>(static_cast<std::underlying_type_t<EnumClassName>>(left) ^ static_cast<std::underlying_type<EnumClassName>::type>(right)); \
    } \
    [[nodiscard]] constexpr inline EnumClassName operator~(EnumClassName val) noexcept { \
        return static_cast<EnumClassName>(~static_cast<std::underlying_type_t<EnumClassName>>(val)); \
    } \
    constexpr inline EnumClassName& operator|=(EnumClassName &left, EnumClassName right) noexcept { \
        left = static_cast<EnumClassName>(static_cast<std::underlying_type_t<EnumClassName>>(left) | static_cast<std::underlying_type<EnumClassName>::type>(right)); \
        return left; \
    } \
    constexpr inline EnumClassName& operator&=(EnumClassName &left, EnumClassName right) noexcept { \
        left = static_cast<EnumClassName>(static_cast<std::underlying_type_t<EnumClassName>>(left) & static_cast<std::underlying_type<EnumClassName>::type>(right)); \
        return left; \
    } \
    constexpr inline EnumClassName& operator^=(EnumClassName &left, EnumClassName right) noexcept { \
        left = static_cast<EnumClassName>(static_cast<std::underlying_type_t<EnumClassName>>(left) ^ static_cast<std::underlying_type<EnumClassName>::type>(right)); \
        return left; \
    }

namespace gpufmt {
    static constexpr Extent MaxTextureExtent = {16535, 16535, 16535};
    
    template<class T>
    struct Surface {
        gpufmt::span<T> blockData;
        gpufmt::Extent extentInBlocks;

        template<class T>
        [[nodiscard]]
        constexpr Surface<T> getAs() const noexcept {
            return { gpufmt::span<T>(reinterpret_cast<T*>(blockData.data()), blockData.size() / sizeof(T)), extentInBlocks };
        }

        [[nodiscard]]
        constexpr uint32_t blockCount() const noexcept {
            return extentInBlocks.x * extentInBlocks.y * extentInBlocks.z;
        }
    };

    enum class GraphicsApi : uint32_t {
        None = 0,
        DirectX11 = 1 << 0,
        DirectX12 = 1 << 1,
        OpenGL = 1 << 2,
        Vulkan = 1 << 3,
    };

    enum class Channels : uint32_t {
        None = 0,
        Red = 1 << 0,
        Green = 1 << 1,
        Blue = 1 << 2,
        Alpha = 1 << 3,
        Depth = 1 << 4,
        Stencil = 1 << 5,
        RG = Red | Green,
        RGB = Red | Green | Blue,
        RGBA = Red | Green | Blue | Alpha,
        DepthStencil = Depth | Stencil
    };

    enum class BlockSampleError {
        None,
        SourceTooSmall,
        DestinationTooSmall,
        DepthStencilUnsupported,
        InvalidFormat,
        BloxelOutOfRange,
    };

    enum class WriteError {
        None,
        FormatNotWriteable,
        DestinationTooSmall
    };

    enum class DecompressError {
        None,
        FormatNotDecompressible,
        SourceTooSmall,
        DestinationTooSmall,
    };

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
}

GPUFORMAT_ENUM_BITWISE_OPERATORS(gpufmt::GraphicsApi)
GPUFORMAT_ENUM_BITWISE_OPERATORS(gpufmt::Channels)