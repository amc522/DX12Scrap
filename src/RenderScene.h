// RenderScene is responsible for rendering the main scene and organizing the different rendering passes. RenderScene
// will see a lot of change. In earlier implementations, it will have lots of inline creation of D3D12 objects.
// Eventaully those D3D12 objects will be encapsulated in other classes and RenderScene will begin to look a lot more
// platform agnostic.

#pragma once

#include "d3d12/D3D12Config.h"
#include "d3d12/D3D12FixedDescriptorHeap.h"

#include <array>
#include <cstdint>

#include <wrl/client.h>

struct ID3D12CommandAllocator;
struct ID3D12GraphicsCommandList;
struct ID3D12RootSignature;

namespace scrap
{
namespace d3d12
{
class DeviceContext;
class GraphicsPipelineState;
class Texture;
}

class RenderScene
{
public:
    RenderScene(d3d12::DeviceContext& d3d12Context);
    RenderScene(const RenderScene&) = delete;
    RenderScene(RenderScene&&);
    ~RenderScene();

    RenderScene& operator=(const RenderScene&) = delete;
    RenderScene& operator=(RenderScene&&);

    void render(d3d12::DeviceContext& d3d12Context);

    bool initialized() { return mInitialized; }

private:
    bool loadShaders();
    void createTexture();

    Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature;
    std::shared_ptr<d3d12::GraphicsPipelineState> mPso;

    std::array<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>, d3d12::kFrameBufferCount> mCommandAllocators;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;

    std::unique_ptr<d3d12::Texture> mTexture;
    bool mInitialized = false;
};
} // namespace scrap