#pragma once

#include "d3d12/D3D12FixedDescriptorHeap.h"
#include "d3d12/D3D12FrameCodes.h"

#include <type_traits>

#include <wrl/client.h>

struct ID3D12CommandList;
struct ID3D12CommandQueue;
struct ID3D12DeviceChild;
struct ID3D12Resource;

namespace scrap::d3d12
{
class TrackedDeviceChild
{
public:
    TrackedDeviceChild() = default;
    explicit TrackedDeviceChild(Microsoft::WRL::ComPtr<ID3D12DeviceChild> deviceChild)
        : mDeviceChild(std::move(deviceChild))
    {}

    TrackedDeviceChild(const TrackedDeviceChild&) = delete;
    TrackedDeviceChild(TrackedDeviceChild&&) = default;
    ~TrackedDeviceChild() { destroy(); }

    TrackedDeviceChild& operator=(const TrackedDeviceChild&) = delete;
    TrackedDeviceChild& operator=(TrackedDeviceChild&&) = default;

    bool operator==(std::nullptr_t) const { return mDeviceChild == nullptr; }
    bool operator!=(std::nullptr_t) const { return mDeviceChild != nullptr; }

    ID3D12DeviceChild* operator->() const { return mDeviceChild.Get(); }

    explicit operator ID3D12DeviceChild*() const { return mDeviceChild.Get(); }
    ID3D12DeviceChild* getDeviceChild() const { return mDeviceChild.Get(); }

    template<class T>
    T* getAs() const
    {
        return static_cast<T*>(mDeviceChild.Get());
    }

    void markAsUsed(ID3D12CommandQueue* commandQueue);
    void markAsUsed(ID3D12CommandList* commandList);

    bool isInUse(ID3D12CommandQueue* commandQueue) const;
    bool isInUse(ID3D12CommandList* commandList) const;

    void reset()
    {
        destroy();

        mLastUsedRenderFrameCode = RenderFrameCode{};
        mLastUsedCopyFrameCode = CopyFrameCode{};
    }

protected:
    void destroy();

    Microsoft::WRL::ComPtr<ID3D12DeviceChild> mDeviceChild;
    RenderFrameCode mLastUsedRenderFrameCode;
    CopyFrameCode mLastUsedCopyFrameCode;
};

class TrackedShaderResource
{
public:
    TrackedShaderResource() = default;

    explicit TrackedShaderResource(Microsoft::WRL::ComPtr<ID3D12Resource> resource): mResource(std::move(resource)) {}

    explicit TrackedShaderResource(Microsoft::WRL::ComPtr<ID3D12Resource> resource,
                                   FixedDescriptorHeapReservation&& descriptorHeapReservation)
        : mResource(std::move(resource))
        , mDescriptorHeapReservation(std::move(descriptorHeapReservation))
    {}

    TrackedShaderResource(const TrackedShaderResource&) = delete;
    TrackedShaderResource(TrackedShaderResource&&) = default;
    ~TrackedShaderResource() { destroy(); }

    TrackedShaderResource& operator=(const TrackedShaderResource&) = delete;
    TrackedShaderResource& operator=(TrackedShaderResource&&) = default;

    bool operator==(std::nullptr_t) const { return mResource == nullptr; }
    bool operator!=(std::nullptr_t) const { return mResource != nullptr; }

    explicit operator ID3D12Resource*() const { return mResource.Get(); }
    ID3D12Resource* getResource() const { return mResource.Get(); }

    const FixedDescriptorHeapReservation& getDescriptorHeapReservation() const { return mDescriptorHeapReservation; }
    void setDescriptorHeapReservation(FixedDescriptorHeapReservation&& descriptorHeapReservation);

    void markAsUsed(ID3D12CommandQueue* commandQueue);
    void markAsUsed(ID3D12CommandList* commandList);

    bool isInUse(ID3D12CommandQueue* commandQueue) const;
    bool isInUse(ID3D12CommandList* commandList) const;

    void reset()
    {
        destroy();
        mLastUsedRenderFrameCode = RenderFrameCode{};
        mLastUsedCopyFrameCode = CopyFrameCode{};
    }

protected:
    void destroy();

    Microsoft::WRL::ComPtr<ID3D12Resource> mResource;
    FixedDescriptorHeapReservation mDescriptorHeapReservation;
    RenderFrameCode mLastUsedRenderFrameCode;
    CopyFrameCode mLastUsedCopyFrameCode;
};

template<class T>
class TrackedGpuObject
{
public:
    TrackedGpuObject() = default;
    explicit TrackedGpuObject(Microsoft::WRL::ComPtr<T> object): mDeviceChild(std::move(object)) {}

    TrackedGpuObject(const TrackedGpuObject&) = delete;
    TrackedGpuObject(TrackedGpuObject&&) = default;
    ~TrackedGpuObject() = default;

    TrackedGpuObject& operator=(const TrackedGpuObject&) = delete;
    TrackedGpuObject& operator=(TrackedGpuObject&&) = default;

    bool operator==(std::nullptr_t) const { return mDeviceChild == nullptr; }
    bool operator!=(std::nullptr_t) const { return mDeviceChild != nullptr; }

    T* operator->() const { return get(); }
    T* get() const { return mDeviceChild.getAs<T>(); }

    void markAsUsed(ID3D12CommandQueue* commandQueue) { mDeviceChild.markAsUsed(commandQueue); }
    void markAsUsed(ID3D12CommandList* commandList) { mDeviceChild.markAsUsed(commandList); }

    bool isInUse(ID3D12CommandQueue* commandQueue) const { return mDeviceChild.isInUse(commandQueue); }
    bool isInUse(ID3D12CommandList* commandList) const { return mDeviceChild.isInUse(commandList); }

    void reset() { mDeviceChild.reset(); }

private:
    TrackedDeviceChild mDeviceChild;
};
} // namespace scrap::d3d12