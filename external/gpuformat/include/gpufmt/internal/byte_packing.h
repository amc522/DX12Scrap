#pragma once

#include <gpufmt/internal/packing_common.h>
#include <glm/vec4.hpp>

namespace gpufmt::internal {
    template<class T>
    [[nodiscard]]
    constexpr T packUNorm(float value) noexcept {
        static_assert(std::is_same_v<T, uint8_t> ||
                      std::is_same_v<T, uint16_t>);

        static constexpr float scaleFactor = scaleFactorUNorm<sizeof(T) * 8>();
        return static_cast<T>(std::clamp(value, 0.0f, 1.0f) * scaleFactor);
    }

    template<class T>
    [[nodiscard]]
    constexpr T packSNorm(float value) noexcept {
        static_assert(std::is_same_v<T, int8_t> ||
                      std::is_same_v<T, int16_t>);

        static constexpr float scaleFactor = scaleFactorSNorm<sizeof(T) * 8>();
        return static_cast<T>(std::clamp(value, -1.0f, 1.0f) * scaleFactor);
    }

    template<class T>
    [[nodiscard]]
    constexpr T packUScaled(float value) noexcept {
        static_assert(std::is_same_v<T, uint8_t> ||
                      std::is_same_v<T, uint16_t>);

        static constexpr float min = 0.0f;
        static constexpr float max = uscaledMax<sizeof(T) * 8>();

        return static_cast<T>(std::clamp(value, min, max));
    }

    template<class T>
    [[nodiscard]]
    constexpr T packSScaled(float value) noexcept {
        static_assert(std::is_same_v<T, int8_t> ||
                      std::is_same_v<T, int16_t>);

        static constexpr float min = sscaledMin<sizeof(T) * 8>();
        static constexpr float max = sscaledMax<sizeof(T) * 8>();
        
        return static_cast<T>(std::clamp(value, min, max));
    }

    template<class PackedT>
    [[nodiscard]]
    constexpr PackedT packUInt(uint32_t value) noexcept {
        static_assert(std::is_unsigned_v<PackedT>);

        static constexpr uint32_t min = 0;
        static constexpr uint32_t max = std::numeric_limits<PackedT>::max();

        return static_cast<PackedT>(std::clamp(value, min, max));
    }

    template<class PackedT>
    [[nodiscard]]
    constexpr PackedT packUInt(uint64_t value) noexcept {
        static_assert(std::is_unsigned_v<PackedT>);

        static constexpr uint64_t min = 0;
        static constexpr uint64_t max = std::numeric_limits<PackedT>::max();

        return static_cast<PackedT>(std::clamp(value, min, max));
    }

    template<class PackedT>
    [[nodiscard]]
    constexpr PackedT packSInt(int32_t value) noexcept {
        static_assert(std::is_signed_v<PackedT>);

        static constexpr int32_t min = std::numeric_limits<PackedT>::lowest();
        static constexpr int32_t max = std::numeric_limits<PackedT>::max();

        return static_cast<PackedT>(std::clamp(value, min, max));
    }

    template<class PackedT>
    [[nodiscard]]
    constexpr PackedT packSInt(int64_t value) noexcept {
        static_assert(std::is_signed_v<PackedT>);

        static constexpr int64_t min = std::numeric_limits<PackedT>::lowest();
        static constexpr int64_t max = std::numeric_limits<PackedT>::max();

        return static_cast<PackedT>(std::clamp(value, min, max));
    }

    template<class PackedT, class UnpackedT>
    [[nodiscard]]
    constexpr PackedT packFloat(UnpackedT value) noexcept {
        if constexpr(std::is_same_v<PackedT, uint16_t>) {
            return floatToHalf(value);
        } else if constexpr(std::is_same_v<PackedT, float>) {
            return value;
        } else if constexpr(std::is_same_v<PackedT, double>) {
            return value;
        }
    }

    template<class T>
    [[nodiscard]]
    constexpr float unpackUNorm(T value) noexcept {
        static_assert(std::is_same_v<T, uint8_t> ||
                      std::is_same_v<T, uint16_t>);
        
        static constexpr float scaleFactor = scaleFactorUNorm<sizeof(T) * 8>();
        return static_cast<float>(value) / scaleFactor;
    }

    template<class T>
    [[nodiscard]]
    constexpr glm::vec4 unpackUNorm4(glm::vec<4, T> value) noexcept {
        static_assert(std::is_same_v<T, uint8_t> ||
                      std::is_same_v<T, uint16_t>);

        return glm::vec4{
            unpackUNorm(value.x),
            unpackUNorm(value.y),
            unpackUNorm(value.z),
            unpackUNorm(value.w)
        };
    }

    template<class T>
    [[nodiscard]]
    constexpr float unpackSNorm(T value) noexcept {
        static_assert(std::is_same_v<T, int8_t> ||
                      std::is_same_v<T, int16_t>);

        static constexpr float scaleFactor = 1.0f / scaleFactorUNorm<sizeof(T) * 8>();
        return static_cast<float>(value) * scaleFactor;
    }

    template<class T>
    [[nodiscard]]
    constexpr float unpackUScaled(T value) noexcept {
        static_assert(std::is_unsigned_v<T>);
        static_assert(std::is_same_v<T, uint8_t> ||
                      std::is_same_v<T, uint16_t>);

        return static_cast<float>(value);
    }

    template<class T>
    [[nodiscard]]
    constexpr float unpackSScaled(T value) noexcept {
        static_assert(std::is_signed_v<T>);
        static_assert(std::is_same_v<T, int8_t> ||
                      std::is_same_v<T, int16_t>);

        return static_cast<float>(value);
    }

    template<class PackedT>
    [[nodiscard]]
    constexpr auto unpackUInt(PackedT value) noexcept {
        static_assert(std::is_unsigned_v<PackedT>);
        
        if constexpr(sizeof(PackedT) <= sizeof(uint32_t)) {
            return static_cast<uint32_t>(value);
        } else {
            return static_cast<uint64_t>(value);
        }
    }

    template<class PackedT>
    [[nodiscard]]
    constexpr auto unpackSInt(PackedT value) noexcept {
        static_assert(std::is_signed_v<PackedT>);
        
        if constexpr(sizeof(PackedT) <= sizeof(int32_t)) {
            return static_cast<int32_t>(value);
        } else {
            return static_cast<int64_t>(value);
        }
    }

    template<class PackedT>
    [[nodiscard]]
    constexpr auto unpackFloat(PackedT value) noexcept {
        if constexpr(std::is_same_v<PackedT, uint16_t>) {
            return halfToFloat(value);
        } else if constexpr(std::is_same_v<PackedT, float>) {
            return value;
        } else if constexpr(std::is_same_v<PackedT, double>) {
            return value;
        }
    }
}