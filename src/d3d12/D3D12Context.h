#pragma once

#include <array>
#include <fmt/format.h>
#include <wrl/client.h>

enum D3D_FEATURE_LEVEL;
struct ID3D12CommandQueue;
struct ID3D12DescriptorHeap;
struct ID3D12Device;
struct ID3D12Resource;
struct IDXGIAdapter4;
struct IDXGIFactory4;
struct IDXGISwapChain3;

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

private:
    void GetHardwareAdapter(GpuPreference gpuPreference, D3D_FEATURE_LEVEL featureLevel, IDXGIFactory4* dxgiFactory);

    static constexpr size_t sFrameCount = 2;

    Microsoft::WRL::ComPtr<IDXGIAdapter4> mAdapter;
    Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
    Microsoft::WRL::ComPtr<IDXGISwapChain3> mSwapChain;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;
    std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, sFrameCount> mRenderTargets;
    uint32_t mRtvDescriptorSize = 0;

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