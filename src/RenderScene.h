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
#include "d3d12/D3D12ShaderTable.h"

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
class BLAccelerationStructure;
class Buffer;
class DeviceContext;
class GraphicsPipelineState;
class RaytracingPipelineState;
class RaytracingShader;
class Texture;
class TLAccelerationStructure;
} // namespace d3d12

struct Viewport
{
    float left;
    float top;
    float right;
    float bottom;
};

struct RayGenConstantBuffer
{
    Viewport viewport;
    Viewport stencil;
};

struct FrameConstantBuffer
{
    glm::mat4x4 worldToView;
    glm::mat4x4 viewToClip;
    glm::mat4x4 worldToClip;
    float time;
    float frameTimeDelta;
};

enum class Scene
{
    Raster,
    Raytracing
};

class RasterScene
{
public:
    RasterScene();
    RasterScene(const RasterScene&) = delete;
    RasterScene(RasterScene&&);
    ~RasterScene();

    RasterScene& operator=(const RasterScene&) = delete;
    RasterScene& operator=(RasterScene&&);

    void preRender(const FrameInfo& frameInfo);
    void render(const FrameInfo& frameInfo, d3d12::DeviceContext& d3d12Context);
    void endFrame();

    bool isInitialized() { return mInitialized; }

private:
    struct TextureBindingDesc
    {
        size_t nameHash;
        d3d12::Texture* texture;
    };

    bool createRootSignature();
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

class RaytracingScene
{
public:
    RaytracingScene();
    RaytracingScene(const RaytracingScene&) = delete;
    RaytracingScene(RaytracingScene&&);
    ~RaytracingScene();

    RaytracingScene& operator=(const RaytracingScene&) = delete;
    RaytracingScene& operator=(RaytracingScene&&);

    void preRender(const FrameInfo& frameInfo);
    void render(const FrameInfo& frameInfo, d3d12::DeviceContext& d3d12Context);
    void endFrame();

    bool isInitialized() { return mInitialized; }

private:
    bool createRenderTargets();
    bool createRootSignatures();
    bool createPipelineState();
    bool buildGeometry();
    bool buildAccelerationStructures();
    bool buildShaderTables();

    static constexpr std::wstring_view kRaygenShaderName = L"MyRaygenShader";
    static constexpr std::wstring_view kClosestHitShaderName = L"MyClosestHitShader";
    static constexpr std::wstring_view kMissShaderName = L"MyMissShader";
    static constexpr std::wstring_view kHitGroupName = L"MyHitGroup";

    Camera mCamera;

    Microsoft::WRL::ComPtr<ID3D12RootSignature> mGlobalRootSignature;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> mLocalRootSignature;
    d3d12::GraphicsCommandList mCommandList;

    std::unique_ptr<d3d12::Texture> mRenderTarget;
    GpuMesh mTriangleMesh;

    std::unique_ptr<d3d12::TLAccelerationStructure> mTlas;
    std::shared_ptr<d3d12::BLAccelerationStructure> mBlas;

    std::shared_ptr<d3d12::RaytracingShader> mShader;
    std::shared_ptr<d3d12::RaytracingPipelineState> mPipelineState;

    std::shared_ptr<d3d12::ShaderTable> mShaderTable;

    RayGenConstantBuffer mRayGenCpuConstantBuffer;
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
    std::unique_ptr<RasterScene> mRasterScene;
    std::unique_ptr<RaytracingScene> mRaytraceScene;
    Scene mActiveScene = Scene::Raytracing;
};
} // namespace scrap