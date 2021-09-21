// RenderScene is responsible for rendering the main scene and organizing the different rendering passes. RenderScene
// will see a lot of change. In earlier implementations, it will have lots of inline creation of D3D12 objects.
// Eventaully those D3D12 objects will be encapsulated in other classes and RenderScene will begin to look a lot more
// platform agnostic.

#pragma once

#include "Camera.h"
#include "GpuMesh.h"
#include "d3d12/D3D12CommandList.h"
#include "d3d12/D3D12Config.h"
#include "d3d12/D3D12FixedDescriptorHeap.h"

#include <array>
#include <cstdint>

#include <glm/vec2.hpp>
#include <wrl/client.h>

struct ID3D12CommandAllocator;
struct ID3D12GraphicsCommandList;
struct ID3D12RootSignature;

namespace scrap
{
struct FrameInfo;

namespace d3d12
{
class Buffer;
class DeviceContext;
class GraphicsPipelineState;
class Texture;
} // namespace d3d12

struct FrameConstantBuffer
{
    glm::mat4x4 worldToView;
    glm::mat4x4 viewToClip;
    glm::mat4x4 worldToClip;
    float time;
    float frameTimeDelta;
};

class RenderScene
{
public:
    RenderScene(d3d12::DeviceContext& d3d12Context);
    RenderScene(const RenderScene&) = delete;
    RenderScene(RenderScene&&);
    ~RenderScene();

    RenderScene& operator=(const RenderScene&) = delete;
    RenderScene& operator=(RenderScene&&);

    void preRender(const FrameInfo& frameInfo);
    void render(const FrameInfo& frameInfo, d3d12::DeviceContext& d3d12Context);
    void endFrame();

    bool initialized() { return mInitialized; }

private:
    struct TextureBindingDesc
    {
        size_t nameHash;
        d3d12::Texture* texture;
    };

    bool loadShaders();
    void createRenderTargets();
    void createFrameConstantBuffer();
    void createTriangle();
    void createCube();
    void createTexture();

    void drawIndexed(d3d12::GraphicsPipelineState& pso, GpuMesh& mesh, nonstd::span<TextureBindingDesc> textures);

    Camera mCamera;

    Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature;
    std::shared_ptr<d3d12::GraphicsPipelineState> mHelloTrianglePso;
    std::shared_ptr<d3d12::GraphicsPipelineState> mHelloCubePso;

    d3d12::GraphicsCommandList mCommandList;

    std::shared_ptr<d3d12::Buffer> mFrameConstantBuffer;

    GpuMesh mTriangleMesh{PrimitiveTopology::TriangleList};
    GpuMesh mCubeMesh;

    std::unique_ptr<d3d12::Texture> mDepthStencilTexture;
    std::unique_ptr<d3d12::Texture> mTexture;
    bool mInitialized = false;
};
} // namespace scrap