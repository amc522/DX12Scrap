#pragma once

#include <wrl/client.h>

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

private:
    Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature;
};
} // namespace scrap