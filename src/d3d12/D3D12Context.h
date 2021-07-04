// D3D12Context contains all the shared objects for the D3D12 pipeline. This includes (but is not limited to) the
// hardware adapter, device, and swap chains. Specific D3D12 rendering related functions should NOT be implemented here.
// D3D12Context is not responsible for loading and managing shaders, pipeline states, or resources.

#pragma once

#include "d3d12/D3D12Config.h"
#include "d3d12/D3D12FixedDescriptorHeap.h"
#include "d3d12/D3D12PermanentDescriptorHeap.h"

#include <array>

#include <fmt/format.h>
#include <glm/vec2.hpp>
#include <wrl/client.h>

enum D3D_FEATURE_LEVEL;
struct ID3D12CommandAllocator;
struct ID3D12CommandQueue;
struct ID3D12DescriptorHeap;
struct ID3D12Device;
struct ID3D12Device1;
struct ID3D12Device2;
struct ID3D12Device3;
struct ID3D12Device4;
struct ID3D12Device5;
struct ID3D12Device6;
struct ID3D12Resource;
struct IDXGIAdapter4;
struct IDXGIFactory4;
struct IDXGISwapChain3;
struct D3D12_CPU_DESCRIPTOR_HANDLE;

namespace scrap
{
class Window;

enum class GpuPreference
{
    None,
    HighPerformance,
    MinimumPower,
    MaximumMemory,
    MinimumMemory
};

constexpr std::string_view ToString(GpuPreference gpuPreference)
{
    switch(gpuPreference)
    {
    case scrap::GpuPreference::None: return "None";
    case scrap::GpuPreference::HighPerformance: return "HighPerformance";
    case scrap::GpuPreference::MinimumPower: return "MinimumPower";
    case scrap::GpuPreference::MaximumMemory: return "MaximumMemory";
    case scrap::GpuPreference::MinimumMemory: return "MinimumMemory";
    default: return "Unknown GpuPreference";
    }
}

class D3D12Context
{
public:
    D3D12Context(const Window& window, GpuPreference gpuPreference);
    ~D3D12Context();

    bool initialized() const { return mInitialized; }

    ID3D12Device* getDevice() { return mDevice.Get(); }
    ID3D12Device1* getDevice1() { return mDevice1.Get(); }
    ID3D12Device2* getDevice2() { return mDevice2.Get(); }
    ID3D12Device3* getDevice3() { return mDevice3.Get(); }
    ID3D12Device4* getDevice4() { return mDevice4.Get(); }
    ID3D12Device5* getDevice5() { return mDevice5.Get(); }
    ID3D12Device6* getDevice6() { return mDevice6.Get(); }

    uint32_t getFrameIndex() const { return mFrameIndex; }
    ID3D12CommandAllocator* getCommandAllocator() { return mCommandAllocators[mFrameIndex].Get(); }
    ID3D12CommandQueue* getCommandQueue() { return mCommandQueue.Get(); }

    ID3D12Resource* getBackBuffer() { return mRenderTargets[mFrameIndex].Get(); }
    D3D12_CPU_DESCRIPTOR_HANDLE getBackBufferRtv() const;

    d3d12::FixedDescriptorHeap_CBV_SRV_UAV& getCbvSrvUavHeap() { return *mCbvSrvUavHeap; }

    glm::i32vec2 frameSize() const { return mFrameSize; }

    void beginFrame();

    void present();

    void swap();

private:
    void getHardwareAdapter(GpuPreference gpuPreference, D3D_FEATURE_LEVEL featureLevel, IDXGIFactory4* dxgiFactory);
    HRESULT createDevice(D3D_FEATURE_LEVEL featureLevel);

    Microsoft::WRL::ComPtr<IDXGIAdapter4> mAdapter;
    Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
    Microsoft::WRL::ComPtr<ID3D12Device1> mDevice1;
    Microsoft::WRL::ComPtr<ID3D12Device2> mDevice2;
    Microsoft::WRL::ComPtr<ID3D12Device3> mDevice3;
    Microsoft::WRL::ComPtr<ID3D12Device4> mDevice4;
    Microsoft::WRL::ComPtr<ID3D12Device5> mDevice5;
    Microsoft::WRL::ComPtr<ID3D12Device6> mDevice6;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
    std::array<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>, d3d12::kFrameBufferCount> mCommandAllocators;
    Microsoft::WRL::ComPtr<IDXGISwapChain3> mSwapChain;
    std::unique_ptr<d3d12::PermanentDescriptorHeap_RTV> mRtvHeap;
    d3d12::PermanentDescriptorHeapAllocation mSwapChainRtvs;
    std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, d3d12::kFrameBufferCount> mRenderTargets;

    std::unique_ptr<d3d12::FixedDescriptorHeap_CBV_SRV_UAV> mCbvSrvUavHeap;

    glm::i32vec2 mFrameSize{0, 0};
    uint32_t mFrameIndex = 0;

    bool mInitialized = false;
};

} // namespace scrap

template<>
struct fmt::formatter<scrap::GpuPreference> : public fmt::formatter<std::string_view>
{
    template<typename FormatContext>
    auto format(scrap::GpuPreference gpuPreference, FormatContext& ctx)
    {
        return format_to(ctx.out(), scrap::ToString(gpuPreference));
    }
};