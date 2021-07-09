#pragma once

#include <algorithm>
#include <cstdint>
#include <intrin.h>
#include <immintrin.h>
#include <limits>
#include <type_traits>

namespace gpufmt::internal {
    template<class T, size_t BitCount>
    [[nodiscard]]
    constexpr T bitMask() noexcept {
        static_assert(std::is_integral_v<T>);
        static_assert(std::is_unsigned_v<T>);
        static_assert(BitCount <= sizeof(T) * 8u);

        if constexpr(BitCount < (sizeof(T) * 8u)) {
            return ((static_cast<T>(1u)) << (static_cast<T>(BitCount))) - static_cast<T>(1u);
        } else {
            return std::numeric_limits<T>::max();
        }
    }

    template<class T, size_t BitCount>
    [[nodiscard]]
    constexpr T signedMin()noexcept {
        static_assert(std::is_signed_v<T>);
        static_assert(std::is_integral_v<T>);
        static_assert(BitCount <= sizeof(T) * 8);
        static_assert(BitCount > 0);

        using UnsignedT = std::make_unsigned_t<T>;

        if constexpr(BitCount < (sizeof(T) * 8u)) {
            return static_cast<T>(-1) * static_cast<T>((static_cast<UnsignedT>(1u)) << (static_cast<UnsignedT>(BitCount - 1u)));
        } else {
            return std::numeric_limits<T>::lowest();
        }
    }

    template<class T, size_t BitCount>
    [[nodiscard]]
    constexpr T signedMax()noexcept {
        static_assert(std::is_signed_v<T>);
        static_assert(std::is_integral_v<T>);
        static_assert(BitCount <= sizeof(T) * 8);
        static_assert(BitCount > 0);

        using UnsignedT = std::make_unsigned_t<T>;

        if constexpr(BitCount < (sizeof(T) * 8u)) {
            return static_cast<T>(bitMask<UnsignedT, BitCount - 1>());
        } else {
            return std::numeric_limits<T>::max();
        }
    }

    template<class T, size_t BitCount>
    [[nodiscard]]
    constexpr std::make_unsigned_t<T> packSignedInt(T value) noexcept {
        static_assert(std::is_signed_v<T>);
        static_assert(std::is_integral_v<T>);
        static_assert(BitCount <= sizeof(T) * 8);
        static_assert(BitCount > 0);
        
        using UnsignedT = std::make_unsigned_t<T>;
        static constexpr T min = signedMin<T, BitCount>();
        static constexpr T max = signedMax<T, BitCount>();
        static constexpr UnsignedT valueMask = bitMask<UnsignedT, BitCount - 1u>();
        static constexpr UnsignedT signMask = (static_cast<UnsignedT>(1u)) << (static_cast<UnsignedT>(sizeof(UnsignedT) * 8u - 1u));
        static constexpr UnsignedT signOffset = (sizeof(T) * 8u) - BitCount;

        union SignedToUnsigned {
            T signed_;
            UnsignedT unsigned_;
        };
        
        const T clampedValue = std::clamp(value, min, max);
        const SignedToUnsigned packedValue{ clampedValue };
        const UnsignedT sign = packedValue.unsigned_ & signMask;
        return (packedValue.unsigned_ & valueMask) + (sign >> signOffset);
    }

    template<class T, size_t BitCount>
    [[nodiscard]]
    constexpr std::make_signed_t<T> unpackSignedInt(T value) noexcept {
        static_assert(std::is_unsigned_v<T>);
        static_assert(std::is_integral_v<T>);
        static_assert(BitCount <= sizeof(T) * 8u);
        static_assert(BitCount > 0);

        union UnsignedToSigned {
            T unsigned_;
            std::make_signed_t<T> signed_;
        };

        static constexpr T valueMask = bitMask<T, BitCount - 1u>();
        const T sign = static_cast<size_t>(value) & ~valueMask;
        UnsignedToSigned unpackedValue;
        unpackedValue.unsigned_ = (sign > 0) ? (value & valueMask) + (~valueMask) : value;

        return unpackedValue.signed_;
    }

    [[nodiscard]]
    inline uint16_t floatToHalf(float value) noexcept {
        // c++20 std::is_constant_evaluated() can be used to check if the function
        // is being evaluated at compile time. When evaluating at compone time, use
        // the normal function instead.

        // Copied from DirectXMath file DirectXPackedVector.inl
        // https://github.com/microsoft/DirectXMath/blob/master/Inc/DirectXPackedVector.inl
        // Copyright (c) Microsoft Corporation. All rights reserved.
        // Licensed under the MIT License.
#if defined(GF_F16C_INTRINSICS)
        __m128 V1 = _mm_set_ss(value);
        __m128i V2 = _mm_cvtps_ph(V1, 0);
        return static_cast<uint16_t>(_mm_cvtsi128_si32(V2));
#else
        uint32_t Result;

        auto IValue = reinterpret_cast<uint32_t *>(&value)[0];
        uint32_t Sign = (IValue & 0x80000000U) >> 16U;
        IValue = IValue & 0x7FFFFFFFU;      // Hack off the sign

        if(IValue > 0x477FE000U)
        {
            // The number is too large to be represented as a half.  Saturate to infinity.
            if(((IValue & 0x7F800000) == 0x7F800000) && ((IValue & 0x7FFFFF) != 0))
            {
                Result = 0x7FFF; // NAN
            }
            else
            {
                Result = 0x7C00U; // INF
            }
        }
        else if(!IValue)
        {
            Result = 0;
        }
        else
        {
            if(IValue < 0x38800000U)
            {
                // The number is too small to be represented as a normalized half.
                // Convert it to a denormalized value.
                uint32_t Shift = 113U - (IValue >> 23U);
                IValue = (0x800000U | (IValue & 0x7FFFFFU)) >> Shift;
            }
            else
            {
                // Rebias the exponent to represent the value as a normalized half.
                IValue += 0xC8000000U;
            }

            Result = ((IValue + 0x0FFFU + ((IValue >> 13U) & 1U)) >> 13U) & 0x7FFFU;
        }
        return static_cast<uint16_t>(Result | Sign);
#endif
    }

    [[nodiscard]]
    inline float halfToFloat(uint16_t value) noexcept {
        // c++20 std::is_constant_evaluated() can be used to check if the function
        // is being evaluated at compile time. When evaluating at compone time, use
        // the normal function instead.

        // Copied from DirectXMath file DirectXPackedVector.inl
        // https://github.com/microsoft/DirectXMath/blob/master/Inc/DirectXPackedVector.inl
        // Copyright (c) Microsoft Corporation. All rights reserved.
        // Licensed under the MIT License.
#if defined(GF_F16C_INTRINSICS)
        __m128i V1 = _mm_cvtsi32_si128(static_cast<int>(value));
        __m128 V2 = _mm_cvtph_ps(V1);
        return _mm_cvtss_f32(V2);
#else
        auto Mantissa = static_cast<uint32_t>(value & 0x03FF);

        uint32_t Exponent = (value & 0x7C00);
        if(Exponent == 0x7C00) // INF/NAN
        {
            Exponent = 0x8f;
        }
        else if(Exponent != 0)  // The value is normalized
        {
            Exponent = static_cast<uint32_t>((static_cast<int>(value) >> 10) & 0x1F);
        }
        else if(Mantissa != 0)     // The value is denormalized
        {
            // Normalize the value in the resulting float
            Exponent = 1;

            do
            {
                Exponent--;
                Mantissa <<= 1;
            } while((Mantissa & 0x0400) == 0);

            Mantissa &= 0x03FF;
        }
        else                        // The value is zero
        {
            Exponent = static_cast<uint32_t>(-112);
        }

        uint32_t Result =
            ((static_cast<uint32_t>(value) & 0x8000) << 16) // Sign
            | ((Exponent + 112) << 23)                      // Exponent
            | (Mantissa << 13);                             // Mantissa

        return reinterpret_cast<float *>(&Result)[0];
#endif
    }

    template<size_t BitCount>
    [[nodiscard]]
    constexpr float scaleFactorUNorm() noexcept {
        static_assert(BitCount > 0);
        static_assert(BitCount <= 16);

        return static_cast<float>(bitMask<uint64_t, BitCount>());
    }

    template<size_t BitCount>
    [[nodiscard]]
    constexpr float scaleFactorSNorm() noexcept {
        static_assert(BitCount > 0);
        static_assert(BitCount <= 16);

        return static_cast<float>(bitMask<uint64_t, BitCount - 1>());
    }

    template<size_t BitCount>
    [[nodiscard]]
    constexpr float uscaledMax() noexcept {
        static_assert(BitCount > 0);
        static_assert(BitCount <= 64);

        return static_cast<float>(bitMask<uint64_t, BitCount>());
    }

    template<size_t BitCount>
    [[nodiscard]]
    constexpr float sscaledMin() noexcept {
        static_assert(BitCount > 0);
        static_assert(BitCount <= 64);

        return -static_cast<float>(1u << (BitCount - 1u));
    }

    template<size_t BitCount>
    [[nodiscard]]
    constexpr float sscaledMax() noexcept {
        static_assert(BitCount > 0);
        static_assert(BitCount <= 64);

        return static_cast<float>(bitMask<uint64_t, BitCount - 1>());
    }
}