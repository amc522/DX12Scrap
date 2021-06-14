#include "RenderScene.h"

#include "d3d12/D3D12Context.h"

#include <d3d12.h>
#include <d3dx12.h>
#include <spdlog/spdlog.h>

using namespace Microsoft::WRL;

namespace scrap
{
RenderScene::RenderScene(D3D12Context& d3d12Context)
{
    // Create an empty root signature.
    {
        D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
        CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init(0,       // number of parameters
                               nullptr, // array of D3D12_ROOT_PARAMETER
                               0,       // number of static samplers,
                               nullptr, // array of D3D12_STATIC_SAMPLER_DESC
                               flags);

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;

        // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-d3d12serializerootsignature
        if(FAILED(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error)))
        {
            if(error != nullptr)
            {
                spdlog::critical(L"Failed to serialize d3d12 root signature. {}",
                                 std::wstring_view((const WCHAR*)error->GetBufferPointer(), error->GetBufferSize()));
            }
            else
            {
                spdlog::critical("Failed to serialize d3d12 root signature");
            }

            return;
        }

        if(FAILED(d3d12Context.getDevice()->CreateRootSignature(
               0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&mRootSignature))))
        {
            spdlog::critical("Failed to create d3d12 root signature");
            return;
        }

        spdlog::info("Created d3d12 root signature");
    }
}

RenderScene::~RenderScene() {}

void RenderScene::Render(D3D12Context& d3d12Context) {}
} // namespace scrap
