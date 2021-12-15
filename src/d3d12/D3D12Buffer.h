#pragma once

#include "RenderDefs.h"
#include "d3d12/D3D12FixedDescriptorHeap.h"
#include "d3d12/D3D12TrackedGpuObject.h"

#include <optional>
#include <span>

#include <d3d12.h>
#include <dxgiformat.h>
#include <gpufmt/format.h>
#include <wrl/client.h>

namespace scrap::d3d12
{
enum class BufferFlags
{
    None = 0,
    ConstantBuffer = 1 << 0,
    IndexBuffer = 1 << 1,
    AccelerationStructure = 1 << 2,
    UavEnabled = 1 << 3,
    Raw = 1 << 4,
    NonPixelShaderResource = 1 << 5,
};
DEFINE_ENUM_BITWISE_OPERATORS(BufferFlags);

namespace detail
{
struct BufferCommonParams
{
    ResourceAccessFlags accessFlags = ResourceAccessFlags::None;
    BufferFlags flags = BufferFlags::None;
    std::optional<D3D12_RESOURCE_STATES> initialResourceState;
    std::string_view name;
};

struct BufferElementParams
{
    uint32_t numElements = 0;
};

struct BufferByteParams
{
    uint64_t byteSize = 0;
};

struct BufferFormattedSpecificParams
{
    gpufmt::Format format = gpufmt::Format::UNDEFINED;
};

struct BufferStructuredSpecificParams
{
    uint32_t elementByteSize = 0;
};
} // namespace detail

class Buffer
{
public:
    enum class Error
    {
        InvalidFormat,
        FailedToCreateGpuResource,
        FailedToCreateUploadResource,
        InsufficientDescriptorHeapSpace
    };

    struct SimpleParams : detail::BufferCommonParams, detail::BufferByteParams
    {};

    struct FormattedParams : detail::BufferCommonParams,
                             detail::BufferElementParams,
                             detail::BufferFormattedSpecificParams
    {};

    struct StructuredParams : detail::BufferCommonParams,
                              detail::BufferElementParams,
                              detail::BufferStructuredSpecificParams
    {};

    Buffer() = default;
    Buffer(const Buffer&) = delete;
    Buffer(Buffer&&) = delete;
    ~Buffer() = default;

    Buffer& operator=(const Buffer&) = delete;
    Buffer& operator=(Buffer&&) = delete;

    std::optional<Error> init(const SimpleParams& params, std::span<const std::byte> buffer = {});
    std::optional<Error> init(const FormattedParams& params, std::span<const std::byte> buffer = {});
    std::optional<Error> init(const StructuredParams& params, std::span<const std::byte> buffer = {});

    ID3D12Resource* getResource() const { return mResource.getResource(); }

    D3D12_CPU_DESCRIPTOR_HANDLE getSrvCpu() const;
    D3D12_GPU_DESCRIPTOR_HANDLE getSrvGpu() const;
    D3D12_CPU_DESCRIPTOR_HANDLE getUavCpu() const;
    D3D12_GPU_DESCRIPTOR_HANDLE getUavGpu() const;
    D3D12_CPU_DESCRIPTOR_HANDLE getCbvCpu() const;
    D3D12_GPU_DESCRIPTOR_HANDLE getCbvGpu() const;

    uint32_t getSrvDescriptorHeapIndex() const
    {
        return mResource.getCbvSrvUavDescriptorHeapReservation().getStartHeapIndex() + mSrvIndex;
    }
    uint32_t getUavDescriptorHeapIndex() const
    {
        return mResource.getCbvSrvUavDescriptorHeapReservation().getStartHeapIndex() + mUavIndex;
    }
    uint32_t getCbvDescriptorHeapIndex() const
    {
        return mResource.getCbvSrvUavDescriptorHeapReservation().getStartHeapIndex() + mCbvIndex;
    }

    D3D12_INDEX_BUFFER_VIEW getIndexView() const;
    D3D12_INDEX_BUFFER_VIEW getIndexView(DXGI_FORMAT format) const;

    uint64_t getByteSize() const { return mParams.byteSize; }
    uint64_t getElementByteSize() const { return mParams.elementByteSize; }
    uint64_t getElementCount() const { return mParams.byteSize / mParams.elementByteSize; }
    DXGI_FORMAT getFormat() const { return mParams.dxgiFormat; }

    bool isReady() const;
    void markAsUsed(ID3D12CommandQueue* commandQueue);
    void markAsUsed(ID3D12CommandList* commandList);

    std::span<std::byte> map();

    template<class T>
    std::span<T> mapAs()
    {
        auto buffer = map();
        return std::span<T>(reinterpret_cast<T*>(buffer.data()), buffer.size_bytes() / sizeof(T));
    }

    void unmap(ID3D12GraphicsCommandList* commandList);

    void transition(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);

private:
    enum class Type
    {
        Unknown,
        Simple,
        Formatted,
        Structured
    };

    struct Params : detail::BufferCommonParams,
                    detail::BufferByteParams,
                    detail::BufferElementParams,
                    detail::BufferFormattedSpecificParams,
                    detail::BufferStructuredSpecificParams
    {
        Params() = default;
        Params(const SimpleParams& params)
            : detail::BufferCommonParams(params)
            , detail::BufferByteParams(params)
            , type(Type::Simple)
        {}

        Params(const FormattedParams& params)
            : detail::BufferCommonParams(params)
            , detail::BufferElementParams(params)
            , detail::BufferFormattedSpecificParams(params)
            , type(Type::Formatted)
        {}

        Params(const StructuredParams& params)
            : detail::BufferCommonParams(params)
            , detail::BufferElementParams(params)
            , detail::BufferStructuredSpecificParams(params)
            , type(Type::Structured)
        {}

        DXGI_FORMAT dxgiFormat = DXGI_FORMAT_UNKNOWN;
        Type type = Type::Unknown;
    };

    std::optional<Error> initInternal(Params params, std::span<const std::byte> buffer);

    Params mParams;
    TrackedShaderResource mResource;
    TrackedGpuObject<ID3D12Resource> mUploadResource;
    CopyFrameCode mInitFrameCode;
    uint32_t mSrvIndex = 0;
    uint32_t mUavIndex = 0;
    uint32_t mCbvIndex = 0;
    bool mInitialized = false;
};

template<class T>
class GpuBufferWriteGuard
{
public:
    GpuBufferWriteGuard() = default;
    GpuBufferWriteGuard(T& gpuBuffer, ID3D12GraphicsCommandList* commandList)
        : mGpuBuffer(&gpuBuffer)
        , mCommandList(commandList)
    {
        mWriteBuffer = mGpuBuffer->map();
    }

    GpuBufferWriteGuard(const GpuBufferWriteGuard<T>&) = delete;
    GpuBufferWriteGuard(GpuBufferWriteGuard<T>&& other)
        : mGpuBuffer(other.mGpuBuffer)
        , mCommandList(other.mCommandList)
        , mWriteBuffer(std::move(other.mWriteBuffer))
    {
        other.mGpuBuffer = nullptr;
        other.mCommandList = nullptr;
    }

    ~GpuBufferWriteGuard()
    {
        if(mGpuBuffer != nullptr) { mGpuBuffer->unmap(mCommandList); }
    }

    GpuBufferWriteGuard& operator=(const GpuBufferWriteGuard<T>&) = delete;
    GpuBufferWriteGuard& operator=(GpuBufferWriteGuard<T>&& other)
    {
        if(mGpuBuffer != nullptr) { mGpuBuffer->unmap(mCommandList); }
        mGpuBuffer = other.mGpuBuffer;
        other.mGpuBuffer = nullptr;

        mCommandList = other.mCommandList;
        other.mCommandList = nullptr;

        mWriteBuffer = std::move(other.mWriteBuffer);

        return *this;
    }

    std::span<std::byte> getWriteBuffer() { return mWriteBuffer; }

    template<class U>
    std::span<U> getWriteBufferAs()
    {
        return std::span<U>(reinterpret_cast<U*>(mWriteBuffer.data()), mWriteBuffer.size_bytes() / sizeof(U));
    }

private:
    T* mGpuBuffer = nullptr;
    ID3D12GraphicsCommandList* mCommandList = nullptr;
    std::span<std::byte> mWriteBuffer;
};
} // namespace scrap::d3d12