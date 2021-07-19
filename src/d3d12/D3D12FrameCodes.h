#pragma once

#include <cstdint>
#include <limits>

namespace scrap::d3d12
{
using FrameCodeValueType = uint64_t;

class RenderFrameCode
{
public:
    RenderFrameCode() = default;
    explicit RenderFrameCode(FrameCodeValueType value): mValue(value) {}
    RenderFrameCode(const RenderFrameCode&) = default;
    RenderFrameCode(RenderFrameCode&&) = default;
    ~RenderFrameCode() = default;

    RenderFrameCode& operator=(const RenderFrameCode&) = default;
    RenderFrameCode& operator=(RenderFrameCode&&) = default;

    RenderFrameCode& operator=(const FrameCodeValueType& value)
    {
        mValue = value;
        return *this;
    }

    explicit operator uint64_t() const { return mValue; }

    RenderFrameCode& operator++()
    {
        ++mValue;
        return *this;
    }

    RenderFrameCode operator++(int) { return RenderFrameCode(mValue++); }

    FrameCodeValueType operator*() const { return mValue; }

    bool operator==(const RenderFrameCode& right) const { return mValue == right.mValue; }
    bool operator!=(const RenderFrameCode& right) const { return mValue != right.mValue; }
    bool operator<(const RenderFrameCode& right) const { return mValue < right.mValue; }
    bool operator<=(const RenderFrameCode& right) const { return mValue <= right.mValue; }
    bool operator>(const RenderFrameCode& right) const { return mValue > right.mValue; }
    bool operator>=(const RenderFrameCode& right) const { return mValue >= right.mValue; }

private:
    FrameCodeValueType mValue{0};
};

class CopyFrameCode
{
public:
    CopyFrameCode() = default;
    explicit CopyFrameCode(FrameCodeValueType value): mValue(value) {}
    CopyFrameCode(const CopyFrameCode&) = default;
    CopyFrameCode(CopyFrameCode&&) = default;
    ~CopyFrameCode() = default;

    CopyFrameCode& operator=(const CopyFrameCode&) = default;
    CopyFrameCode& operator=(CopyFrameCode&&) = default;

    CopyFrameCode& operator=(const FrameCodeValueType& value)
    {
        mValue = value;
        return *this;
    }

    explicit operator uint64_t() const { return mValue; }

    CopyFrameCode& operator++()
    {
        ++mValue;
        return *this;
    }

    CopyFrameCode operator++(int) { return CopyFrameCode(mValue++); }

    FrameCodeValueType operator*() const { return mValue; }

    bool operator==(const CopyFrameCode& right) const { return mValue == right.mValue; }
    bool operator!=(const CopyFrameCode& right) const { return mValue != right.mValue; }
    bool operator<(const CopyFrameCode& right) const { return mValue < right.mValue; }
    bool operator<=(const CopyFrameCode& right) const { return mValue <= right.mValue; }
    bool operator>(const CopyFrameCode& right) const { return mValue > right.mValue; }
    bool operator>=(const CopyFrameCode& right) const { return mValue >= right.mValue; }

private:
    FrameCodeValueType mValue{0};
};
} // namespace scrap::d3d12