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

D3D12Context::D3D12Context()
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

    { // find the adapater (gpu) to use with the most dedicated video memory
        ComPtr<IDXGIFactory4> dxgiFactory;
        UINT createFactoryFlags = 0;
#ifdef _DEBUG
        createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

        if(FAILED(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory))))
        {
            spdlog::critical("CreateDXGIFactory2 failed.");
            return;
        }

        spdlog::info("Enumerating adapters");

        ComPtr<IDXGIAdapter1> dxgiAdapter1;
        ComPtr<IDXGIAdapter4> dxgiAdapter4;

        SIZE_T maxDedicatedVideoMemory = 0;
        UINT selectedAdapterIndex = 0;

        for(UINT i = 0; dxgiFactory->EnumAdapters1(i, &dxgiAdapter1) != DXGI_ERROR_NOT_FOUND; ++i)
        {
            DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
            dxgiAdapter1->GetDesc1(&dxgiAdapterDesc1);

            // Check if this is a software adapter. If it is, just skip it.
            if((dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) != 0) { continue; }

            spdlog::info(L"Adapter {}\n\t{}\n\tVendor: {} ({:#x})\n\tDevice Id: {:#x}\n\tDedicated Video Memory: "
                         L"{:L} bytes\n\tDedicated System Memory: {:L} bytes\n\tShared System Memory: {:L} bytes",
                         i, dxgiAdapterDesc1.Description, GetGpuVendorName(dxgiAdapterDesc1.VendorId),
                         dxgiAdapterDesc1.VendorId, dxgiAdapterDesc1.DeviceId, dxgiAdapterDesc1.DedicatedVideoMemory,
                         dxgiAdapterDesc1.DedicatedSystemMemory, dxgiAdapterDesc1.SharedSystemMemory);

            // Feature level documentation
            // https://docs.microsoft.com/en-us/windows/win32/direct3d12/hardware-feature-levels
            constexpr D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

            if(FAILED(D3D12CreateDevice(dxgiAdapter1.Get(), featureLevel, __uuidof(ID3D12Device), nullptr)))
            {
                spdlog::info("Adapter {} does not support feature level {}", i, featureLevel);
                continue;
            }

            if(dxgiAdapterDesc1.DedicatedVideoMemory < maxDedicatedVideoMemory) { continue; }

            if(FAILED(dxgiAdapter1.As(&dxgiAdapter4))) { continue; }

            maxDedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
            selectedAdapterIndex = i;
        }

        if(dxgiAdapter4 == nullptr)
        {
            spdlog::critical("Failed to find a support adapter.");
            return;
        }

        mAdapter = std::move(dxgiAdapter4);
        spdlog::info("Using adapter {}", selectedAdapterIndex);
    }
}

D3D12Context::~D3D12Context() { spdlog::info("Shutting down D3D12"); }

} // namespace scrap