//-------------------------------------------------------------------------------------
// BC.h
//  
// Block-compression (BC) functionality
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=248926
//-------------------------------------------------------------------------------------

#pragma once

#include <assert.h>

#include "DirectXTexStub.h"

namespace DirectX
{
//-------------------------------------------------------------------------------------
// Macros
//-------------------------------------------------------------------------------------

// Because these are used in SAL annotations, they need to remain macros rather than const values
#define NUM_PIXELS_PER_BLOCK 16

//-------------------------------------------------------------------------------------
// Structures
//-------------------------------------------------------------------------------------
class LDRColorA;

class HDRColor
{
public:
    float r, g, b;

public:
    HDRColor() = default;
    HDRColor(float _r, float _g, float _b) : r(_r), g(_g), b(_b) {}
    HDRColor(const HDRColor &c) : r(c.r), g(c.g), b(c.b) {}

    // binary operators
    HDRColor operator + (const HDRColor &c) const
    {
        return HDRColor(r + c.r, g + c.g, b + c.b);
    }

    HDRColor operator - (const HDRColor &c) const
    {
        return HDRColor(r - c.r, g - c.g, b - c.b);
    }

    HDRColor operator * (float f) const
    {
        return HDRColor(r * f, g * f, b * f);
    }

    HDRColor operator / (float f) const
    {
        float fInv = 1.0f / f;
        return HDRColor(r * fInv, g * fInv, b * fInv);
    }

    float operator * (const HDRColor &c) const
    {
        return r * c.r + g * c.g + b * c.b;
    }

    // assignment operators
    HDRColor &operator += (const HDRColor &c)
    {
        r += c.r;
        g += c.g;
        b += c.b;
        return *this;
    }

    HDRColor &operator -= (const HDRColor &c)
    {
        r -= c.r;
        g -= c.g;
        b -= c.b;
        return *this;
    }

    HDRColor &operator *= (float f)
    {
        r *= f;
        g *= f;
        b *= f;
        return *this;
    }

    HDRColor &operator /= (float f)
    {
        float fInv = 1.0f / f;
        r *= fInv;
        g *= fInv;
        b *= fInv;
        return *this;
    }

    HDRColor &Clamp(_In_ float fMin, _In_ float fMax)
    {
        r = std::min<float>(fMax, std::max<float>(fMin, r));
        g = std::min<float>(fMax, std::max<float>(fMin, g));
        b = std::min<float>(fMax, std::max<float>(fMin, b));
        return *this;
    }

    HDRColor(const LDRColorA &c);
    HDRColor &operator = (const LDRColorA &c);
    LDRColorA ToLDRColorA() const;
};

static_assert(sizeof(HDRColor) == 12);

class HDRColorA
{
public:
    float r, g, b, a;

public:
    HDRColorA() = default;
    HDRColorA(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a) {}
    HDRColorA(const HDRColorA& c) : r(c.r), g(c.g), b(c.b), a(c.a) {}

    // binary operators
    HDRColorA operator + (const HDRColorA& c) const
    {
        return HDRColorA(r + c.r, g + c.g, b + c.b, a + c.a);
    }

    HDRColorA operator - (const HDRColorA& c) const
    {
        return HDRColorA(r - c.r, g - c.g, b - c.b, a - c.a);
    }

    HDRColorA operator * (float f) const
    {
        return HDRColorA(r * f, g * f, b * f, a * f);
    }

    HDRColorA operator / (float f) const
    {
        float fInv = 1.0f / f;
        return HDRColorA(r * fInv, g * fInv, b * fInv, a * fInv);
    }

    float operator * (const HDRColorA& c) const
    {
        return r * c.r + g * c.g + b * c.b + a * c.a;
    }

    // assignment operators
    HDRColorA& operator += (const HDRColorA& c)
    {
        r += c.r;
        g += c.g;
        b += c.b;
        a += c.a;
        return *this;
    }

    HDRColorA& operator -= (const HDRColorA& c)
    {
        r -= c.r;
        g -= c.g;
        b -= c.b;
        a -= c.a;
        return *this;
    }

    HDRColorA& operator *= (float f)
    {
        r *= f;
        g *= f;
        b *= f;
        a *= f;
        return *this;
    }

    HDRColorA& operator /= (float f)
    {
        float fInv = 1.0f / f;
        r *= fInv;
        g *= fInv;
        b *= fInv;
        a *= fInv;
        return *this;
    }

    HDRColorA& Clamp(_In_ float fMin, _In_ float fMax)
    {
        r = std::min<float>(fMax, std::max<float>(fMin, r));
        g = std::min<float>(fMax, std::max<float>(fMin, g));
        b = std::min<float>(fMax, std::max<float>(fMin, b));
        a = std::min<float>(fMax, std::max<float>(fMin, a));
        return *this;
    }

    HDRColorA(const LDRColorA& c);
    HDRColorA& operator = (const LDRColorA& c);
    LDRColorA ToLDRColorA() const;
};

class LDRColorA
{
public:
    uint8_t r, g, b, a;

    LDRColorA() = default;
    LDRColorA(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a) : r(_r), g(_g), b(_b), a(_a) {}

    const uint8_t &operator [] (_In_range_(0, 3) size_t uElement) const
    {
        switch(uElement)
        {
        case 0: return r;
        case 1: return g;
        case 2: return b;
        case 3: return a;
        default: assert(false); return r;
        }
    }

    uint8_t &operator [] (_In_range_(0, 3) size_t uElement)
    {
        switch(uElement)
        {
        case 0: return r;
        case 1: return g;
        case 2: return b;
        case 3: return a;
        default: assert(false); return r;
        }
    }

    LDRColorA operator = (_In_ const HDRColorA &c)
    {
        LDRColorA ret;
        HDRColorA tmp(c);
        tmp = tmp.Clamp(0.0f, 1.0f) * 255.0f;
        ret.r = uint8_t(tmp.r + 0.001f);
        ret.g = uint8_t(tmp.g + 0.001f);
        ret.b = uint8_t(tmp.b + 0.001f);
        ret.a = uint8_t(tmp.a + 0.001f);
        return ret;
    }

    static void InterpolateRGB(_In_ const LDRColorA &c0, _In_ const LDRColorA &c1, _In_ size_t wc, _In_ _In_range_(2, 4) size_t wcprec, _Out_ LDRColorA &out);

    static void InterpolateA(_In_ const LDRColorA &c0, _In_ const LDRColorA &c1, _In_ size_t wa, _In_range_(2, 4) _In_ size_t waprec, _Out_ LDRColorA &out);

    static void Interpolate(_In_ const LDRColorA &c0, _In_ const LDRColorA &c1, _In_ size_t wc, _In_ size_t wa, _In_ _In_range_(2, 4) size_t wcprec, _In_ _In_range_(2, 4) size_t waprec, _Out_ LDRColorA &out)
    {
        InterpolateRGB(c0, c1, wc, wcprec, out);
        InterpolateA(c0, c1, wa, waprec, out);
    }
};

static_assert(sizeof(LDRColorA) == 4, "Unexpected packing");

//-------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------

void D3DXDecodeBC6HU(_Out_writes_(NUM_PIXELS_PER_BLOCK) XMVECTOR *pColor, _In_reads_(16) const uint8_t *pBC);
void D3DXDecodeBC6HS(_Out_writes_(NUM_PIXELS_PER_BLOCK) XMVECTOR *pColor, _In_reads_(16) const uint8_t *pBC);
void D3DXDecodeBC6HU(_Out_writes_(NUM_PIXELS_PER_BLOCK) HDRColor *pColor, _In_reads_(16) const uint8_t *pBC);
void D3DXDecodeBC6HS(_Out_writes_(NUM_PIXELS_PER_BLOCK) HDRColor *pColor, _In_reads_(16) const uint8_t *pBC);
void D3DXDecodeBC6HU(_Out_writes_(NUM_PIXELS_PER_BLOCK) PackedVector::XMHALF4 *pColor, _In_reads_(16) const uint8_t *pBC);
void D3DXDecodeBC6HS(_Out_writes_(NUM_PIXELS_PER_BLOCK) PackedVector::XMHALF4 *pColor, _In_reads_(16) const uint8_t *pBC);
void D3DXDecodeBC7(_Out_writes_(NUM_PIXELS_PER_BLOCK) XMVECTOR *pColor, _In_reads_(16) const uint8_t *pBC);
void D3DXDecodeBC7(_Out_writes_(NUM_PIXELS_PER_BLOCK) LDRColorA *pColor, _In_reads_(16) const uint8_t *pBC);

} // namespace
