#pragma once

#include <cstdint>
#include <wrl/client.h>

struct ID3D12GraphicsCommandList;
struct ID3D12Device;
struct ID3D12Fence;
struct ID3D12PipelineState;
struct ID3D12RootSignature;

namespace scrap
{
class D3D12Context;

class RenderScene
{
public:
    RenderScene(D3D12Context& d3d12Context);
    ~RenderScene();

    void Render(D3D12Context& d3d12Context);

    bool initialized() { return mInitialized; }

private:
    bool LoadShaders(ID3D12Device* device);
    void WaitForPreviousFrame(D3D12Context& d3d12Context);

    Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> mPso;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;
    Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
    uint64_t mFenceValue = 0;
    HANDLE mFenceEvent;

    bool mInitialized = false;
};
} // namespace scrap