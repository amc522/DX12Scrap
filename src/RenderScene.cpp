#include "RenderScene.h"

#include "d3d12/D3D12Context.h"

#include <d3d12.h>
#include <d3dcompiler.h>
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

    if(!LoadShaders(d3d12Context.getDevice())) { return; }

    mInitialized = true;
}

RenderScene::~RenderScene() {}

bool RenderScene::LoadShaders(ID3D12Device* device)
{
    ComPtr<ID3DBlob> vertexShader;
    ComPtr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
    // Enable better shader debugging with the graphics debugging tools.
    UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    UINT compileFlags = 0;
#endif

    ComPtr<ID3DBlob> error;
    // Shader paths are hardcoded and only work with the directory structure of the project
    constexpr std::wstring_view vertexShaderPath = L"assets\\hello_triangle.hlsl";
    constexpr std::wstring_view pixelShaderPath = L"assets\\hello_triangle.hlsl";

    // https://docs.microsoft.com/en-us/windows/win32/api/d3dcompiler/nf-d3dcompiler-d3dcompilefromfile
    HRESULT hr = D3DCompileFromFile(vertexShaderPath.data(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0,
                                    &vertexShader, &error);
    if(FAILED(hr))
    {
        if(error != nullptr)
        {
            spdlog::critical("Failed to compile vertex shader.\n{}",
                             std::string_view((const char*)error->GetBufferPointer(), error->GetBufferSize()));
        }
        else
        {
            spdlog::critical(L"Failed to compile vertex shader {}", vertexShaderPath);
        }

        return false;
    }

    spdlog::info(L"Compiled vertex shader {}", vertexShaderPath);

    hr = D3DCompileFromFile(pixelShaderPath.data(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader,
                            &error);

    if(FAILED(hr))
    {
        if(error != nullptr)
        {
            spdlog::critical("Failed to compile pixel shader.\n{}",
                             std::string_view((const char*)error->GetBufferPointer(), error->GetBufferSize()));
        }
        else
        {
            spdlog::critical(L"Failed to compile pixel shader {}", pixelShaderPath);
        }

        return false;
    }

    spdlog::info(L"Compiled pixel shader {}", pixelShaderPath);

    // Describe and create the graphics pipeline state object (PSO).
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.pRootSignature = mRootSignature.Get();
    psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
    psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState.DepthEnable = FALSE;
    psoDesc.DepthStencilState.StencilEnable = FALSE;
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.SampleDesc.Count = 1;

    // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12device-creategraphicspipelinestate
    if(FAILED(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPso))))
    {
        spdlog::critical("Failed to create d3d12 pipeline state object");
        return false;
    }

    return true;
}

void RenderScene::Render(D3D12Context& d3d12Context) {}

} // namespace scrap
