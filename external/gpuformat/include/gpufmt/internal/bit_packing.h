#pragma once

#include <gpufmt/internal/packing_common.h>

#include <cmath>

namespace gpufmt::internal {
    template<class T, size_t BitCount, size_t Offset>
    [[nodiscard]]
    constexpr T packBits(T value) noexcept {
        static_assert(Offset < sizeof(T) * 8);
        static_assert(BitCount <= sizeof(T) * 8);
        static_assert(BitCount > 0);
        static_assert((BitCount + Offset) <= sizeof(T) * 8);
        static_assert(std::is_unsigned_v<T>);

        static constexpr T mask = bitMask<T, BitCount>();
        return (value & mask) << Offset;
    }

    template<class T, size_t BitCount, size_t Offset>
    [[nodiscard]]
    constexpr T packBitsUNorm(float value) noexcept {
        static_assert(Offset < sizeof(T) * 8);
        static_assert(BitCount <= sizeof(T) * 8);
        static_assert(BitCount > 0);
        static_assert((BitCount + Offset) <= sizeof(T) * 8);
        static_assert(std::is_unsigned_v<T>);

        static constexpr float scaleFactor = scaleFactorUNorm<BitCount>();
        static constexpr T mask = bitMask<T, BitCount>();
        return (static_cast<T>(value * scaleFactor) & mask) << Offset;
    }

    template<class T, size_t BitCount, size_t Offset>
    [[nodiscard]]
    constexpr T packBitsSNorm(float value) noexcept {
        static_assert(Offset < sizeof(T) * 8);
        static_assert(BitCount <= sizeof(T) * 8);
        static_assert(BitCount > 0);
        static_assert((BitCount + Offset) <= sizeof(T) * 8);
        static_assert(std::is_unsigned_v<T>);

        using SignedT = std::make_signed_t<T>;
        static constexpr float scaleFactor = scaleFactorSNorm<BitCount>();
        static constexpr T mask = bitMask<T, BitCount>();
        
        const SignedT scaledValue = static_cast<SignedT>(std::round((std::clamp(value, -1.0f, 1.0f)) * scaleFactor));
        return packSignedInt<SignedT, BitCount>(scaledValue) << Offset;
    }

    template<class T, size_t BitCount, size_t Offset>
    [[nodiscard]]
    constexpr T packBitsUScaled(float value) noexcept {
        static_assert(Offset < sizeof(T) * 8);
        static_assert(BitCount <= sizeof(T) * 8);
        static_assert(BitCount > 0);
        static_assert((BitCount + Offset) <= sizeof(T) * 8);
        static_assert(std::is_unsigned_v<T>);

        static constexpr float min = 0.0f;
        static constexpr float max = uscaledMax<BitCount>();

        const float clampedValue = std::clamp(value, min, max);
        return static_cast<T>(clampedValue) << Offset;
    }

    template<class T, size_t BitCount, size_t Offset>
    [[nodiscard]]
    constexpr T packBitsSScaled(float value) noexcept {
        static_assert(Offset < sizeof(T) * 8);
        static_assert(BitCount <= sizeof(T) * 8);
        static_assert(BitCount > 0);
        static_assert((BitCount + Offset) <= sizeof(T) * 8);
        static_assert(std::is_unsigned_v<T>);

        using SignedT = std::make_signed_t<T>;

        return static_cast<T>(packSignedInt<int32_t, BitCount>(static_cast<int32_t>(value)) << Offset);
    }

    template<class T, size_t BitCount, size_t Offset>
    [[nodiscard]]
    constexpr T packBitsUInt(uint32_t value) noexcept {
        static_assert(Offset < sizeof(T) * 8);
        static_assert(BitCount <= sizeof(T) * 8);
        static_assert(BitCount > 0);
        static_assert((BitCount + Offset) <= sizeof(T) * 8);
        static_assert(std::is_unsigned_v<T>);

        static constexpr uint32_t min = 0;
        static constexpr uint32_t max = (BitCount < (sizeof(T) * 8)) ? ((1 << BitCount) - 1) : std::numeric_limits<T>::max();

        return static_cast<T>(std::clamp(value, min, max)) << Offset;
    }

    template<class T, size_t BitCount, size_t Offset>
    [[nodiscard]]
    constexpr T packBitsSInt(int32_t value) noexcept {
        static_assert(Offset < sizeof(T) * 8);
        static_assert(BitCount <= sizeof(T) * 8);
        static_assert(BitCount > 0);
        static_assert((BitCount + Offset) <= sizeof(T) * 8);
        static_assert(std::is_unsigned_v<T>);

        return static_cast<T>(packSignedInt<int32_t, BitCount>(value) << Offset);
    }

    template<class T, size_t BitCount, size_t Offset>
    [[nodiscard]]
    constexpr T unpackBits(T value) noexcept {
        static_assert(Offset < sizeof(T) * 8);
        static_assert(BitCount <= sizeof(T) * 8);
        static_assert(BitCount > 0);
        static_assert((BitCount + Offset) <= sizeof(T) * 8);
        static_assert(std::is_unsigned_v<T>);

        static constexpr T mask = bitMask<T, BitCount>();
        return (value >> Offset) & mask;
    }

    template<class T, size_t BitCount, size_t Offset>
    [[nodiscard]]
    constexpr float unpackBitsUNorm(T value) noexcept {
        static_assert(std::is_unsigned_v<T>);
        static_assert(BitCount <= sizeof(T) * 8);
        static_assert(BitCount > 0);
        static_assert((BitCount + Offset) <= sizeof(T) * 8);
        static_assert(Offset < sizeof(T) * 8);

        static constexpr float scaleFactor = scaleFactorUNorm<BitCount>();
        static constexpr T mask = bitMask<T, BitCount>();
        return ((value >> Offset) & mask) / scaleFactor;
    }

    template<class T, size_t BitCount, size_t Offset>
    [[nodiscard]]
    constexpr float unpackBitsSNorm(T value) noexcept {
        static_assert(std::is_unsigned_v<T>);
        static_assert(BitCount <= sizeof(T) * 8);
        static_assert(BitCount > 0);
        static_assert((BitCount + Offset) <= sizeof(T) * 8);
        static_assert(Offset < sizeof(T) * 8);

        static constexpr float scaleFactor = 1.0f / scaleFactorSNorm<BitCount>();
        static constexpr T mask = bitMask<T, BitCount>();

        return std::clamp(unpackSignedInt<T, BitCount>((value >> Offset) & mask) * scaleFactor, -1.0f, 1.0f);
    }

    template<class T, size_t BitCount, size_t Offset>
    [[nodiscard]]
    constexpr float unpackBitsUScaled(T value) noexcept {
        static_assert(std::is_unsigned_v<T>);
        static_assert(BitCount <= sizeof(T) * 8);
        static_assert(BitCount > 0);
        static_assert((BitCount + Offset) <= sizeof(T) * 8);
        static_assert(Offset < sizeof(T) * 8);

        static constexpr T mask = bitMask<T, BitCount>();
        return static_cast<float>((value >> Offset) & mask);
    }

    template<class T, size_t BitCount, size_t Offset>
    [[nodiscard]]
    constexpr float unpackBitsSScaled(T value) noexcept {
        static_assert(BitCount <= sizeof(T) * 8);
        static_assert(Offset < sizeof(T) * 8);
        static_assert(BitCount > 0);
        static_assert((BitCount + Offset) <= sizeof(T) * 8);
        static_assert(std::is_unsigned_v<T>);

        static constexpr T mask = bitMask<T, BitCount>();

        return static_cast<float>(unpackSignedInt<T, BitCount>((value >> Offset) & mask));
    }

    template<class T, size_t BitCount, size_t Offset>
    [[nodiscard]]
    constexpr uint32_t unpackBitsUInt(T value) noexcept {
        static_assert(std::is_unsigned_v<T>);
        static_assert(BitCount > 0);
        static_assert((BitCount + Offset) <= sizeof(T) * 8);
        static_assert(BitCount <= sizeof(T) * 8);
        static_assert(Offset < sizeof(T) * 8);

        static constexpr T mask = bitMask<T, BitCount>();

        return static_cast<uint32_t>(value >> Offset) & mask;
    }

    template<class T, size_t BitCount, size_t Offset>
    [[nodiscard]]
    constexpr int32_t unpackBitsSInt(T value) noexcept {
        static_assert(BitCount <= sizeof(T) * 8);
        static_assert(Offset < sizeof(T) * 8);
        static_assert(BitCount > 0);
        static_assert((BitCount + Offset) <= sizeof(T) * 8);
        static_assert(std::is_unsigned_v<T>);

        static constexpr T mask = bitMask<T, BitCount>();
        
        return unpackSignedInt<T, BitCount>((value >> Offset) & mask);
    }

    template<class T>
    [[nodiscard]]
    constexpr uint8_t bitCount(T value) noexcept {
        uint8_t bits = 0;
        for(int i = 0; i < sizeof(T) * 8; ++i) {
            bits += (value & 1); value >>= 1;
        }

        return bits;
    }

    template<class T>
    [[nodiscard]]
    constexpr uint16_t bitCount(T value0, T value1, T value2, T value3) noexcept {
        uint16_t bits = 0;

        bits += bitCount(value0);
        bits += bitCount(value1);
        bits += bitCount(value2);
        bits += bitCount(value3);

        return bits;
    }

    template<class T>
    [[nodiscard]]
    constexpr uint8_t leadingZeroes(T value) noexcept {
        uint8_t zeroes = 0;

        while(value > 0 && (value % 2) == 0) {
            ++zeroes;
            value >>= 1;
        }
        return zeroes;
    }

    template<class T>
    [[nodiscard]]
    constexpr uint16_t leadingZeroes(T value0, T value1, T value2, T value3) noexcept {
        uint16_t zeroes = leadingZeroes(value0);
        zeroes += (zeroes % (sizeof(T) * 8) == 0) ? leadingZeroes(value1) : 0u;
        zeroes += (zeroes % (sizeof(T) * 8) == 0) ? leadingZeroes(value2) : 0u;
        zeroes += (zeroes % (sizeof(T) * 8) == 0) ? leadingZeroes(value3) : 0u;

        return zeroes;
    }
}