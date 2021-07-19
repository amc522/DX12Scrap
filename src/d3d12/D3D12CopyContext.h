#pragma once

#include "d3d12/D3D12Config.h"
#include "d3d12/D3D12FrameCodes.h"

#include <array>
#include <vector>

#include <wrl/client.h>

struct ID3D12CommandAllocator;
struct ID3D12GraphicsCommandList;
struct ID3D12CommandQueue;
struct ID3D12Fence;
struct ID3D12Resource;

namespace scrap::d3d12
{
class DeviceContext;

class CopyContext
{
public:
    CopyContext() = default;
    CopyContext(const CopyContext&) = delete;
    CopyContext(CopyContext&&) = default;
    ~CopyContext();

    CopyContext& operator=(const CopyContext&) = delete;
    CopyContext& operator=(CopyContext&& other) = delete;

    HRESULT init(DeviceContext& deviceContext);

    ID3D12CommandQueue* getCommandQueue() const;
    ID3D12GraphicsCommandList* getCommandList() const;
    void trackCopyResource(Microsoft::WRL::ComPtr<ID3D12Resource> sourceResource,
                           Microsoft::WRL::ComPtr<ID3D12Resource> destResource);

    void beginCopyFrame();
    void endCopyFrame();

    void waitOnGpu();

    CopyFrameCode getCurrentFrameCode() const { return mFenceValues[mFrameIndex]; }
    CopyFrameCode getLastCompletedFrameCode() const { return mLastCompletedFrameCode; }

private:
    struct PendingResource
    {
        PendingResource(Microsoft::WRL::ComPtr<ID3D12Resource>&& source,
                        Microsoft::WRL::ComPtr<ID3D12Resource>&& dest,
                        CopyFrameCode frameCode)
            : sourceResource(std::move(source))
            , destResource(std::move(dest))
            , copyFrameCode(frameCode)
        {}

        Microsoft::WRL::ComPtr<ID3D12Resource> sourceResource;
        Microsoft::WRL::ComPtr<ID3D12Resource> destResource;
        CopyFrameCode copyFrameCode{std::numeric_limits<uint64_t>::max()};
    };

    DeviceContext* mDeviceContext = nullptr;

    Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCopyQueue;
    std::array<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>, kFrameBufferCount> mCommandAllocators;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;

    uint32_t mFrameIndex = 0;
    Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
    std::array<CopyFrameCode, kFrameBufferCount> mFenceValues;
    HANDLE mFenceEvent = nullptr;

    CopyFrameCode mLastCompletedFrameCode;

    std::vector<PendingResource> mPendingResources;
};
} // namespace scrap::d3d12