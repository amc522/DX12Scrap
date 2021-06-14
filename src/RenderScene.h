#pragma once

#include <wrl/client.h>

struct ID3D12Device;
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

    Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> mPso;
    bool mInitialized = false;
};
} // namespace scrap