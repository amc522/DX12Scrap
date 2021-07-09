#pragma once

#include <algorithm>

// The following defines are originally defined in DirectXMath.h
#ifndef XM_CONSTEXPR
#define XM_CONSTEXPR constexpr
#endif

#ifndef XM_CALLCONV
#define XM_CALLCONV
#endif

#ifndef _XM_NO_INTRINSICS_
#define _XM_NO_INTRINSICS_
#endif

// The following defines are originally defined in the Windows SDK
#ifndef _Use_decl_annotations_
#define _Use_decl_annotations_
#endif

#ifndef OutputDebugStringA
#define OutputDebugStringA(x)
#endif

#ifndef _In_
#define _In_
#endif

#ifndef _In_reads_
#define _In_reads_(x)
#endif

#ifndef _In_range_
#define _In_range_(x, y)
#endif

#ifndef _Inout_updates_all_
#define _Inout_updates_all_(x)
#endif

#ifndef _Out_
#define _Out_
#endif

#ifndef _Out_writes_
#define _Out_writes_(x)
#endif

#ifndef _Analysis_assume_
#define _Analysis_assume_(x)
#endif

#ifndef DWORD
using DWORD = unsigned long;
#endif

template<class T, size_t N>
constexpr size_t GetArraySize(const T (&a)[N]) noexcept {
	return N;
}

#ifndef ARRAYSIZE
#define ARRAYSIZE(x) GetArraySize(x)
#endif

#ifndef _countof
#define __countof_helper(x) GetArraySize(x)
#endif

// The rest of this section is copied from DirectXMath library
namespace DirectX {

    // Everything in namespace PackedVector is copied from DirectXPackedVector.inl
	namespace PackedVector {
		//------------------------------------------------------------------------------
		// 16 bit floating point number consisting of a sign bit, a 5 bit biased 
		// exponent, and a 10 bit mantissa
		using HALF = uint16_t;

		//------------------------------------------------------------------------------
		// 4D Vector; 16 bit floating point components
		struct XMHALF4
		{
			union
			{
				struct
				{
					HALF x;
					HALF y;
					HALF z;
					HALF w;
				};
				uint64_t v;
			};

			XMHALF4() = default;

			XMHALF4(const XMHALF4&) = default;
			XMHALF4& operator=(const XMHALF4&) = default;

			XMHALF4(XMHALF4&&) = default;
			XMHALF4& operator=(XMHALF4&&) = default;

			explicit XM_CONSTEXPR XMHALF4(uint64_t Packed) : v(Packed) {}
			XM_CONSTEXPR XMHALF4(HALF _x, HALF _y, HALF _z, HALF _w) : x(_x), y(_y), z(_z), w(_w) {}
			explicit XMHALF4(_In_reads_(4) const HALF *pArray) : x(pArray[0]), y(pArray[1]), z(pArray[2]), w(pArray[3]) {}
			XMHALF4(float _x, float _y, float _z, float _w);
			explicit XMHALF4(_In_reads_(4) const float *pArray);

			XMHALF4& operator= (uint64_t Packed) { v = Packed; return *this; }
		};
	}

    struct XMVECTOR
    {
        union
        {
            float       vector4_f32[4];
            uint32_t    vector4_u32[4];
        };
    };

    using FXMVECTOR = const XMVECTOR &;


	//------------------------------------------------------------------------------
	// 4D Vector; 8 bit unsigned integer components
	struct XMUBYTE4
	{
		union
		{
			struct
			{
				uint8_t x;
				uint8_t y;
				uint8_t z;
				uint8_t w;
			};
			uint32_t v;
		};

		XMUBYTE4() = default;

		XMUBYTE4(const XMUBYTE4&) = default;
		XMUBYTE4& operator=(const XMUBYTE4&) = default;

		XMUBYTE4(XMUBYTE4&&) = default;
		XMUBYTE4& operator=(XMUBYTE4&&) = default;

		XM_CONSTEXPR XMUBYTE4(uint8_t _x, uint8_t _y, uint8_t _z, uint8_t _w) : x(_x), y(_y), z(_z), w(_w) {}
		explicit XM_CONSTEXPR XMUBYTE4(uint32_t Packed) : v(Packed) {}
		explicit XMUBYTE4(_In_reads_(4) const uint8_t *pArray) : x(pArray[0]), y(pArray[1]), z(pArray[2]), w(pArray[3]) {}
		XMUBYTE4(float _x, float _y, float _z, float _w);
		explicit XMUBYTE4(_In_reads_(4) const float *pArray);

		XMUBYTE4& operator= (uint32_t Packed) { v = Packed; return *this; }
	};

	//------------------------------------------------------------------------------
	// 4D Vector; 32 bit floating point components
	struct XMFLOAT4
	{
		float x;
		float y;
		float z;
		float w;

		XMFLOAT4() = default;

		XMFLOAT4(const XMFLOAT4&) = default;
		XMFLOAT4& operator=(const XMFLOAT4&) = default;

		XMFLOAT4(XMFLOAT4&&) = default;
		XMFLOAT4& operator=(XMFLOAT4&&) = default;

		XM_CONSTEXPR XMFLOAT4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
		explicit XMFLOAT4(_In_reads_(4) const float *pArray) : x(pArray[0]), y(pArray[1]), z(pArray[2]), w(pArray[3]) {}
	};

	// 4D Vector; 32 bit floating point components aligned on a 16 byte boundary
	__declspec(align(16)) struct XMFLOAT4A : public XMFLOAT4
	{
		XMFLOAT4A() = default;

		XMFLOAT4A(const XMFLOAT4A&) = default;
		XMFLOAT4A& operator=(const XMFLOAT4A&) = default;

		XMFLOAT4A(XMFLOAT4A&&) = default;
		XMFLOAT4A& operator=(XMFLOAT4A&&) = default;

		XM_CONSTEXPR XMFLOAT4A(float _x, float _y, float _z, float _w) : XMFLOAT4(_x, _y, _z, _w) {}
		explicit XMFLOAT4A(_In_reads_(4) const float *pArray) : XMFLOAT4(pArray) {}
	};

	//------------------------------------------------------------------------------
	// 4D Vector; 32 bit signed integer components
	struct XMINT4
	{
		int32_t x;
		int32_t y;
		int32_t z;
		int32_t w;

		XMINT4() = default;

		XMINT4(const XMINT4&) = default;
		XMINT4& operator=(const XMINT4&) = default;

		XMINT4(XMINT4&&) = default;
		XMINT4& operator=(XMINT4&&) = default;

		XM_CONSTEXPR XMINT4(int32_t _x, int32_t _y, int32_t _z, int32_t _w) : x(_x), y(_y), z(_z), w(_w) {}
		explicit XMINT4(_In_reads_(4) const int32_t *pArray) : x(pArray[0]), y(pArray[1]), z(pArray[2]), w(pArray[3]) {}
	};

	//------------------------------------------------------------------------------
	// Conversion types for constants
	__declspec(align(16)) struct XMVECTORF32
	{
		union
		{
			float f[4];
			XMVECTOR v;
		};

		inline operator XMVECTOR() const { return v; }
		inline operator const float*() const { return f; }
#if !defined(_XM_NO_INTRINSICS_) && defined(_XM_SSE_INTRINSICS_)
		inline operator __m128i() const { return _mm_castps_si128(v); }
		inline operator __m128d() const { return _mm_castps_pd(v); }
#endif
	};

	//------------------------------------------------------------------------------
	_Use_decl_annotations_
		inline XMVECTOR XM_CALLCONV XMLoadFloat4
	(
		const XMFLOAT4* pSource
	)
	{
		assert(pSource);
#if defined(_XM_NO_INTRINSICS_)
		XMVECTOR V;
		V.vector4_f32[0] = pSource->x;
		V.vector4_f32[1] = pSource->y;
		V.vector4_f32[2] = pSource->z;
		V.vector4_f32[3] = pSource->w;
		return V;
#elif defined(_XM_ARM_NEON_INTRINSICS_)
		return vld1q_f32(reinterpret_cast<const float*>(pSource));
#elif defined(_XM_SSE_INTRINSICS_)
		return _mm_loadu_ps(&pSource->x);
#endif
	}

	//------------------------------------------------------------------------------
	_Use_decl_annotations_
	inline XMVECTOR XM_CALLCONV XMLoadUByte4
	(
		const XMUBYTE4* pSource
	)
	{
		assert(pSource);
#if defined(_XM_NO_INTRINSICS_)
		XMVECTORF32 vResult = { { {
				static_cast<float>(pSource->x),
				static_cast<float>(pSource->y),
				static_cast<float>(pSource->z),
				static_cast<float>(pSource->w)
			} } };
		return vResult.v;
#elif defined(_XM_ARM_NEON_INTRINSICS_)
		uint32x2_t vInt8 = vld1_dup_u32(reinterpret_cast<const uint32_t*>(pSource));
		uint16x8_t vInt16 = vmovl_u8(vreinterpret_u8_u32(vInt8));
		uint32x4_t vInt = vmovl_u16(vget_low_u16(vInt16));
		return vcvtq_f32_u32(vInt);
#elif defined(_XM_SSE_INTRINSICS_)
		static const XMVECTORF32 LoadUByte4Mul = { { { 1.0f, 1.0f / 256.0f, 1.0f / 65536.0f, 1.0f / (65536.0f*256.0f) } } };
		// Splat the color in all four entries (x,z,y,w)
		XMVECTOR vTemp = _mm_load1_ps(reinterpret_cast<const float *>(&pSource->x));
		// Mask x&0ff,y&0xff00,z&0xff0000,w&0xff000000
		vTemp = _mm_and_ps(vTemp, g_XMMaskByte4);
		// w is signed! Flip the bits to convert the order to unsigned
		vTemp = _mm_xor_ps(vTemp, g_XMFlipW);
		// Convert to floating point numbers
		vTemp = _mm_cvtepi32_ps(_mm_castps_si128(vTemp));
		// w + 0x80 to complete the conversion
		vTemp = _mm_add_ps(vTemp, g_XMAddUDec4);
		// Fix y, z and w because they are too large
		vTemp = _mm_mul_ps(vTemp, LoadUByte4Mul);
		return vTemp;
#endif
	}

	//------------------------------------------------------------------------------
	_Use_decl_annotations_
	inline XMVECTOR XM_CALLCONV XMLoadSInt4
	(
		const XMINT4* pSource
	)
	{
		assert(pSource);
#if defined(_XM_NO_INTRINSICS_)

		XMVECTOR V;
		V.vector4_f32[0] = static_cast<float>(pSource->x);
		V.vector4_f32[1] = static_cast<float>(pSource->y);
		V.vector4_f32[2] = static_cast<float>(pSource->z);
		V.vector4_f32[3] = static_cast<float>(pSource->w);
		return V;

#elif defined(_XM_ARM_NEON_INTRINSICS_)
		int32x4_t v = vld1q_s32(reinterpret_cast<const int32_t*>(pSource));
		return vcvtq_f32_s32(v);
#elif defined(_XM_SSE_INTRINSICS_)
		__m128i V = _mm_loadu_si128(reinterpret_cast<const __m128i*>(pSource));
		return _mm_cvtepi32_ps(V);
#endif
	}

	//------------------------------------------------------------------------------
	// Return the X component in an FPU register. 
	inline float XM_CALLCONV XMVectorGetX(FXMVECTOR V)
	{
#if defined(_XM_NO_INTRINSICS_)
		return V.vector4_f32[0];
#elif defined(_XM_ARM_NEON_INTRINSICS_)
		return vgetq_lane_f32(V, 0);
#elif defined(_XM_SSE_INTRINSICS_)
		return _mm_cvtss_f32(V);
#endif
	}

	inline PackedVector::HALF XMConvertFloatToHalf
	(
		float Value
	)
	{
#if defined(_XM_F16C_INTRINSICS_) && !defined(_XM_NO_INTRINSICS_)
		__m128 V1 = _mm_set_ss(Value);
		__m128i V2 = _mm_cvtps_ph(V1, 0);
		return static_cast<HALF>(_mm_cvtsi128_si32(V2));
#elif defined(_XM_ARM_NEON_INTRINSICS_) && (defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64)) && !defined(_XM_NO_INTRINSICS_)
		float32x4_t vFloat = vdupq_n_f32(Value);
		float16x4_t vHalf = vcvt_f16_f32(vFloat);
		return vget_lane_u16(vreinterpret_u16_f16(vHalf), 0);
#else
		uint32_t Result;

		auto IValue = reinterpret_cast<uint32_t *>(&Value)[0];
		uint32_t Sign = (IValue & 0x80000000U) >> 16U;
		IValue = IValue & 0x7FFFFFFFU;      // Hack off the sign

		if (IValue > 0x477FE000U)
		{
			// The number is too large to be represented as a half.  Saturate to infinity.
			if (((IValue & 0x7F800000) == 0x7F800000) && ((IValue & 0x7FFFFF) != 0))
			{
				Result = 0x7FFF; // NAN
			}
			else
			{
				Result = 0x7C00U; // INF
			}
		}
		else if (!IValue)
		{
			Result = 0;
		}
		else
		{
			if (IValue < 0x38800000U)
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
		return static_cast<PackedVector::HALF>(Result | Sign);
#endif // !_XM_F16C_INTRINSICS_
	}

	inline float XMConvertHalfToFloat
	(
		PackedVector::HALF Value
	)
	{
#if defined(_XM_F16C_INTRINSICS_) && !defined(_XM_NO_INTRINSICS_)
		__m128i V1 = _mm_cvtsi32_si128(static_cast<int>(Value));
		__m128 V2 = _mm_cvtph_ps(V1);
		return _mm_cvtss_f32(V2);
#elif defined(_XM_ARM_NEON_INTRINSICS_) && (defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64)) && !defined(_XM_NO_INTRINSICS_)
		uint16x4_t vHalf = vdup_n_u16(Value);
		float32x4_t vFloat = vcvt_f32_f16(vreinterpret_f16_u16(vHalf));
		return vgetq_lane_f32(vFloat, 0);
#else
		auto Mantissa = static_cast<uint32_t>(Value & 0x03FF);

		uint32_t Exponent = (Value & 0x7C00);
		if (Exponent == 0x7C00) // INF/NAN
		{
			Exponent = 0x8f;
		}
		else if (Exponent != 0)  // The value is normalized
		{
			Exponent = static_cast<uint32_t>((static_cast<int>(Value) >> 10) & 0x1F);
		}
		else if (Mantissa != 0)     // The value is denormalized
		{
			// Normalize the value in the resulting float
			Exponent = 1;

			do
			{
				Exponent--;
				Mantissa <<= 1;
			} while ((Mantissa & 0x0400) == 0);

			Mantissa &= 0x03FF;
		}
		else                        // The value is zero
		{
			Exponent = static_cast<uint32_t>(-112);
		}

		uint32_t Result =
			((static_cast<uint32_t>(Value) & 0x8000) << 16) // Sign
			| ((Exponent + 112) << 23)                      // Exponent
			| (Mantissa << 13);                             // Mantissa

		return reinterpret_cast<float*>(&Result)[0];
#endif // !_XM_F16C_INTRINSICS_
	}

	//------------------------------------------------------------------------------
	_Use_decl_annotations_
	inline void XM_CALLCONV XMStoreFloat4A
	(
		XMFLOAT4A*   pDestination,
		FXMVECTOR     V
	)
	{
		assert(pDestination);
		assert((reinterpret_cast<uintptr_t>(pDestination) & 0xF) == 0);
#if defined(_XM_NO_INTRINSICS_)
		pDestination->x = V.vector4_f32[0];
		pDestination->y = V.vector4_f32[1];
		pDestination->z = V.vector4_f32[2];
		pDestination->w = V.vector4_f32[3];
#elif defined(_XM_ARM_NEON_INTRINSICS_)
		vst1q_f32_ex(reinterpret_cast<float*>(pDestination), V, 128);
#elif defined(_XM_SSE_INTRINSICS_)
		_mm_store_ps(&pDestination->x, V);
#endif
	}

	_Use_decl_annotations_
	inline void XM_CALLCONV XMStoreHalf4
	(
		PackedVector::XMHALF4* pDestination,
		FXMVECTOR V
	)
	{
		assert(pDestination);
#if defined(_XM_F16C_INTRINSICS_) && !defined(_XM_NO_INTRINSICS_)
		__m128i V1 = _mm_cvtps_ph(V, 0);
		_mm_storel_epi64(reinterpret_cast<__m128i*>(pDestination), V1);
#else
		XMFLOAT4A t;
		XMStoreFloat4A(&t, V);

		pDestination->x = XMConvertFloatToHalf(t.x);
		pDestination->y = XMConvertFloatToHalf(t.y);
		pDestination->z = XMConvertFloatToHalf(t.z);
		pDestination->w = XMConvertFloatToHalf(t.w);
#endif // !_XM_F16C_INTRINSICS_
	}

	//------------------------------------------------------------------------------

	inline XMVECTOR XM_CALLCONV XMVectorSubtract
	(
		FXMVECTOR V1,
		FXMVECTOR V2
	)
	{
#if defined(_XM_NO_INTRINSICS_)

		XMVECTORF32 Result = { { {
				V1.vector4_f32[0] - V2.vector4_f32[0],
				V1.vector4_f32[1] - V2.vector4_f32[1],
				V1.vector4_f32[2] - V2.vector4_f32[2],
				V1.vector4_f32[3] - V2.vector4_f32[3]
			} } };
		return Result.v;

#elif defined(_XM_ARM_NEON_INTRINSICS_)
		return vsubq_f32(V1, V2);
#elif defined(_XM_SSE_INTRINSICS_)
		return _mm_sub_ps(V1, V2);
#endif
	}

	//------------------------------------------------------------------------------

	inline XMVECTOR XM_CALLCONV XMVector3Dot
	(
		FXMVECTOR V1,
		FXMVECTOR V2
	)
	{
#if defined(_XM_NO_INTRINSICS_)
		float fValue = V1.vector4_f32[0] * V2.vector4_f32[0] + V1.vector4_f32[1] * V2.vector4_f32[1] + V1.vector4_f32[2] * V2.vector4_f32[2];
		XMVECTORF32 vResult;
		vResult.f[0] =
			vResult.f[1] =
			vResult.f[2] =
			vResult.f[3] = fValue;
		return vResult.v;
#elif defined(_XM_ARM_NEON_INTRINSICS_)
		float32x4_t vTemp = vmulq_f32(V1, V2);
		float32x2_t v1 = vget_low_f32(vTemp);
		float32x2_t v2 = vget_high_f32(vTemp);
		v1 = vpadd_f32(v1, v1);
		v2 = vdup_lane_f32(v2, 0);
		v1 = vadd_f32(v1, v2);
		return vcombine_f32(v1, v1);
#elif defined(_XM_SSE4_INTRINSICS_)
		return _mm_dp_ps(V1, V2, 0x7f);
#elif defined(_XM_SSE3_INTRINSICS_)
		XMVECTOR vTemp = _mm_mul_ps(V1, V2);
		vTemp = _mm_and_ps(vTemp, g_XMMask3);
		vTemp = _mm_hadd_ps(vTemp, vTemp);
		return _mm_hadd_ps(vTemp, vTemp);
#elif defined(_XM_SSE_INTRINSICS_)
		// Perform the dot product
		XMVECTOR vDot = _mm_mul_ps(V1, V2);
		// x=Dot.vector4_f32[1], y=Dot.vector4_f32[2]
		XMVECTOR vTemp = XM_PERMUTE_PS(vDot, _MM_SHUFFLE(2, 1, 2, 1));
		// Result.vector4_f32[0] = x+y
		vDot = _mm_add_ss(vDot, vTemp);
		// x=Dot.vector4_f32[2]
		vTemp = XM_PERMUTE_PS(vTemp, _MM_SHUFFLE(1, 1, 1, 1));
		// Result.vector4_f32[0] = (x+y)+z
		vDot = _mm_add_ss(vDot, vTemp);
		// Splat x
		return XM_PERMUTE_PS(vDot, _MM_SHUFFLE(0, 0, 0, 0));
#endif
	}

	//------------------------------------------------------------------------------

	inline XMVECTOR XM_CALLCONV XMVector4Dot
	(
		FXMVECTOR V1,
		FXMVECTOR V2
	)
	{
#if defined(_XM_NO_INTRINSICS_)

		XMVECTORF32 Result;
		Result.f[0] =
			Result.f[1] =
			Result.f[2] =
			Result.f[3] = V1.vector4_f32[0] * V2.vector4_f32[0] + V1.vector4_f32[1] * V2.vector4_f32[1] + V1.vector4_f32[2] * V2.vector4_f32[2] + V1.vector4_f32[3] * V2.vector4_f32[3];
		return Result.v;

#elif defined(_XM_ARM_NEON_INTRINSICS_)
		float32x4_t vTemp = vmulq_f32(V1, V2);
		float32x2_t v1 = vget_low_f32(vTemp);
		float32x2_t v2 = vget_high_f32(vTemp);
		v1 = vadd_f32(v1, v2);
		v1 = vpadd_f32(v1, v1);
		return vcombine_f32(v1, v1);
#elif defined(_XM_SSE4_INTRINSICS_)
		return _mm_dp_ps(V1, V2, 0xff);
#elif defined(_XM_SSE3_INTRINSICS_)
		XMVECTOR vTemp = _mm_mul_ps(V1, V2);
		vTemp = _mm_hadd_ps(vTemp, vTemp);
		return _mm_hadd_ps(vTemp, vTemp);
#elif defined(_XM_SSE_INTRINSICS_)
		XMVECTOR vTemp2 = V2;
		XMVECTOR vTemp = _mm_mul_ps(V1, vTemp2);
		vTemp2 = _mm_shuffle_ps(vTemp2, vTemp, _MM_SHUFFLE(1, 0, 0, 0)); // Copy X to the Z position and Y to the W position
		vTemp2 = _mm_add_ps(vTemp2, vTemp);          // Add Z = X+Z; W = Y+W;
		vTemp = _mm_shuffle_ps(vTemp, vTemp2, _MM_SHUFFLE(0, 3, 0, 0));  // Copy W to the Z position
		vTemp = _mm_add_ps(vTemp, vTemp2);           // Add Z and W together
		return XM_PERMUTE_PS(vTemp, _MM_SHUFFLE(2, 2, 2, 2));    // Splat Z and return
#endif
	}
}