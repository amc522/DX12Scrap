#pragma once

#include <gpufmt/internal/byte_packing.h>
#include <gpufmt/internal/format_traits_common.h>

namespace gpufmt::internal {
    //----------------------------------
    // BITS
    //----------------------------------
    template<Format F>
    constexpr typename FormatTraits<F>::NarrowSampleType formatUnpackBitsUNorm(typename FormatTraits<F>::BlockType value) noexcept {
        using Traits = FormatTraits<F>;
        static_assert(Traits::info.packed);
        static_assert(!Traits::info.isSigned);
        static_assert(Traits::info.normalized);
        static_assert(std::is_floating_point_v<Traits::ValueType>);

        typename Traits::NarrowSampleType unpackedValue;

        if constexpr(Traits::info.componentCount == 1) {
            unpackedValue = internal::unpackBitsUNorm<Traits::BlockType, Traits::info.redBitMask.width, Traits::info.redBitMask.offset>(value);
        } else {
            unpackedValue.r = internal::unpackBitsUNorm<Traits::BlockType, Traits::info.redBitMask.width, Traits::info.redBitMask.offset>(value);
        }
        if constexpr(Traits::info.componentCount >= 2) {
            unpackedValue.g = internal::unpackBitsUNorm<Traits::BlockType, Traits::info.greenBitMask.width, Traits::info.greenBitMask.offset>(value);
        }
        if constexpr(Traits::info.componentCount >= 3) {
            unpackedValue.b = internal::unpackBitsUNorm<Traits::BlockType, Traits::info.blueBitMask.width, Traits::info.blueBitMask.offset>(value);
        }
        if constexpr(Traits::info.componentCount == 4) {
            unpackedValue.a = internal::unpackBitsUNorm<Traits::BlockType, Traits::info.alphaBitMask.width, Traits::info.alphaBitMask.offset>(value);
        }

        return unpackedValue;
    }

    template<Format F>
    constexpr typename FormatTraits<F>::NarrowSampleType formatUnpackBitsSNorm(typename FormatTraits<F>::BlockType value) noexcept {
        using Traits = FormatTraits<F>;
        static_assert(Traits::info.packed);
        static_assert(Traits::info.isSigned);
        static_assert(Traits::info.normalized);
        static_assert(std::is_floating_point_v<Traits::ValueType>);

        typename Traits::NarrowSampleType unpackedValue;

        if constexpr(Traits::info.componentCount == 1) {
            unpackedValue = internal::unpackBitsSNorm<Traits::BlockType, Traits::info.redBitMask.width, Traits::info.redBitMask.offset>(value);
        } else {
            unpackedValue.r = internal::unpackBitsSNorm<Traits::BlockType, Traits::info.redBitMask.width, Traits::info.redBitMask.offset>(value);
        }
        if constexpr(Traits::info.componentCount >= 2) {
            unpackedValue.g = internal::unpackBitsSNorm<Traits::BlockType, Traits::info.greenBitMask.width, Traits::info.greenBitMask.offset>(value);
        }
        if constexpr(Traits::info.componentCount >= 3) {
            unpackedValue.b = internal::unpackBitsSNorm<Traits::BlockType, Traits::info.blueBitMask.width, Traits::info.blueBitMask.offset>(value);
        }
        if constexpr(Traits::info.componentCount == 4) {
            unpackedValue.a = internal::unpackBitsSNorm<Traits::BlockType, Traits::info.alphaBitMask.width, Traits::info.alphaBitMask.offset>(value);
        }

        return unpackedValue;
    }

#ifndef GF_EXCLUDE_SCALED_FORMATS
    template<Format F>
    constexpr typename FormatTraits<F>::NarrowSampleType formatUnpackBitsUScaled(typename FormatTraits<F>::BlockType value) noexcept {
        using Traits = FormatTraits<F>;
        static_assert(Traits::info.packed);
        static_assert(!Traits::info.isSigned);
        static_assert(Traits::info.scaled);
        static_assert(std::is_floating_point_v<Traits::ValueType>);

        typename Traits::NarrowSampleType unpackedValue;

        if constexpr(Traits::info.componentCount == 1) {
            unpackedValue = internal::unpackBitsUScaled<Traits::BlockType, Traits::info.redBitMask.width, Traits::info.redBitMask.offset>(value);
        } else {
            unpackedValue.r = internal::unpackBitsUScaled<Traits::BlockType, Traits::info.redBitMask.width, Traits::info.redBitMask.offset>(value);
        }
        if constexpr(Traits::info.componentCount >= 2) {
            unpackedValue.g = internal::unpackBitsUScaled<Traits::BlockType, Traits::info.greenBitMask.width, Traits::info.greenBitMask.offset>(value);
        }
        if constexpr(Traits::info.componentCount >= 3) {
            unpackedValue.b = internal::unpackBitsUScaled<Traits::BlockType, Traits::info.blueBitMask.width, Traits::info.blueBitMask.offset>(value);
        }
        if constexpr(Traits::info.componentCount == 4) {
            unpackedValue.a = internal::unpackBitsUScaled<Traits::BlockType, Traits::info.alphaBitMask.width, Traits::info.alphaBitMask.offset>(value);
        }

        return unpackedValue;
    }

    template<Format F>
    constexpr typename FormatTraits<F>::NarrowSampleType formatUnpackBitsSScaled(typename FormatTraits<F>::BlockType value) noexcept {
        using Traits = FormatTraits<F>;
        static_assert(Traits::info.packed);
        static_assert(Traits::info.isSigned);
        static_assert(Traits::info.scaled);
        static_assert(std::is_floating_point_v<Traits::ValueType>);

        typename Traits::NarrowSampleType unpackedValue;

        if constexpr(Traits::info.componentCount == 1) {
            unpackedValue = internal::unpackBitsSScaled<Traits::BlockType, Traits::info.redBitMask.width, Traits::info.redBitMask.offset>(value);
        } else {
            unpackedValue.r = internal::unpackBitsSScaled<Traits::BlockType, Traits::info.redBitMask.width, Traits::info.redBitMask.offset>(value);
        }
        if constexpr(Traits::info.componentCount >= 2) {
            unpackedValue.g = internal::unpackBitsSScaled<Traits::BlockType, Traits::info.greenBitMask.width, Traits::info.greenBitMask.offset>(value);
        }
        if constexpr(Traits::info.componentCount >= 3) {
            unpackedValue.b = internal::unpackBitsSScaled<Traits::BlockType, Traits::info.blueBitMask.width, Traits::info.blueBitMask.offset>(value);
        }
        if constexpr(Traits::info.componentCount == 4) {
            unpackedValue.a = internal::unpackBitsSScaled<Traits::BlockType, Traits::info.alphaBitMask.width, Traits::info.alphaBitMask.offset>(value);
        }

        return unpackedValue;
    }
#endif // GF_EXCLUDE_SCALED_FORMATS

    template<Format F>
    constexpr typename FormatTraits<F>::NarrowSampleType formatUnpackBitsUInt(typename FormatTraits<F>::BlockType value) noexcept {
        using Traits = FormatTraits<F>;
        static_assert(Traits::info.packed);
        static_assert(!Traits::info.isSigned);
        static_assert(Traits::info.integer);
        static_assert(std::is_integral_v<Traits::ValueType>);
        static_assert(std::is_unsigned_v<Traits::ValueType>);

        typename Traits::NarrowSampleType unpackedValue;

        if constexpr(Traits::info.componentCount == 1) {
            unpackedValue = internal::unpackBitsUInt<Traits::BlockType, Traits::info.redBitMask.width, Traits::info.redBitMask.offset>(value);
        } else {
            unpackedValue.r = internal::unpackBitsUInt<Traits::BlockType, Traits::info.redBitMask.width, Traits::info.redBitMask.offset>(value);
        }
        if constexpr(Traits::info.componentCount >= 2) {
            unpackedValue.g = internal::unpackBitsUInt<Traits::BlockType, Traits::info.greenBitMask.width, Traits::info.greenBitMask.offset>(value);
        }
        if constexpr(Traits::info.componentCount >= 3) {
            unpackedValue.b = internal::unpackBitsUInt<Traits::BlockType, Traits::info.blueBitMask.width, Traits::info.blueBitMask.offset>(value);
        }
        if constexpr(Traits::info.componentCount == 4) {
            unpackedValue.a = internal::unpackBitsUInt<Traits::BlockType, Traits::info.alphaBitMask.width, Traits::info.alphaBitMask.offset>(value);
        }

        return unpackedValue;
    }

    template<Format F>
    constexpr typename FormatTraits<F>::NarrowSampleType formatUnpackBitsSInt(typename FormatTraits<F>::BlockType value) noexcept {
        using Traits = FormatTraits<F>;
        static_assert(Traits::info.packed);
        static_assert(Traits::info.isSigned);
        static_assert(Traits::info.integer);
        static_assert(std::is_integral_v<Traits::ValueType>);
        static_assert(std::is_signed_v<Traits::ValueType>);

        typename Traits::NarrowSampleType unpackedValue;

        if constexpr(Traits::info.componentCount == 1) {
            unpackedValue = internal::unpackBitsSInt<Traits::BlockType, Traits::info.redBitMask.width, Traits::info.redBitMask.offset>(value);
        } else {
            unpackedValue.r = internal::unpackBitsSInt<Traits::BlockType, Traits::info.redBitMask.width, Traits::info.redBitMask.offset>(value);
        }
        if constexpr(Traits::info.componentCount >= 2) {
            unpackedValue.g = internal::unpackBitsSInt<Traits::BlockType, Traits::info.greenBitMask.width, Traits::info.greenBitMask.offset>(value);
        }
        if constexpr(Traits::info.componentCount >= 3) {
            unpackedValue.b = internal::unpackBitsSInt<Traits::BlockType, Traits::info.blueBitMask.width, Traits::info.blueBitMask.offset>(value);
        }
        if constexpr(Traits::info.componentCount == 4) {
            unpackedValue.a = internal::unpackBitsSInt<Traits::BlockType, Traits::info.alphaBitMask.width, Traits::info.alphaBitMask.offset>(value);
        }

        return unpackedValue;
    }

    template<Format F>
    constexpr typename FormatTraits<F>::BlockType formatPackBitsUNorm(typename FormatTraits<F>::NarrowSampleType value) noexcept {
        using Traits = FormatTraits<F>;
        static_assert(Traits::info.packed);
        static_assert(!Traits::info.isSigned);
        static_assert(Traits::info.normalized);
        static_assert(std::is_floating_point_v<Traits::ValueType>);

        typename Traits::BlockType packedValue;

        if constexpr(Traits::info.componentCount == 1) {
            packedValue = internal::packBitsUNorm<Traits::BlockType, Traits::info.redBitMask.width, Traits::info.redBitMask.offset>(value);
        } else {
            packedValue = internal::packBitsUNorm<Traits::BlockType, Traits::info.redBitMask.width, Traits::info.redBitMask.offset>(value.r);
        }
        if constexpr(Traits::info.componentCount >= 2) {
            packedValue |= internal::packBitsUNorm<Traits::BlockType, Traits::info.greenBitMask.width, Traits::info.greenBitMask.offset>(value.g);
        }
        if constexpr(Traits::info.componentCount >= 3) {
            packedValue |= internal::packBitsUNorm<Traits::BlockType, Traits::info.blueBitMask.width, Traits::info.blueBitMask.offset>(value.b);
        }
        if constexpr(Traits::info.componentCount == 4) {
            packedValue |= internal::packBitsUNorm<Traits::BlockType, Traits::info.alphaBitMask.width, Traits::info.alphaBitMask.offset>(value.a);
        }

        return packedValue;
    }

    template<Format F>
    constexpr typename FormatTraits<F>::BlockType formatPackBitsSNorm(typename FormatTraits<F>::NarrowSampleType value) noexcept {
        using Traits = FormatTraits<F>;
        static_assert(Traits::info.packed);
        static_assert(Traits::info.isSigned);
        static_assert(Traits::info.normalized);
        static_assert(std::is_floating_point_v<Traits::ValueType>);
        
        typename Traits::BlockType packedValue;

        if constexpr(Traits::info.componentCount == 1) {
            packedValue = internal::packBitsSNorm<Traits::BlockType, Traits::info.redBitMask.width, Traits::info.redBitMask.offset>(value);
        } else {
            packedValue = internal::packBitsSNorm<Traits::BlockType, Traits::info.redBitMask.width, Traits::info.redBitMask.offset>(value.r);
        }
        if constexpr(Traits::info.componentCount >= 2) {
            packedValue |= internal::packBitsSNorm<Traits::BlockType, Traits::info.greenBitMask.width, Traits::info.greenBitMask.offset>(value.g);
        }
        if constexpr(Traits::info.componentCount >= 3) {
            packedValue |= internal::packBitsSNorm<Traits::BlockType, Traits::info.blueBitMask.width, Traits::info.blueBitMask.offset>(value.b);
        }
        if constexpr(Traits::info.componentCount == 4) {
            packedValue |= internal::packBitsSNorm<Traits::BlockType, Traits::info.alphaBitMask.width, Traits::info.alphaBitMask.offset>(value.a);
        }

        return packedValue;
    }

#ifndef GF_EXCLUDE_SCALED_FORMATS
    template<Format F>
    constexpr typename FormatTraits<F>::BlockType formatPackBitsUScaled(typename FormatTraits<F>::NarrowSampleType value) noexcept {
        using Traits = FormatTraits<F>;
        static_assert(Traits::info.packed);
        static_assert(!Traits::info.isSigned);
        static_assert(Traits::info.scaled);
        static_assert(std::is_floating_point_v<Traits::ValueType>);

        typename Traits::BlockType packedValue;

        if constexpr(Traits::info.componentCount == 1) {
            packedValue = internal::packBitsUScaled<Traits::BlockType, Traits::info.redBitMask.width, Traits::info.redBitMask.offset>(value);
        } else {
            packedValue = internal::packBitsUScaled<Traits::BlockType, Traits::info.redBitMask.width, Traits::info.redBitMask.offset>(value.r);
        }
        if constexpr(Traits::info.componentCount >= 2) {
            packedValue |= internal::packBitsUScaled<Traits::BlockType, Traits::info.greenBitMask.width, Traits::info.greenBitMask.offset>(value.g);
        }
        if constexpr(Traits::info.componentCount >= 3) {
            packedValue |= internal::packBitsUScaled<Traits::BlockType, Traits::info.blueBitMask.width, Traits::info.blueBitMask.offset>(value.b);
        }
        if constexpr(Traits::info.componentCount == 4) {
            packedValue |= internal::packBitsUScaled<Traits::BlockType, Traits::info.alphaBitMask.width, Traits::info.alphaBitMask.offset>(value.a);
        }

        return packedValue;
    }

    template<Format F>
    constexpr typename FormatTraits<F>::BlockType formatPackBitsSScaled(typename FormatTraits<F>::NarrowSampleType value) noexcept {
        using Traits = FormatTraits<F>;
        static_assert(Traits::info.packed);
        static_assert(Traits::info.isSigned);
        static_assert(Traits::info.scaled);
        static_assert(std::is_floating_point_v<Traits::ValueType>);

        typename Traits::BlockType packedValue;

        if constexpr(Traits::info.componentCount == 1) {
            packedValue = internal::packBitsSScaled<Traits::BlockType, Traits::info.redBitMask.width, Traits::info.redBitMask.offset>(value);
        } else {
            packedValue = internal::packBitsSScaled<Traits::BlockType, Traits::info.redBitMask.width, Traits::info.redBitMask.offset>(value.r);
        }
        if constexpr(Traits::info.componentCount >= 2) {
            packedValue |= internal::packBitsSScaled<Traits::BlockType, Traits::info.greenBitMask.width, Traits::info.greenBitMask.offset>(value.g);
        }
        if constexpr(Traits::info.componentCount >= 3) {
            packedValue |= internal::packBitsSScaled<Traits::BlockType, Traits::info.blueBitMask.width, Traits::info.blueBitMask.offset>(value.b);
        }
        if constexpr(Traits::info.componentCount == 4) {
            packedValue |= internal::packBitsSScaled<Traits::BlockType, Traits::info.alphaBitMask.width, Traits::info.alphaBitMask.offset>(value.a);
        }

        return packedValue;
    }
#endif // GF_EXCLUDE_SCALED_FORMATS

    template<Format F>
    constexpr typename FormatTraits<F>::BlockType formatPackBitsUInt(typename FormatTraits<F>::NarrowSampleType value) noexcept {
        using Traits = FormatTraits<F>;
        static_assert(Traits::info.packed);
        static_assert(!Traits::info.isSigned);
        static_assert(Traits::info.integer);
        static_assert(std::is_integral_v<Traits::ValueType>);
        static_assert(std::is_unsigned_v<Traits::ValueType>);

        typename Traits::BlockType packedValue;

        if constexpr(Traits::info.componentCount == 1) {
            packedValue = internal::packBitsUInt<Traits::BlockType, Traits::info.redBitMask.width, Traits::info.redBitMask.offset>(value);
        } else {
            packedValue = internal::packBitsUInt<Traits::BlockType, Traits::info.redBitMask.width, Traits::info.redBitMask.offset>(value.r);
        }
        if constexpr(Traits::info.componentCount >= 2) {
            packedValue |= internal::packBitsUInt<Traits::BlockType, Traits::info.greenBitMask.width, Traits::info.greenBitMask.offset>(value.g);
        }
        if constexpr(Traits::info.componentCount >= 3) {
            packedValue |= internal::packBitsUInt<Traits::BlockType, Traits::info.blueBitMask.width, Traits::info.blueBitMask.offset>(value.b);
        }
        if constexpr(Traits::info.componentCount == 4) {
            packedValue |= internal::packBitsUInt<Traits::BlockType, Traits::info.alphaBitMask.width, Traits::info.alphaBitMask.offset>(value.a);
        }

        return packedValue;
    }

    template<Format F>
    constexpr typename FormatTraits<F>::BlockType formatPackBitsSInt(typename FormatTraits<F>::NarrowSampleType value) noexcept {
        using Traits = FormatTraits<F>;
        static_assert(Traits::info.packed);
        static_assert(Traits::info.isSigned);
        static_assert(Traits::info.integer);
        static_assert(std::is_integral_v<Traits::ValueType>);
        static_assert(std::is_signed_v<Traits::ValueType>);

        typename Traits::BlockType packedValue;

        if constexpr(Traits::info.componentCount == 1) {
            packedValue = internal::packBitsSInt<Traits::BlockType, Traits::info.redBitMask.width, Traits::info.redBitMask.offset>(value);
        } else {
            packedValue = internal::packBitsSInt<Traits::BlockType, Traits::info.redBitMask.width, Traits::info.redBitMask.offset>(value.r);
        }
        if constexpr(Traits::info.componentCount >= 2) {
            packedValue |= internal::packBitsSInt<Traits::BlockType, Traits::info.greenBitMask.width, Traits::info.greenBitMask.offset>(value.g);
        }
        if constexpr(Traits::info.componentCount >= 3) {
            packedValue |= internal::packBitsSInt<Traits::BlockType, Traits::info.blueBitMask.width, Traits::info.blueBitMask.offset>(value.b);
        }
        if constexpr(Traits::info.componentCount == 4) {
            packedValue |= internal::packBitsSInt<Traits::BlockType, Traits::info.alphaBitMask.width, Traits::info.alphaBitMask.offset>(value.a);
        }

        return packedValue;
    }

    template<Format F>
    [[nodiscard]]
    constexpr typename FormatTraits<F>::NarrowSampleType formatUnpackBits(typename FormatTraits<F>::BlockType value) noexcept {
        using Traits = FormatTraits<F>;
        static_assert(Traits::info.packed);

        if constexpr(Traits::info.normalized)
        {
            if constexpr(!Traits::info.isSigned) {
                //unorm
                return formatUnpackBitsUNorm<F>(value);
            } else {
                //snorm
                return formatUnpackBitsSNorm<F>(value);
            }
        }
#ifndef GF_EXCLUDE_SCALED_FORMATS
        else if constexpr(Traits::info.scaled) {
            if constexpr(!Traits::info.isSigned) {
                //uscaled
                return formatUnpackBitsUScaled<F>(value);
            } else {
                //sscaled
                return formatUnpackBitsSScaled<F>(value);
            }
        }
#endif // GF_EXCLUDE_SCALED_FORMATS
        else if constexpr(Traits::info.integer) {
            if constexpr(!Traits::info.isSigned) {
                //uint
                return formatUnpackBitsUInt<F>(value);
            } else {
                //sint
                return formatUnpackBitsSInt<F>(value);
            }
        }
    }

    template<Format F>
    [[nodiscard]]
    constexpr typename FormatTraits<F>::BlockType formatPackBits(const typename FormatTraits<F>::NarrowSampleType &value) noexcept {
        using Traits = FormatTraits<F>;
        static_assert(Traits::info.packed);

        if constexpr(Traits::info.normalized)
        {
            if constexpr(!Traits::info.isSigned) {
                //unorm
                return formatPackBitsUNorm<F>(value);
            } else {
                //snorm
                return formatPackBitsSNorm<F>(value);
            }
        }
#ifndef GF_EXCLUDE_SCALED_FORMATS
        else if constexpr(Traits::info.scaled) {
            if constexpr(!Traits::info.isSigned) {
                //uscaled
                return formatPackBitsUScaled<F>(value);
            } else {
                //sscaled
                return formatPackBitsSScaled<F>(value);
            }
        }
#endif
        else if constexpr(Traits::info.integer) {
            if constexpr(!Traits::info.isSigned) {
                //uint
                return formatPackBitsUInt<F>(value);
            } else {
                //sint
                return formatPackBitsSInt<F>(value);
            }
        }
    }

    //----------------------------
    // BYTES
    //----------------------------
    template<Format F>
    [[nodiscard]]
    constexpr typename FormatTraits<F>::NarrowSampleType formatUnpackBytesUNorm(typename FormatTraits<F>::BlockType value) noexcept {
        using Traits = FormatTraits<F>;
        static_assert(!Traits::info.packed);
        static_assert(!Traits::info.isSigned);
        static_assert(Traits::info.normalized);
        static_assert(std::is_floating_point_v<Traits::ValueType>);

        typename Traits::NarrowSampleType unpackedValue;

        if constexpr(Traits::info.componentCount == 1) {
            unpackedValue = internal::unpackUNorm<Traits::BlockType>(value);
        } else {
            unpackedValue.r = internal::unpackUNorm<Traits::BlockType::value_type>(value[Traits::info.redIndex]);
        }
        if constexpr(Traits::info.componentCount >= 2) {
            unpackedValue.g = internal::unpackUNorm<Traits::BlockType::value_type>(value[Traits::info.greenIndex]);
        }
        if constexpr(Traits::info.componentCount >= 3) {
            unpackedValue.b = internal::unpackUNorm<Traits::BlockType::value_type>(value[Traits::info.blueIndex]);
        }
        if constexpr(Traits::info.componentCount == 4) {
            unpackedValue.a = internal::unpackUNorm<Traits::BlockType::value_type>(value[Traits::info.alphaIndex]);
        }

        return unpackedValue;
    }

    template<Format F>
    [[nodiscard]]
    constexpr typename FormatTraits<F>::NarrowSampleType formatUnpackBytesSNorm(typename FormatTraits<F>::BlockType value) noexcept {
        using Traits = FormatTraits<F>;
        static_assert(!Traits::info.packed);
        static_assert(Traits::info.isSigned);
        static_assert(Traits::info.normalized);
        static_assert(std::is_floating_point_v<Traits::ValueType>);

        typename Traits::NarrowSampleType unpackedValue;

        if constexpr(Traits::info.componentCount == 1) {
            unpackedValue = internal::unpackSNorm<Traits::BlockType>(value);
        } else {
            unpackedValue.r = internal::unpackSNorm<Traits::BlockType::value_type>(value[Traits::info.redIndex]);
        }
        if constexpr(Traits::info.componentCount >= 2) {
            unpackedValue.g = internal::unpackSNorm<Traits::BlockType::value_type>(value[Traits::info.greenIndex]);
        }
        if constexpr(Traits::info.componentCount >= 3) {
            unpackedValue.b = internal::unpackSNorm<Traits::BlockType::value_type>(value[Traits::info.blueIndex]);
        }
        if constexpr(Traits::info.componentCount == 4) {
            unpackedValue.a = internal::unpackSNorm<Traits::BlockType::value_type>(value[Traits::info.alphaIndex]);
        }

        return unpackedValue;
    }

#ifndef GF_EXCLUDE_SCALED_FORMATS
    template<Format F>
    [[nodiscard]]
    constexpr typename FormatTraits<F>::NarrowSampleType formatUnpackBytesUScaled(typename FormatTraits<F>::BlockType value) noexcept {
        using Traits = FormatTraits<F>;
        static_assert(!Traits::info.packed);
        static_assert(!Traits::info.isSigned);
        static_assert(Traits::info.scaled);
        static_assert(std::is_floating_point_v<Traits::ValueType>);

        typename Traits::NarrowSampleType unpackedValue;

        if constexpr(Traits::info.componentCount == 1) {
            unpackedValue = internal::unpackUScaled<Traits::BlockType>(value);
        } else {
            unpackedValue.r = internal::unpackUScaled<Traits::BlockType::value_type>(value[Traits::info.redIndex]);
        }
        if constexpr(Traits::info.componentCount >= 2) {
            unpackedValue.g = internal::unpackUScaled<Traits::BlockType::value_type>(value[Traits::info.greenIndex]);
        }
        if constexpr(Traits::info.componentCount >= 3) {
            unpackedValue.b = internal::unpackUScaled<Traits::BlockType::value_type>(value[Traits::info.blueIndex]);
        }
        if constexpr(Traits::info.componentCount == 4) {
            unpackedValue.a = internal::unpackUScaled<Traits::BlockType::value_type>(value[Traits::info.alphaIndex]);
        }

        return unpackedValue;
    }

    template<Format F>
    [[nodiscard]]
    constexpr typename FormatTraits<F>::NarrowSampleType formatUnpackBytesSScaled(typename FormatTraits<F>::BlockType value) noexcept {
        using Traits = FormatTraits<F>;
        static_assert(!Traits::info.packed);
        static_assert(Traits::info.isSigned);
        static_assert(Traits::info.scaled);
        static_assert(std::is_floating_point_v<Traits::ValueType>);

        typename Traits::NarrowSampleType unpackedValue;

        if constexpr(Traits::info.componentCount == 1) {
            unpackedValue = internal::unpackSScaled<Traits::BlockType>(value);
        } else {
            unpackedValue.r = internal::unpackSScaled<Traits::BlockType::value_type>(value[Traits::info.redIndex]);
        }
        if constexpr(Traits::info.componentCount >= 2) {
            unpackedValue.g = internal::unpackSScaled<Traits::BlockType::value_type>(value[Traits::info.greenIndex]);
        }
        if constexpr(Traits::info.componentCount >= 3) {
            unpackedValue.b = internal::unpackSScaled<Traits::BlockType::value_type>(value[Traits::info.blueIndex]);
        }
        if constexpr(Traits::info.componentCount == 4) {
            unpackedValue.a = internal::unpackSScaled<Traits::BlockType::value_type>(value[Traits::info.alphaIndex]);
        }

        return unpackedValue;
    }
#endif

    template<Format F>
    [[nodiscard]]
    constexpr typename FormatTraits<F>::NarrowSampleType formatUnpackBytesUInt(typename FormatTraits<F>::BlockType value) noexcept {
        using Traits = FormatTraits<F>;
        static_assert(!Traits::info.packed);
        static_assert(!Traits::info.isSigned);
        static_assert(Traits::info.integer);
        static_assert(std::is_unsigned_v<Traits::ValueType>);
        static_assert(std::is_integral_v<Traits::ValueType>);

        typename Traits::NarrowSampleType unpackedValue;

        if constexpr(Traits::info.componentCount == 1) {
            unpackedValue = internal::unpackUInt<Traits::BlockType>(value);
        } else {
            unpackedValue.r = internal::unpackUInt<Traits::BlockType::value_type>(value[Traits::info.redIndex]);
        }
        if constexpr(Traits::info.componentCount >= 2) {
            unpackedValue.g = internal::unpackUInt<Traits::BlockType::value_type>(value[Traits::info.greenIndex]);
        }
        if constexpr(Traits::info.componentCount >= 3) {
            unpackedValue.b = internal::unpackUInt<Traits::BlockType::value_type>(value[Traits::info.blueIndex]);
        }
        if constexpr(Traits::info.componentCount == 4) {
            unpackedValue.a = internal::unpackUInt<Traits::BlockType::value_type>(value[Traits::info.alphaIndex]);
        }

        return unpackedValue;
    }

    template<Format F>
    [[nodiscard]]
    constexpr typename FormatTraits<F>::NarrowSampleType formatUnpackBytesSInt(typename FormatTraits<F>::BlockType value) noexcept {
        using Traits = FormatTraits<F>;
        static_assert(!Traits::info.packed);
        static_assert(Traits::info.isSigned);
        static_assert(Traits::info.integer);
        static_assert(std::is_signed_v<Traits::ValueType>);
        static_assert(std::is_integral_v<Traits::ValueType>);

        typename Traits::NarrowSampleType unpackedValue;

        if constexpr(Traits::info.componentCount == 1) {
            unpackedValue = internal::unpackSInt<Traits::BlockType>(value);
        } else {
            unpackedValue.r = internal::unpackSInt<Traits::BlockType::value_type>(value[Traits::info.redIndex]);
        }
        if constexpr(Traits::info.componentCount >= 2) {
            unpackedValue.g = internal::unpackSInt<Traits::BlockType::value_type>(value[Traits::info.greenIndex]);
        }
        if constexpr(Traits::info.componentCount >= 3) {
            unpackedValue.b = internal::unpackSInt<Traits::BlockType::value_type>(value[Traits::info.blueIndex]);
        }
        if constexpr(Traits::info.componentCount == 4) {
            unpackedValue.a = internal::unpackSInt<Traits::BlockType::value_type>(value[Traits::info.alphaIndex]);
        }

        return unpackedValue;
    }

    template<Format F>
    [[nodiscard]]
    constexpr typename FormatTraits<F>::NarrowSampleType formatUnpackBytesFloat(typename FormatTraits<F>::BlockType value) noexcept {
        using Traits = FormatTraits<F>;
        static_assert(!Traits::info.packed);
        static_assert(Traits::info.isSigned);
        static_assert(Traits::info.floatingPoint);
        static_assert(std::is_floating_point_v<Traits::ValueType>);

        typename Traits::NarrowSampleType unpackedValue;

        if constexpr(Traits::info.componentCount == 1) {
            unpackedValue = internal::unpackFloat<Traits::BlockType>(value);
        } else {
            unpackedValue.r = internal::unpackFloat<Traits::BlockType::value_type>(value[Traits::info.redIndex]);
        }
        if constexpr(Traits::info.componentCount >= 2) {
            unpackedValue.g = internal::unpackFloat<Traits::BlockType::value_type>(value[Traits::info.greenIndex]);
        }
        if constexpr(Traits::info.componentCount >= 3) {
            unpackedValue.b = internal::unpackFloat<Traits::BlockType::value_type>(value[Traits::info.blueIndex]);
        }
        if constexpr(Traits::info.componentCount == 4) {
            unpackedValue.a = internal::unpackFloat<Traits::BlockType::value_type>(value[Traits::info.alphaIndex]);
        }

        return unpackedValue;
    }

    template<Format F>
    [[nodiscard]]
    constexpr typename FormatTraits<F>::BlockType formatPackBytesUNorm(const typename FormatTraits<F>::NarrowSampleType &value) noexcept {
        using Traits = FormatTraits<F>;
        static_assert(!Traits::info.packed);
        static_assert(!Traits::info.isSigned);
        static_assert(Traits::info.normalized);
        static_assert(std::is_floating_point_v<Traits::ValueType>);

        typename Traits::BlockType packedValue;

        if constexpr(Traits::info.componentCount == 1) {
            packedValue = internal::packUNorm<Traits::BlockType>(value);
        } else {
            packedValue[Traits::info.redIndex] = internal::packUNorm<Traits::BlockType::value_type>(value.r);
        }
        if constexpr(Traits::info.componentCount >= 2) {
            packedValue[Traits::info.greenIndex] = internal::packUNorm<Traits::BlockType::value_type>(value.g);
        }
        if constexpr(Traits::info.componentCount >= 3) {
            packedValue[Traits::info.blueIndex] = internal::packUNorm<Traits::BlockType::value_type>(value.b);
        }
        if constexpr(Traits::info.componentCount == 4) {
            packedValue[Traits::info.alphaIndex] = internal::packUNorm<Traits::BlockType::value_type>(value.a);
        }

        return packedValue;
    }

    template<Format F>
    [[nodiscard]]
    constexpr typename FormatTraits<F>::BlockType formatPackBytesSNorm(const typename FormatTraits<F>::NarrowSampleType &value) noexcept {
        using Traits = FormatTraits<F>;
        static_assert(!Traits::info.packed);
        static_assert(Traits::info.isSigned);
        static_assert(Traits::info.normalized);
        static_assert(std::is_floating_point_v<Traits::ValueType>);

        typename Traits::BlockType packedValue;

        if constexpr(Traits::info.componentCount == 1) {
            packedValue = internal::packSNorm<Traits::BlockType>(value);
        } else {
            packedValue[Traits::info.redIndex] = internal::packSNorm<Traits::BlockType::value_type>(value.r);
        }
        if constexpr(Traits::info.componentCount >= 2) {
            packedValue[Traits::info.greenIndex] = internal::packSNorm<Traits::BlockType::value_type>(value.g);
        }
        if constexpr(Traits::info.componentCount >= 3) {
            packedValue[Traits::info.blueIndex] = internal::packSNorm<Traits::BlockType::value_type>(value.b);
        }
        if constexpr(Traits::info.componentCount == 4) {
            packedValue[Traits::info.alphaIndex] = internal::packSNorm<Traits::BlockType::value_type>(value.a);
        }

        return packedValue;
    }

#ifndef GF_EXCLUDE_SCALED_FORMATS
    template<Format F>
    [[nodiscard]]
    constexpr typename FormatTraits<F>::BlockType formatPackBytesUScaled(const typename FormatTraits<F>::NarrowSampleType &value) noexcept {
        using Traits = FormatTraits<F>;
        static_assert(!Traits::info.packed);
        static_assert(!Traits::info.isSigned);
        static_assert(Traits::info.scaled);
        static_assert(std::is_floating_point_v<Traits::ValueType>);

        typename Traits::BlockType packedValue;

        if constexpr(Traits::info.componentCount == 1) {
            packedValue = internal::packUScaled<Traits::BlockType>(value);
        } else {
            packedValue[Traits::info.redIndex] = internal::packUScaled<Traits::BlockType::value_type>(value.r);
        }
        if constexpr(Traits::info.componentCount >= 2) {
            packedValue[Traits::info.greenIndex] = internal::packUScaled<Traits::BlockType::value_type>(value.g);
        }
        if constexpr(Traits::info.componentCount >= 3) {
            packedValue[Traits::info.blueIndex] = internal::packUScaled<Traits::BlockType::value_type>(value.b);
        }
        if constexpr(Traits::info.componentCount == 4) {
            packedValue[Traits::info.alphaIndex] = internal::packUScaled<Traits::BlockType::value_type>(value.a);
        }

        return packedValue;
    }

    template<Format F>
    [[nodiscard]]
    constexpr typename FormatTraits<F>::BlockType formatPackBytesSScaled(const typename FormatTraits<F>::NarrowSampleType &value) noexcept {
        using Traits = FormatTraits<F>;
        static_assert(!Traits::info.packed);
        static_assert(Traits::info.isSigned);
        static_assert(Traits::info.scaled);
        static_assert(std::is_floating_point_v<Traits::ValueType>);

        typename Traits::BlockType packedValue;

        if constexpr(Traits::info.componentCount == 1) {
            packedValue = internal::packSScaled<Traits::BlockType>(value);
        } else {
            packedValue[Traits::info.redIndex] = internal::packSScaled<Traits::BlockType::value_type>(value.r);
        }
        if constexpr(Traits::info.componentCount >= 2) {
            packedValue[Traits::info.greenIndex] = internal::packSScaled<Traits::BlockType::value_type>(value.g);
        }
        if constexpr(Traits::info.componentCount >= 3) {
            packedValue[Traits::info.blueIndex] = internal::packSScaled<Traits::BlockType::value_type>(value.b);
        }
        if constexpr(Traits::info.componentCount == 4) {
            packedValue[Traits::info.alphaIndex] = internal::packSScaled<Traits::BlockType::value_type>(value.a);
        }

        return packedValue;
    }
#endif

    template<Format F>
    [[nodiscard]]
    constexpr typename FormatTraits<F>::BlockType formatPackBytesUInt(const typename FormatTraits<F>::NarrowSampleType &value) noexcept {
        using Traits = FormatTraits<F>;
        static_assert(!Traits::info.packed);
        static_assert(!Traits::info.isSigned);
        static_assert(Traits::info.integer);
        static_assert(std::is_unsigned_v<Traits::ValueType>);
        static_assert(std::is_integral_v<Traits::ValueType>);

        typename Traits::BlockType packedValue;

        if constexpr(Traits::info.componentCount == 1) {
            packedValue = internal::packUInt<Traits::BlockType>(value);
        } else {
            packedValue[Traits::info.redIndex] = internal::packUInt<Traits::BlockType::value_type>(value.r);
        }
        if constexpr(Traits::info.componentCount >= 2) {
            packedValue[Traits::info.greenIndex] = internal::packUInt<Traits::BlockType::value_type>(value.g);
        }
        if constexpr(Traits::info.componentCount >= 3) {
            packedValue[Traits::info.blueIndex] = internal::packUInt<Traits::BlockType::value_type>(value.b);
        }
        if constexpr(Traits::info.componentCount == 4) {
            packedValue[Traits::info.alphaIndex] = internal::packUInt<Traits::BlockType::value_type>(value.a);
        }

        return packedValue;
    }

    template<Format F>
    [[nodiscard]]
    constexpr typename FormatTraits<F>::BlockType formatPackBytesSInt(const typename FormatTraits<F>::NarrowSampleType &value) noexcept {
        using Traits = FormatTraits<F>;
        static_assert(!Traits::info.packed);
        static_assert(Traits::info.isSigned);
        static_assert(Traits::info.integer);
        static_assert(std::is_signed_v<Traits::ValueType>);
        static_assert(std::is_integral_v<Traits::ValueType>);

        typename Traits::BlockType packedValue;

        if constexpr(Traits::info.componentCount == 1) {
            packedValue = internal::packSInt<Traits::BlockType>(value);
        } else {
            packedValue[Traits::info.redIndex] = internal::packSInt<Traits::BlockType::value_type>(value.r);
        }
        if constexpr(Traits::info.componentCount >= 2) {
            packedValue[Traits::info.greenIndex] = internal::packSInt<Traits::BlockType::value_type>(value.g);
        }
        if constexpr(Traits::info.componentCount >= 3) {
            packedValue[Traits::info.blueIndex] = internal::packSInt<Traits::BlockType::value_type>(value.b);
        }
        if constexpr(Traits::info.componentCount == 4) {
            packedValue[Traits::info.alphaIndex] = internal::packSInt<Traits::BlockType::value_type>(value.a);
        }

        return packedValue;
    }

    template<Format F>
    [[nodiscard]]
    constexpr typename FormatTraits<F>::BlockType formatPackBytesFloat(const typename FormatTraits<F>::NarrowSampleType &value) noexcept {
        using Traits = FormatTraits<F>;
        static_assert(!Traits::info.packed);
        static_assert(Traits::info.isSigned);
        static_assert(Traits::info.floatingPoint);
        static_assert(std::is_floating_point_v<Traits::ValueType>);

        typename Traits::BlockType packedValue;

        if constexpr(Traits::info.componentCount == 1) {
            packedValue = internal::packFloat<Traits::BlockType>(value);
        } else {
            packedValue[Traits::info.redIndex] = internal::packFloat<Traits::BlockType::value_type>(value.r);
        }
        if constexpr(Traits::info.componentCount >= 2) {
            packedValue[Traits::info.greenIndex] = internal::packFloat<Traits::BlockType::value_type>(value.g);
        }
        if constexpr(Traits::info.componentCount >= 3) {
            packedValue[Traits::info.blueIndex] = internal::packFloat<Traits::BlockType::value_type>(value.b);
        }
        if constexpr(Traits::info.componentCount == 4) {
            packedValue[Traits::info.alphaIndex] = internal::packFloat<Traits::BlockType::value_type>(value.a);
        }

        return packedValue;
    }

    template<Format F>
    [[nodiscard]]
    constexpr typename FormatTraits<F>::NarrowSampleType formatUnpackBytes(typename FormatTraits<F>::BlockType value) noexcept {
        using Traits = FormatTraits<F>;
        static_assert(!Traits::info.packed);

        if constexpr(Traits::info.normalized)
        {
            if constexpr(!Traits::info.isSigned) {
                //unorm
                return formatUnpackBytesUNorm<F>(value);
            } else {
                //snorm
                return formatUnpackBytesSNorm<F>(value);
            }
        }
#ifndef GF_EXCLUDE_SCALED_FORMATS
        else if constexpr(Traits::info.scaled)
        {
            if constexpr(!Traits::info.isSigned) {
                //uscaled
                return formatUnpackBytesUScaled<F>(value);
            } else {
                //sscaled
                return formatUnpackBytesSScaled<F>(value);
            }
        }
#endif
        else if constexpr(Traits::info.integer) {
            if constexpr(!Traits::info.isSigned) {
                //uint
                return formatUnpackBytesUInt<F>(value);
            } else {
                //sint
                return formatUnpackBytesSInt<F>(value);
            }
        } else if constexpr(Traits::info.floatingPoint) {
            return formatUnpackBytesFloat<F>(value);
        }
    }

    template<Format F>
    [[nodiscard]]
    constexpr typename FormatTraits<F>::BlockType formatPackBytes(const typename FormatTraits<F>::NarrowSampleType &value) noexcept {
        using Traits = FormatTraits<F>;
        static_assert(!Traits::info.packed);

        if constexpr(Traits::info.normalized)
        {
            if constexpr(!Traits::info.isSigned) {
                //unorm
                return formatPackBytesUNorm<F>(value);
            } else {
                //snorm
                return formatPackBytesSNorm<F>(value);
            }
        }
#ifndef GF_EXCLUDE_SCALED_FORMATS
        else if constexpr(Traits::info.scaled)
        {
            if constexpr(!Traits::info.isSigned) {
                //uscaled
                return formatPackBytesUScaled<F>(value);
            } else {
                //sscaled
                return formatPackBytesSScaled<F>(value);
            }
        }
#endif
        else if constexpr(Traits::info.integer)
        {
            if constexpr(!Traits::info.isSigned) {
                //uint
                return formatPackBytesUInt<F>(value);
            } else {
                //sint
                return formatPackBytesSInt<F>(value);
            }
        }
        else if constexpr(Traits::info.floatingPoint) {
            return formatPackBytesFloat<F>(value);
        }
    }


    //---------------------
    // MAIN FUNCTIONS
    //---------------------
    template<Format F>
    [[nodiscard]]
    constexpr typename FormatTraits<F>::NarrowSampleType formatUnpack(const typename FormatTraits<F>::BlockType &value) noexcept {
        using Traits = FormatTraits<F>;

        if constexpr(Traits::info.packed) {
            return formatUnpackBits<F>(value);
        } else {
            return formatUnpackBytes<F>(value);
        }
    }

    template<Format F>
    [[nodiscard]]
    constexpr typename FormatTraits<F>::BlockType formatPack(const typename FormatTraits<F>::NarrowSampleType &value) noexcept {
        using Traits = FormatTraits<F>;

        if constexpr(Traits::info.packed) {
            return formatPackBits<F>(value);
        } else {
            return formatPackBytes<F>(value);
        }
    }
}