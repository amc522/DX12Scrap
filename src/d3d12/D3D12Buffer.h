#pragma once

#include "RenderDefs.h"
#include "d3d12/D3D12FixedDescriptorHeap.h"
#include "d3d12/D3D12TrackedGpuObject.h"

#include <optional>

#include <d3d12.h>
#include <dxgiformat.h>
#include <gpufmt/format.h>
#include <nonstd/span.hpp>
#include <wrl/client.h>

struct D3D12_INDEX_BUFFER_VIEW;
struct ID3D12GraphicsCommandList;
struct ID3D12Resource;

namespace scrap::d3d12
{
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

    struct CommonParams
    {
        ResourceAccessFlags accessFlags;
        std::string_view name;
    };

    struct ElementBased
    {
        uint32_t numElements = 0;
    };

    struct ByteBased
    {
        uint32_t byteSize = 0;
        bool isConstantBuffer = false;
    };

    struct FormattedSpecificParams
    {
        gpufmt::Format format = gpufmt::Format::UNDEFINED;
    };

    struct StructuredSpecificParams
    {
        uint32_t elementByteSize = 0;
    };

    struct SimpleParams : CommonParams, ByteBased
    {};
    struct FormattedParams : CommonParams, ElementBased, FormattedSpecificParams
    {};
    struct StructuredParams : CommonParams, ElementBased, StructuredSpecificParams
    {};

    Buffer() = default;
    Buffer(const Buffer&) = delete;
    Buffer(Buffer&&) = delete;
    ~Buffer() = default;

    Buffer& operator=(const Buffer&) = delete;
    Buffer& operator=(Buffer&&) = delete;

    std::optional<Error> init(const SimpleParams& params, nonstd::span<const std::byte> buffer = {});
    std::optional<Error> init(const FormattedParams& params, nonstd::span<const std::byte> buffer = {});
    std::optional<Error> init(const StructuredParams& params, nonstd::span<const std::byte> buffer = {});

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

    bool isReady() const;
    void markAsUsed(ID3D12CommandQueue* commandQueue);
    void markAsUsed(ID3D12CommandList* commandList);

    nonstd::span<std::byte> map();

    template<class T>
    nonstd::span<T> mapAs()
    {
        auto buffer = map();
        return nonstd::span<T>(reinterpret_cast<T*>(buffer.data()), buffer.size_bytes() / sizeof(T));
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

    struct Params : CommonParams, ByteBased, ElementBased, FormattedSpecificParams, StructuredSpecificParams
    {
        Params() = default;
        Params(const SimpleParams& params): CommonParams(params), ByteBased(params), type(Type::Simple) {}

        Params(const FormattedParams& params)
            : CommonParams(params)
            , ElementBased(params)
            , FormattedSpecificParams(params)
            , type(Type::Formatted)
        {}

        Params(const StructuredParams& params)
            : CommonParams(params)
            , ElementBased(params)
            , StructuredSpecificParams(params)
            , type(Type::Structured)
        {}

        DXGI_FORMAT dxgiFormat = DXGI_FORMAT_UNKNOWN;
        Type type = Type::Unknown;
    };

    std::optional<Error> initInternal(Params params, nonstd::span<const std::byte> buffer);

    Params mParams;
    TrackedShaderResource mResource;
    TrackedGpuObject<ID3D12Resource> mUploadResource;
    CopyFrameCode mInitFrameCode;
    uint32_t mSrvIndex = 0;
    uint32_t mUavIndex = 0;
    uint32_t mCbvIndex = 0;
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
    }

    nonstd::span<std::byte> getWriteBuffer() { return mWriteBuffer; }

    template<class U>
    nonstd::span<U> getWriteBufferAs()
    {
        return nonstd::span<U>(reinterpret_cast<U*>(mWriteBuffer.data()), mWriteBuffer.size_bytes() / sizeof(U));
    }

private:
    T* mGpuBuffer = nullptr;
    ID3D12GraphicsCommandList* mCommandList = nullptr;
    nonstd::span<std::byte> mWriteBuffer;
};
} // namespace scrap::d3d12