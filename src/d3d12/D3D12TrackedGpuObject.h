#pragma once

#include "d3d12/D3D12FixedDescriptorHeap.h"
#include "d3d12/D3D12FrameCodes.h"
#include "d3d12/D3D12Fwd.h"

#include <type_traits>

#include <wrl/client.h>

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

    [[nodiscard]] bool operator==(const TrackedDeviceChild& right) const { return mDeviceChild == right.mDeviceChild; }
    [[nodiscard]] bool operator==(const Microsoft::WRL::ComPtr<ID3D12DeviceChild>& right) const
    {
        return mDeviceChild == right;
    }
    [[nodiscard]] bool operator==(std::nullptr_t) const { return mDeviceChild == nullptr; }
    [[nodiscard]] bool operator!=(std::nullptr_t) const { return mDeviceChild != nullptr; }

    ID3D12DeviceChild* operator->() const { return mDeviceChild.Get(); }

    explicit operator ID3D12DeviceChild*() const { return mDeviceChild.Get(); }
    [[nodiscard]] ID3D12DeviceChild* getDeviceChild() const { return mDeviceChild.Get(); }

    template<class T>
    [[nodiscard]] T* getAs() const
    {
        return static_cast<T*>(mDeviceChild.Get());
    }

    void markAsUsed(ID3D12CommandQueue* commandQueue);
    void markAsUsed(ID3D12CommandList* commandList);

    [[nodiscard]] bool isInUse(ID3D12CommandQueue* commandQueue) const;
    [[nodiscard]] bool isInUse(ID3D12CommandList* commandList) const;

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

    TrackedShaderResource(const TrackedShaderResource&) = delete;
    TrackedShaderResource(TrackedShaderResource&&) = default;
    ~TrackedShaderResource() { destroy(); }

    TrackedShaderResource& operator=(const TrackedShaderResource&) = delete;
    TrackedShaderResource& operator=(TrackedShaderResource&&) = default;

    [[nodiscard]] bool operator==(const TrackedShaderResource& right) const { return mResource == right.mResource; }
    [[nodiscard]] bool operator==(const Microsoft::WRL::ComPtr<ID3D12Resource>& right) const
    {
        return mResource == right;
    }
    [[nodiscard]] bool operator==(std::nullptr_t) const { return mResource == nullptr; }
    [[nodiscard]] bool operator!=(std::nullptr_t) const { return mResource != nullptr; }

    explicit operator ID3D12Resource*() const { return mResource.Get(); }
    [[nodiscard]] ID3D12Resource* getResource() const { return mResource.Get(); }

    [[nodiscard]] const FixedDescriptorHeapReservation& getCbvSrvUavDescriptorHeapReservation() const
    {
        return mCbvSrvUavDescriptorHeapReservation;
    }
    void setCbvSrvUavDescriptorHeapReservation(FixedDescriptorHeapReservation&& descriptorHeapReservation);

    [[nodiscard]] const FixedDescriptorHeapReservation& getRtvDescriptorHeapReservation() const
    {
        return mRtvDescriptorHeapReservation;
    }
    void setRtvDescriptorHeapReservation(FixedDescriptorHeapReservation&& descriptorHeapReservation);

    [[nodiscard]] const FixedDescriptorHeapReservation& getDsvDescriptorHeapReservation() const
    {
        return mDsvDescriptorHeapReservation;
    }
    void setDsvDescriptorHeapReservation(FixedDescriptorHeapReservation&& descriptorHeapReservation);

    void markAsUsed(ID3D12CommandQueue* commandQueue);
    void markAsUsed(ID3D12CommandList* commandList);

    [[nodiscard]] bool isInUse(ID3D12CommandQueue* commandQueue) const;
    [[nodiscard]] bool isInUse(ID3D12CommandList* commandList) const;

    void reset()
    {
        destroy();
        mLastUsedRenderFrameCode = RenderFrameCode{};
        mLastUsedCopyFrameCode = CopyFrameCode{};
    }

protected:
    void destroy();

    Microsoft::WRL::ComPtr<ID3D12Resource> mResource;
    FixedDescriptorHeapReservation mCbvSrvUavDescriptorHeapReservation;
    FixedDescriptorHeapReservation mRtvDescriptorHeapReservation;
    FixedDescriptorHeapReservation mDsvDescriptorHeapReservation;
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

    [[nodiscard]] bool operator==(const TrackedGpuObject<T>& right) const { return mDeviceChild == right.mDeviceChild; }
    [[nodiscard]] bool operator==(const Microsoft::WRL::ComPtr<T>& right) const { return mDeviceChild == right; }
    [[nodiscard]] bool operator==(std::nullptr_t) const { return mDeviceChild == nullptr; }
    [[nodiscard]] bool operator!=(std::nullptr_t) const { return mDeviceChild != nullptr; }

    T* operator->() const { return get(); }
    [[nodiscard]] T* get() const { return mDeviceChild.getAs<T>(); }

    void markAsUsed(ID3D12CommandQueue* commandQueue) { mDeviceChild.markAsUsed(commandQueue); }
    void markAsUsed(ID3D12CommandList* commandList) { mDeviceChild.markAsUsed(commandList); }

    [[nodiscard]] bool isInUse(ID3D12CommandQueue* commandQueue) const { return mDeviceChild.isInUse(commandQueue); }
    [[nodiscard]] bool isInUse(ID3D12CommandList* commandList) const { return mDeviceChild.isInUse(commandList); }

    void reset() { mDeviceChild.reset(); }

private:
    TrackedDeviceChild mDeviceChild;
};
} // namespace scrap::d3d12