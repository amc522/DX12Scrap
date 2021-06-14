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

    if(!loadShaders(d3d12Context.getDevice())) { return; }

    { // Create the command list.
        // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12device-createcommandlist
        HRESULT hr = d3d12Context.getDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                                 d3d12Context.getCommandAllocator(), mPso.Get(),
                                                                 IID_PPV_ARGS(&mCommandList));

        if(FAILED(hr))
        {
            spdlog::critical("Failed to create d3d12 command list");
            return;
        }

        // Command lists are created in the recording state, but there is nothing
        // to record yet. The main loop expects it to be closed, so close it now.
        if(FAILED(mCommandList->Close())) { spdlog::error("Failed to close graphics command list"); }

        spdlog::info("Created d3d12 command list");
    }

    { // Create synchronization objects and wait until assets have been uploaded to the GPU.
        HRESULT hr = d3d12Context.getDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence));
        mFenceValue = 1;

        // Create an event handle to use for frame synchronization.
        // https://docs.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-createeventw
        mFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if(mFenceEvent == nullptr)
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
            spdlog::critical("Failed to create fence event");
        }

        // Wait for the command list to execute; we are reusing the same command
        // list in our main loop but for now, we just want to wait for setup to
        // complete before continuing.
        waitForPreviousFrame(d3d12Context);

        spdlog::info("Created syncronization objects");
    }

    mInitialized = true;
}

RenderScene::~RenderScene()
{
    spdlog::info("Destroying RenderScene");
    if(mFenceEvent != nullptr) { CloseHandle(mFenceEvent); }
}

bool RenderScene::loadShaders(ID3D12Device* device)
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

void RenderScene::render(D3D12Context& d3d12Context)
{
    // Command list allocators can only be reset when the associated
    // command lists have finished execution on the GPU; apps should use
    // fences to determine GPU execution progress.
    if(FAILED(d3d12Context.getCommandAllocator()->Reset()))
    {
        spdlog::error("Failed to reset d3d12 command allocator");
        return;
    }

    // However, when ExecuteCommandList() is called on a particular command
    // list, that command list can then be reset at any time and must be before
    // re-recording.
    if(FAILED(mCommandList->Reset(d3d12Context.getCommandAllocator(), mPso.Get())))
    {
        spdlog::error("Failed to reset d3d12 command list");
        return;
    }

    D3D12_VIEWPORT viewport{};
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = static_cast<float>(d3d12Context.frameSize().x);
    viewport.Height = static_cast<float>(d3d12Context.frameSize().y);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    D3D12_RECT scissorRect{static_cast<LONG>(viewport.TopLeftX), static_cast<LONG>(viewport.TopLeftY),
                           static_cast<LONG>(viewport.Width), static_cast<LONG>(viewport.Height)};

    // Set necessary state.
    mCommandList->SetGraphicsRootSignature(mRootSignature.Get());
    mCommandList->RSSetViewports(1, &viewport);
    mCommandList->RSSetScissorRects(1, &scissorRect);

    // Indicate that the back buffer will be used as a render target.
    D3D12_RESOURCE_BARRIER renderTargetBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
        d3d12Context.getBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

    mCommandList->ResourceBarrier(1, &renderTargetBarrier);

    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = d3d12Context.getBackBufferRtv();
    mCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

    // Record commands.
    const float clearColor[] = {0.0f, 0.2f, 0.4f, 1.0f};
    mCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    mCommandList->DrawInstanced(3, 1, 0, 0);

    // Indicate that the back buffer will now be used to present.
    renderTargetBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
        d3d12Context.getBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

    mCommandList->ResourceBarrier(1, &renderTargetBarrier);

    if(FAILED(mCommandList->Close()))
    {
        spdlog::error("Failed to close d3d12 command list");
        return;
    }

    // Execute the command list.
    std::array<ID3D12CommandList*, 1> commandLists = {mCommandList.Get()};
    d3d12Context.getCommandQueue()->ExecuteCommandLists(static_cast<UINT>(commandLists.size()), commandLists.data());

    // Present the frame.
    d3d12Context.present();

    waitForPreviousFrame(d3d12Context);
}

void RenderScene::shutdown(D3D12Context& d3d12Context)
{
    spdlog::info("Shutting down RenderScene");
    waitForPreviousFrame(d3d12Context);
}

void RenderScene::waitForPreviousFrame(D3D12Context& d3d12Context)
{
    // WAITING FOR THE FRAME TO COMPLETE BEFORE CONTINUING IS NOT BEST PRACTICE.
    // This is code implemented as such for simplicity. The D3D12HelloFrameBuffering
    // sample illustrates how to use fences for efficient resource usage and to
    // maximize GPU utilization.

    // Signal and increment the fence value.
    const UINT64 fence = mFenceValue;
    // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12commandqueue-signal
    if(FAILED(d3d12Context.getCommandQueue()->Signal(mFence.Get(), fence)))
    {
        spdlog::error("Failed to update fence.");
        return;
    }
    mFenceValue++;

    // Wait until the previous frame is finished.
    if(mFence->GetCompletedValue() < fence)
    {
        if(FAILED(mFence->SetEventOnCompletion(fence, mFenceEvent)))
        {
            spdlog::error("Fence SetEventOnCompletion call failed");
            return;
        }

        WaitForSingleObject(mFenceEvent, INFINITE);
    }

    d3d12Context.swap();
}

} // namespace scrap
