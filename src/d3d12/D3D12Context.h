#pragma once

#include <fmt/format.h>
#include <wrl/client.h>

enum D3D_FEATURE_LEVEL;
struct IDXGIAdapter4;

namespace scrap
{
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
    D3D12Context(GpuPreference gpuPreference);
    ~D3D12Context();

    operator bool() const { return mInitialized; }

private:
    void GetHardwareAdapter(GpuPreference gpuPreference, D3D_FEATURE_LEVEL featureLevel);

    Microsoft::WRL::ComPtr<IDXGIAdapter4> mAdapter;
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