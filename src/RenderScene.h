// RenderScene is responsible for rendering the main scene and organizing the different rendering passes. RenderScene
// will see a lot of change. In earlier implementations, it will have lots of inline creation of D3D12 objects.
// Eventaully those D3D12 objects will be encapsulated in other classes and RenderScene will begin to look a lot more
// platform agnostic.

#pragma once

#include "Camera.h"
#include "GpuMesh.h"
#include "RenderObject.h"
#include "d3d12/D3D12CommandList.h"
#include "d3d12/D3D12Config.h"
#include "d3d12/D3D12FixedDescriptorHeap.h"
#include "d3d12/D3D12Fwd.h"
#include "d3d12/D3D12ShaderTable.h"
#include "d3d12/D3D12TLAccelerationStructure.h"

#include <array>
#include <cstdint>

#include <glm/vec2.hpp>
#include <wrl/client.h>

namespace scrap
{
struct FrameInfo;

struct FrameConstantBuffer
{
    glm::mat4x4 worldToView;
    glm::mat4x4 viewToWorld;
    glm::mat4x4 viewToClip;
    glm::mat4x4 clipToView;
    glm::mat4x4 worldToClip;
    glm::mat4x4 clipToWorld;
    glm::vec3 cameraWorldPos;
    float time;
    float frameTimeDelta;
    glm::vec3 padding;
};

struct RenderParams
{
    FrameConstantBuffer frameConstants;
    std::span<RenderObject> renderObjects;
};

enum class Scene
{
    Raster,
    Raytracing
};

class RasterRenderer
{
public:
    RasterRenderer();
    RasterRenderer(const RasterRenderer&) = delete;
    RasterRenderer(RasterRenderer&&);
    ~RasterRenderer();

    RasterRenderer& operator=(const RasterRenderer&) = delete;
    RasterRenderer& operator=(RasterRenderer&&);

    void preRender(const FrameInfo& frameInfo, const RenderParams& renderParams);
    void render(const FrameInfo& frameInfo, const RenderParams& renderParams);
    void endFrame();

    bool isInitialized() { return mInitialized; }

    std::shared_ptr<d3d12::GraphicsPipelineState> createPipelineState(d3d12::GraphicsShaderParams&& shaderParams, d3d12::GraphicsPipelineStateParams&& pipelineStateParams);

private:
    struct TextureBindingDesc
    {
        SharedString name;
        d3d12::Texture* texture;
    };

    bool createRootSignature();
    void createRenderTargets();
    void createFrameConstantBuffer();

    void bindTextures(Material& material);

    Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature;

    d3d12::GraphicsCommandList mCommandList;

    std::shared_ptr<d3d12::Buffer> mFrameConstantBuffer;

    std::unique_ptr<d3d12::Texture> mDepthStencilTexture;
    bool mInitialized = false;
};

class RaytracingRenderer
{
public:
    RaytracingRenderer();
    RaytracingRenderer(const RaytracingRenderer&) = delete;
    RaytracingRenderer(RaytracingRenderer&&);
    ~RaytracingRenderer();

    RaytracingRenderer& operator=(const RaytracingRenderer&) = delete;
    RaytracingRenderer& operator=(RaytracingRenderer&&);

    void preRender(const FrameInfo& frameInfo, const RenderParams& renderParams);
    void render(const FrameInfo& frameInfo, const RenderParams& renderParams);
    void endFrame();

    bool isInitialized() { return mInitialized; }

    std::shared_ptr<d3d12::RaytracingPipelineState> createPipelineState(d3d12::RaytracingShaderParams&& shaderParams);

private:
    bool createRenderTargets();
    bool createRootSignatures();
    bool createInternalPipelineStates();
    bool buildAccelerationStructures();
    bool buildShaderTables();

    Microsoft::WRL::ComPtr<ID3D12RootSignature> mGlobalRootSignature;
    std::array<Microsoft::WRL::ComPtr<ID3D12RootSignature>, (size_t)RaytracingShaderStage::Count> mLocalRootSignatures;
    d3d12::GraphicsCommandList mCommandList;

    std::unique_ptr<d3d12::Texture> mRenderTarget;

    std::unique_ptr<d3d12::TLAccelerationStructure> mTlas;

    std::shared_ptr<d3d12::RaytracingDispatchPipelineState> mDispatchPipelineState;
    std::shared_ptr<d3d12::RaytracingPipelineState> mMainPassPipelineState;
    std::shared_ptr<d3d12::ShaderTable> mShaderTable;
    d3d12::ShaderTableAllocation mMainPassShaderTableAllocation;

    std::shared_ptr<d3d12::Buffer> mFrameConstantBuffer;

    bool mInitialized = false;
};

class RenderScene
{
public:
    RenderScene();

    [[nodiscard]] bool isInitialized() const
    {
        return mRasterScene->isInitialized() && (mRaytraceScene == nullptr || mRaytraceScene->isInitialized());
    }

    void preRender(const FrameInfo& frameInfo);
    void render(const FrameInfo& frameInfo, d3d12::DeviceContext& d3d12Context);
    void endFrame();

private:
    GpuMesh createCube();
    std::shared_ptr<d3d12::Texture> createTexture();

    bool createRenderObject();

    Camera mCamera;
    std::unique_ptr<RasterRenderer> mRasterScene;
    std::unique_ptr<RaytracingRenderer> mRaytraceScene;
    Scene mActiveScene = Scene::Raytracing;

    std::vector<RenderObject> mRenderObjects;
    uint32_t mNextRenderObjectId = 0;

    std::shared_ptr<d3d12::Texture> mTexture;
    RenderParams mRenderParams{};
};
} // namespace scrap
