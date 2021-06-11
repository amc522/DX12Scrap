#include "D3D12Context.h"

#include <Windows.h>
#include <d3d12.h>
#include <spdlog/spdlog.h>
#include <wrl.h>
#include <wrl/client.h>

namespace scrap
{
D3D12Context::D3D12Context()
{
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
}

D3D12Context::~D3D12Context() {}

} // namespace scrap