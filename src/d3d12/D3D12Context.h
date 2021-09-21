// DeviceContext contains all the shared objects for the D3D12 pipeline. This includes (but is not limited to) the
// hardware adapter, device, and swap chains. Specific D3D12 rendering related functions should NOT be implemented here.
// DeviceContext is not responsible for loading and managing shaders, pipeline states, or resources.

#pragma once

#include "RenderDefs.h"
#include "d3d12/D3D12Config.h"
#include "d3d12/D3D12CopyContext.h"
#include "d3d12/D3D12Debug.h"
#include "d3d12/D3D12FixedDescriptorHeap.h"
#include "d3d12/D3D12FrameCodes.h"
#include "d3d12/D3D12GraphicsContext.h"
#include "d3d12/D3D12MonotonicDescriptorHeap.h"

#include <array>
#include <bitset>
#include <future>

#include <fmt/format.h>
#include <glm/vec2.hpp>
#include <wrl/client.h>

enum D3D_FEATURE_LEVEL;
struct ID3D12CommandQueue;
struct ID3D12DescriptorHeap;
struct ID3D12Device;
struct ID3D12Device1;
struct ID3D12Device2;
struct ID3D12Device3;
struct ID3D12Device4;
struct ID3D12Device5;
struct ID3D12Device6;
struct ID3D12DeviceChild;
struct ID3D12Fence;
struct ID3D12PipelineState;
struct ID3D12Resource;
struct IDXGIAdapter4;
struct IDXGIFactory4;
struct IDXGISwapChain3;
struct D3D12_CPU_DESCRIPTOR_HANDLE;

namespace scrap
{
class Window;

namespace d3d12
{
class GraphicsPipelineState;
struct GraphicsPipelineStateParams;

class DeviceContext
{
private:
    static DeviceContext* sInstance;

public:
    static DeviceContext& instance() { return *sInstance; }

    DeviceContext(const Window& window, GpuPreference gpuPreference);
    DeviceContext(const DeviceContext&) = delete;
    DeviceContext(DeviceContext&&) = delete;
    ~DeviceContext();

    DeviceContext& operator=(const DeviceContext&) = delete;
    DeviceContext& operator=(DeviceContext&&) = delete;

    bool initialized() const { return mInitialized; }

    ID3D12Device* getDevice() { return mDevice.Get(); }
    ID3D12Device1* getDevice1() { return mDevice1.Get(); }
    ID3D12Device2* getDevice2() { return mDevice2.Get(); }
    ID3D12Device3* getDevice3() { return mDevice3.Get(); }
    ID3D12Device4* getDevice4() { return mDevice4.Get(); }
    ID3D12Device5* getDevice5() { return mDevice5.Get(); }
    ID3D12Device6* getDevice6() { return mDevice6.Get(); }

    uint32_t getFrameIndex() const { return mFrameIndex; }

    ID3D12Resource* getBackBuffer() { return mRenderTargets[mFrameIndex].Get(); }
    D3D12_CPU_DESCRIPTOR_HANDLE getBackBufferRtv() const;

    GraphicsContext& getGraphicsContext() { return *mGraphicsContext; }
    const GraphicsContext& getGraphicsContext() const { return *mGraphicsContext; }

    CopyContext& getCopyContext() { return *mCopyContext; }
    const CopyContext& getCopyContext() const { return *mCopyContext; }

    d3d12::FixedDescriptorHeap_CBV_SRV_UAV& getCbvSrvUavHeap() { return *mCbvSrvUavHeap; }
    d3d12::FixedDescriptorHeap_RTV& getRtvHeap() { return *mRtvHeap; }
    d3d12::FixedDescriptorHeap_DSV& getDsvHeap() { return *mDsvHeap; }

    glm::i32vec2 frameSize() const { return mFrameBufferSize; }

    [[nodiscard]] bool isRaytracingSupported() const { return mRaytracingSupported; }

    [[nodiscard]] bool isSupportedRenderTargetFormat(DXGI_FORMAT format) const
    {
        return mRenderTargetFormatSupport[format];
    }
    [[nodiscard]] bool isSupportedDepthStencilFormat(DXGI_FORMAT format) const
    {
        return mDepthStencilFormatSupport[format];
    }
    [[nodiscard]] bool isSupportedIndexBufferFormat(DXGI_FORMAT format) const { return mIndexFormatSupport[format]; }

    void beginFrame();
    void endFrame();

    std::shared_ptr<GraphicsPipelineState> createGraphicsPipelineState(GraphicsPipelineStateParams&& params);

private:
    void getHardwareAdapter(GpuPreference gpuPreference, D3D_FEATURE_LEVEL featureLevel, IDXGIFactory4* dxgiFactory);
    HRESULT createDevice(D3D_FEATURE_LEVEL featureLevel);
    void collectFormatSupport();

    // Debug needs to be the first member so it's the last one destroyed. Debug checks to see what
    // d3d12 and dxgi objects are still live on destruction.
    Debug mDebug;

    Microsoft::WRL::ComPtr<IDXGIAdapter4> mAdapter;
    Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
    Microsoft::WRL::ComPtr<ID3D12Device1> mDevice1;
    Microsoft::WRL::ComPtr<ID3D12Device2> mDevice2;
    Microsoft::WRL::ComPtr<ID3D12Device3> mDevice3;
    Microsoft::WRL::ComPtr<ID3D12Device4> mDevice4;
    Microsoft::WRL::ComPtr<ID3D12Device5> mDevice5;
    Microsoft::WRL::ComPtr<ID3D12Device6> mDevice6;
    Microsoft::WRL::ComPtr<IDXGISwapChain3> mSwapChain;
    std::unique_ptr<MonotonicDescriptorHeap_RTV> mSwapChainRtvHeap;
    d3d12::MonotonicDescriptorHeapAllocation mSwapChainRtvs;
    std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, d3d12::kFrameBufferCount> mRenderTargets;

    std::unique_ptr<FixedDescriptorHeap_CBV_SRV_UAV> mCbvSrvUavHeap;
    std::unique_ptr<FixedDescriptorHeap_RTV> mRtvHeap;
    std::unique_ptr<FixedDescriptorHeap_DSV> mDsvHeap;

    // Graphcs and copy context need to be declared after the descriptor heaps so they are destroyed before the
    // descriptor heaps.
    std::unique_ptr<GraphicsContext> mGraphicsContext;
    std::unique_ptr<CopyContext> mCopyContext;

    glm::i32vec2 mFrameBufferSize{0, 0};

    uint32_t mFrameIndex = 0;

    std::vector<std::future<void>> mFutures;

    bool mInitialized = false;

    bool mRaytracingSupported = false;

    static constexpr size_t kDxgiFormatBounds = 191;
    std::bitset<kDxgiFormatBounds> mRenderTargetFormatSupport;
    std::bitset<kDxgiFormatBounds> mDepthStencilFormatSupport;
    std::bitset<kDxgiFormatBounds> mIndexFormatSupport;
};
} // namespace d3d12
} // namespace scrap