#pragma once

#include <cassert>
#include <memory>
#include <type_traits>

#define DEFINE_ENUM_BITWISE_OPERATORS(EnumClassName)                                                  \
    constexpr inline EnumClassName operator|(EnumClassName left, EnumClassName right)                 \
    {                                                                                                 \
        return static_cast<EnumClassName>(static_cast<std::underlying_type_t<EnumClassName>>(left) |  \
                                          static_cast<std::underlying_type_t<EnumClassName>>(right)); \
    }                                                                                                 \
    constexpr inline EnumClassName operator&(EnumClassName left, EnumClassName right)                 \
    {                                                                                                 \
        return static_cast<EnumClassName>(static_cast<std::underlying_type_t<EnumClassName>>(left) &  \
                                          static_cast<std::underlying_type_t<EnumClassName>>(right)); \
    }                                                                                                 \
    constexpr inline EnumClassName operator^(EnumClassName left, EnumClassName right)                 \
    {                                                                                                 \
        return static_cast<EnumClassName>(static_cast<std::underlying_type_t<EnumClassName>>(left) ^  \
                                          static_cast<std::underlying_type_t<EnumClassName>>(right)); \
    }                                                                                                 \
    constexpr inline EnumClassName operator~(EnumClassName val)                                       \
    {                                                                                                 \
        return static_cast<EnumClassName>(~static_cast<std::underlying_type_t<EnumClassName>>(val));  \
    }                                                                                                 \
    constexpr inline EnumClassName& operator|=(EnumClassName& left, EnumClassName right)              \
    {                                                                                                 \
        left = static_cast<EnumClassName>(static_cast<std::underlying_type_t<EnumClassName>>(left) |  \
                                          static_cast<std::underlying_type_t<EnumClassName>>(right)); \
        return left;                                                                                  \
    }                                                                                                 \
    constexpr inline EnumClassName& operator&=(EnumClassName& left, EnumClassName right)              \
    {                                                                                                 \
        left = static_cast<EnumClassName>(static_cast<std::underlying_type_t<EnumClassName>>(left) &  \
                                          static_cast<std::underlying_type_t<EnumClassName>>(right)); \
        return left;                                                                                  \
    }                                                                                                 \
    constexpr inline EnumClassName& operator^=(EnumClassName& left, EnumClassName right)              \
    {                                                                                                 \
        left = static_cast<EnumClassName>(static_cast<std::underlying_type_t<EnumClassName>>(left) ^  \
                                          static_cast<std::underlying_type_t<EnumClassName>>(right)); \
        return left;                                                                                  \
    }

namespace scrap
{
template<class EnumT> requires std::is_enum_v<EnumT>
constexpr std::enable_if_t<std::is_enum_v<EnumT>, std::underlying_type_t<EnumT>> ToUnderlying(EnumT e)
{
    return static_cast<std::underlying_type_t<EnumT>>(e);
}

template<class EnumT> requires std::is_enum_v<EnumT>
constexpr std::enable_if_t<std::is_enum_v<EnumT>, EnumT> IncrementEnum(EnumT e)
{
    return static_cast<EnumT>(ToUnderlying(e) + std::underlying_type_t<EnumT>(1));
}

template<class... Ts>
struct Overloaded : Ts...
{
    using Ts::operator()...;
};
template<class... Ts>
Overloaded(Ts...) -> Overloaded<Ts...>;

template<typename T, size_t N>
constexpr size_t GetArraySize(const T (&)[N])
{
    return N;
}

template<std::integral T>
constexpr T AlignInteger(T value, T alignment)
{
    return (value + (alignment - T(1))) & (~(alignment - T(1)));
}

class MemoryCalculator
{
public:
    template<class T>
    constexpr size_t add(size_t count)
    {
        return mByteSize += sizeof(T) * count;
    }

    template<class T>
    constexpr size_t addStdVector(size_t elementCount)
    {
        // The MSVC implementation of std::vector makes an allocation for each construction and move in debug. As of
        // 12/26/2021, each allocation is 16 bytes. This tries to account for a construction and move. It will fail if
        // more moves happen, but this was the best option I could think of for now.
#ifdef _MSVC_LANG
#    if _ITERATOR_DEBUG_LEVEL > 0
        constexpr bool kAccountForDebugAllocations = true;
#    else
        constexpr bool kAccountForDebugAllocations = false;
#    endif
#else
        constexpr bool kAccountForDebugAllocations = false;
#endif

        if constexpr(kAccountForDebugAllocations) { mByteSize += 32; }
        return mByteSize += sizeof(T) * elementCount;
    }

    template<class T>
    constexpr size_t addAligned(size_t count)
    {
        return mByteSize += std::alignment_of_v<T> * count;
    }

    size_t getByteSize() const { return mByteSize; }

private:
    size_t mByteSize = 0;
};

class MonotonicBuffer
{
public:
    MonotonicBuffer() = default;
    MonotonicBuffer(size_t byteSize): mBuffer(std::make_unique<std::byte[]>(byteSize)), mByteCapacity(byteSize) {}

    [[nodiscard]] std::byte* allocate(size_t bytes) const
    {
        assert(mByteOffset + bytes <= mByteCapacity);
        std::byte* ret = mBuffer.get() + mByteOffset;
        mByteOffset += bytes;
        return ret;
    }

    template<class T>
    [[nodiscard]] T* allocate(size_t count) const
    {
        return reinterpret_cast<T*>(allocate(sizeof(T) * count));
    }

private:
    std::unique_ptr<std::byte[]> mBuffer;
    mutable size_t mByteCapacity = 0;
    mutable size_t mByteOffset = 0;
};

template<class T>
class MonotonicBufferStdAllocator
{
public:
    using value_type = T;

    MonotonicBufferStdAllocator(const MonotonicBuffer& buffer): mMonotonicBuffer(&buffer) {}

    template<class U>
    MonotonicBufferStdAllocator(const MonotonicBufferStdAllocator<U>& other) noexcept
        : mMonotonicBuffer(other.mMonotonicBuffer)
    {}

    template<class U>
    [[nodiscard]] bool operator==(const MonotonicBufferStdAllocator<U>& other) const noexcept
    {
        return mMonotonicBuffer == other.mMonotonicBuffer;
    }

    template<class U>
    [[nodiscard]] bool operator!=(const MonotonicBufferStdAllocator<U>& other) const noexcept
    {
        return !this->operator==(other);
    }

    // std::allocate
    T* allocate(size_t n) noexcept { return mMonotonicBuffer->allocate<T>(n); }

    // std::deallocate
    void deallocate(T* const, size_t) const noexcept {}

    const MonotonicBuffer* mMonotonicBuffer = nullptr;
};

template<class T>
class MonotonicBufferEastlAllocator
{
public:
    using value_type = T;

    MonotonicBufferEastlAllocator(const MonotonicBuffer& buffer): mMonotonicBuffer(&buffer) {}

    template<class U>
    MonotonicBufferEastlAllocator(const MonotonicBufferEastlAllocator<U>& other) noexcept
        : mMonotonicBuffer(other.mMonotonicBuffer)
    {}

    template<class U>
    [[nodiscard]] bool operator==(const MonotonicBufferEastlAllocator<U>& other) const noexcept
    {
        return mMonotonicBuffer == other.mMonotonicBuffer;
    }

    template<class U>
    [[nodiscard]] bool operator!=(const MonotonicBufferEastlAllocator<U>& other) const noexcept
    {
        return !this->operator==(other);
    }

    // eastl::allocate
    void* allocate(size_t bytes, int flags = 0) noexcept { return mMonotonicBuffer->allocate(bytes); }

    // eastl::allocate
    void* allocate(size_t bytes, size_t alignment, size_t offset, int flags = 0) noexcept
    {
        return mMonotonicBuffer->allocate(bytes);
    }

    // eastl::deallocate
    void deallocate(void* const, size_t) const noexcept {}

    const MonotonicBuffer* mMonotonicBuffer = nullptr;
};
} // namespace scrap
