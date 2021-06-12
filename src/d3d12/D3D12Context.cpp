#include "D3D12Context.h"

#include "D3D12Strings.h"

#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_4.h>
#include <dxgi1_6.h>
#include <spdlog/spdlog.h>

using namespace Microsoft::WRL;

namespace scrap
{
std::wstring_view GetGpuVendorName(UINT vendorId)
{
    switch(vendorId)
    {
    case 0x1002: return L"AMD";
    case 0x10DE: return L"Nvidia";
    case 0x8086: return L"Intel";
    default: return L"Unknown";
    }
}

void LogAdapterDesc(const DXGI_ADAPTER_DESC1& desc, UINT adapterIndex)
{
    spdlog::info(L"Adapter {}\n\t{}\n\tVendor: {} ({:#x})\n\tDevice Id: {:#x}\n\tDedicated Video Memory: "
                 L"{:L} bytes\n\tDedicated System Memory: {:L} bytes\n\tShared System Memory: {:L} bytes",
                 adapterIndex, desc.Description, GetGpuVendorName(desc.VendorId), desc.VendorId, desc.DeviceId,
                 desc.DedicatedVideoMemory, desc.DedicatedSystemMemory, desc.SharedSystemMemory);
}

D3D12Context::D3D12Context(GpuPreference gpuPreference)
{
    spdlog::info("Initializing D3D12");

#ifdef _DEBUG
    // Always enable the debug layer before doing anything DX12 related
    // so all possible errors generated while creating DX12 objects
    // are caught by the debug layer.
    Microsoft::WRL::ComPtr<ID3D12Debug> debugInterface;

    if(FAILED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface))))
    {
        spdlog::error("Failed to enable the d3d12 debug layer.");
    }

    debugInterface->EnableDebugLayer();
    spdlog::info("Enabled the d3d12 debug layer");
#endif

    // Feature level documentation
    // https://docs.microsoft.com/en-us/windows/win32/direct3d12/hardware-feature-levels
    constexpr D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

    GetHardwareAdapter(gpuPreference, featureLevel);
}

D3D12Context::~D3D12Context() { spdlog::info("Shutting down D3D12"); }

constexpr DXGI_GPU_PREFERENCE GetDxgiGpuPreference(GpuPreference gpuPreference)
{
    switch(gpuPreference)
    {
    case scrap::GpuPreference::HighPerformance: return DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE;
    case scrap::GpuPreference::MinimumPower: return DXGI_GPU_PREFERENCE_MINIMUM_POWER;
    case scrap::GpuPreference::MaximumMemory: [[fallthrough]];
    case scrap::GpuPreference::MinimumMemory: [[fallthrough]];
    case scrap::GpuPreference::None: [[fallthrough]];
    default: return DXGI_GPU_PREFERENCE_UNSPECIFIED;
    }
}

void D3D12Context::GetHardwareAdapter(GpuPreference gpuPreference, D3D_FEATURE_LEVEL featureLevel)
{
    const DXGI_GPU_PREFERENCE dxgiGpuPreference = GetDxgiGpuPreference(gpuPreference);

    ComPtr<IDXGIFactory4> dxgiFactory4;
    UINT createFactoryFlags = 0;
#ifdef _DEBUG
    createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

    if(FAILED(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory4))))
    {
        spdlog::critical("CreateDXGIFactory2 failed.");
        return;
    }

    spdlog::info("Enumerating adapters with preference '{}'", gpuPreference);

    ComPtr<IDXGIAdapter4> dxgiAdapter4;
    UINT selectedAdapterIndex = 0;

    ComPtr<IDXGIFactory6> dxgiFactory6;
    if(dxgiGpuPreference != DXGI_GPU_PREFERENCE_UNSPECIFIED &&
       SUCCEEDED(dxgiFactory4->QueryInterface(IID_PPV_ARGS(&dxgiFactory6))))
    {
        ComPtr<IDXGIAdapter1> dxgiAdapter1;

        for(UINT adapterIndex = 0;
            dxgiFactory6->EnumAdapterByGpuPreference(adapterIndex, dxgiGpuPreference, IID_PPV_ARGS(&dxgiAdapter1)) !=
            DXGI_ERROR_NOT_FOUND;
            ++adapterIndex)
        {
            DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
            dxgiAdapter1->GetDesc1(&dxgiAdapterDesc1);

            if((dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) != 0) { continue; }

            LogAdapterDesc(dxgiAdapterDesc1, adapterIndex);

            if(FAILED(D3D12CreateDevice(dxgiAdapter1.Get(), featureLevel, __uuidof(ID3D12Device), nullptr)))
            {
                spdlog::info("Adapter {} does not support feature level {}", adapterIndex, featureLevel);
                continue;
            }

            if(FAILED(dxgiAdapter1.As(&dxgiAdapter4))) { continue; }

            selectedAdapterIndex = adapterIndex;

            // The adapters are ordered based on dxgiGpuPrefence. The first one should be good.
            break;
        }
    }
    else
    {
        ComPtr<IDXGIAdapter1> dxgiAdapter1;

        SIZE_T selectedDedicatedVideoMemory =
            (gpuPreference == GpuPreference::MaximumMemory) ? 0u : std::numeric_limits<SIZE_T>::max();

        for(UINT adapterIndex = 0; dxgiFactory4->EnumAdapters1(adapterIndex, &dxgiAdapter1) != DXGI_ERROR_NOT_FOUND;
            ++adapterIndex)
        {
            DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
            dxgiAdapter1->GetDesc1(&dxgiAdapterDesc1);

            // Check if this is a software adapter. If it is, just skip it.
            if((dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) != 0) { continue; }

            LogAdapterDesc(dxgiAdapterDesc1, adapterIndex);

            if(FAILED(D3D12CreateDevice(dxgiAdapter1.Get(), featureLevel, __uuidof(ID3D12Device), nullptr)))
            {
                spdlog::info("Adapter {} does not support feature level {}", adapterIndex, featureLevel);
                continue;
            }

            if((gpuPreference == GpuPreference::MaximumMemory &&
                dxgiAdapterDesc1.DedicatedVideoMemory < selectedDedicatedVideoMemory) ||
               (gpuPreference == GpuPreference::MinimumMemory &&
                dxgiAdapterDesc1.DedicatedVideoMemory > selectedDedicatedVideoMemory))
            {
                continue;
            }

            if(FAILED(dxgiAdapter1.As(&dxgiAdapter4))) { continue; }

            selectedDedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
            selectedAdapterIndex = adapterIndex;
        }
    }

    if(dxgiAdapter4 == nullptr)
    {
        spdlog::critical("Failed to find a support adapter.");
        return;
    }

    mAdapter = std::move(dxgiAdapter4);
    spdlog::info("Using adapter {}", selectedAdapterIndex);
}

} // namespace scrap