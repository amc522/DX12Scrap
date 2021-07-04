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

struct ID3D12GraphicsCommandList;
struct ID3D12Device;
struct ID3D12Fence;
struct ID3D12PipelineState;
struct ID3D12Resource;
struct ID3D12RootSignature;

namespace scrap
{
namespace d3d12
{
class DeviceContext;
}

class RenderScene
{
public:
    RenderScene(d3d12::DeviceContext& d3d12Context);
    ~RenderScene();

    void render(d3d12::DeviceContext& d3d12Context);
    void shutdown(d3d12::DeviceContext& d3d12Context);

    bool initialized() { return mInitialized; }

private:
    bool loadShaders(ID3D12Device* device);
    void waitOnGpu(d3d12::DeviceContext& d3d12Context);
    void endFrame(d3d12::DeviceContext& d3d12Context);

    Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> mPso;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;
    Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
    std::array<uint64_t, 2> mFenceValues{};
    HANDLE mFenceEvent = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Resource> mTexture;
    d3d12::FixedDescriptorHeapDescriptor mTextureSrv;

    bool mInitialized = false;
};
} // namespace scrap