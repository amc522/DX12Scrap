#include "RenderScene.h"

#include "CpuMesh.h"
#include "FrameInfo.h"
#include "PrimitiveMesh.h"
#include "Window.h"
#include "d3d12/D3D12Buffer.h"
#include "d3d12/D3D12Context.h"
#include "d3d12/D3D12Debug.h"
#include "d3d12/D3D12GraphicsPipelineState.h"
#include "d3d12/D3D12GraphicsShader.h"
#include "d3d12/D3D12Strings.h"
#include "d3d12/D3D12Texture.h"
#include "d3d12/D3D12Translations.h"

#include <cputex/utility.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <d3dx12.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <spdlog/spdlog.h>

using namespace Microsoft::WRL;

namespace scrap
{
tl::expected<ComPtr<ID3D12RootSignature>, HRESULT>
SerializeAndCreateRootSignature(const D3D12_VERSIONED_ROOT_SIGNATURE_DESC& desc)
{
    d3d12::DeviceContext& deviceContext = d3d12::DeviceContext::instance();

    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;
    HRESULT hr =
        D3DX12SerializeVersionedRootSignature(&desc, deviceContext.getRootSignatureVersion(), &signature, &error);
    if(FAILED(hr))
    {
        if(error != nullptr)
        {
            spdlog::critical("Failed to serialize d3d12 root signature. {}",
                             std::string_view((const CHAR*)error->GetBufferPointer(), error->GetBufferSize()));
        }
        else
        {
            spdlog::critical("Failed to serialize d3d12 root signature");
        }

        return tl::make_unexpected(hr);
    }

    ComPtr<ID3D12RootSignature> rootSignature;
    hr = deviceContext.getDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(),
                                                        IID_PPV_ARGS(&rootSignature));

    if(FAILED(hr))
    {
        spdlog::critical("Failed to create d3d12 root signature");
        return tl::make_unexpected(hr);
    }

    return rootSignature;
}

//=====================================
// RasterScene
//=====================================
RasterScene::RasterScene(): mCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT, "RasterScene Command List")
{
    mCommandList.beginRecording();

    createRenderTargets();
    createRootSignature();
    createFrameConstantBuffer();
    createTriangle();
    createCube();
    createTexture();

    d3d12::DeviceContext& deviceContext = d3d12::DeviceContext::instance();
    deviceContext.getCopyContext().execute();

    mCommandList.execute(deviceContext.getGraphicsContext().getCommandQueue());

    if(!loadShaders()) { return; }

    mCamera.setPosition(glm::vec3(0.0f, 0.0f, -5.0f));
    mInitialized = true;
}

RasterScene::RasterScene(RasterScene&&) = default;

RasterScene::~RasterScene() = default;

bool RasterScene::createRootSignature()
{
    d3d12::DeviceContext& deviceContext = d3d12::DeviceContext::instance();

    // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_root_parameter1
    // The root parameter describes a parameter or argument being passed into the shader as described or declared by
    // the root signautre. Here we are saying we have a constant buffer with 3 constants.
    std::array<D3D12_ROOT_PARAMETER1, d3d12::RasterRootParamSlot::Count> rootParameters = {};

    D3D12_ROOT_PARAMETER1& frameConstantBuffer = rootParameters[d3d12::RasterRootParamSlot::FrameCB];
    frameConstantBuffer.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    frameConstantBuffer.Descriptor.ShaderRegister = d3d12::reservedShaderRegister::kFrameCB.shaderRegister;
    frameConstantBuffer.Descriptor.RegisterSpace = d3d12::reservedShaderRegister::kFrameCB.registerSpace;
    frameConstantBuffer.Descriptor.Flags = D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE;
    frameConstantBuffer.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    D3D12_ROOT_PARAMETER1& resourceRootConstants = rootParameters[d3d12::RasterRootParamSlot::ResourceIndices];
    resourceRootConstants.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
    resourceRootConstants.Constants.Num32BitValues = d3d12::kMaxBindlessResources;
    resourceRootConstants.Constants.ShaderRegister = d3d12::reservedShaderRegister::kResourceCB.shaderRegister;
    resourceRootConstants.Constants.RegisterSpace = d3d12::reservedShaderRegister::kResourceCB.registerSpace;
    resourceRootConstants.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    D3D12_ROOT_PARAMETER1& vertexRootConstants = rootParameters[d3d12::RasterRootParamSlot::VertexIndices];
    vertexRootConstants.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
    vertexRootConstants.Constants.Num32BitValues = d3d12::kMaxBindlessVertexBuffers;
    vertexRootConstants.Constants.ShaderRegister = d3d12::reservedShaderRegister::kVertexCB.shaderRegister;
    vertexRootConstants.Constants.RegisterSpace = d3d12::reservedShaderRegister::kVertexCB.registerSpace;
    vertexRootConstants.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

    // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_static_sampler_desc
    // Static samplers are the same as normal samplers, except that they are not going to change after creating the
    // root signature. Static samplers can be used to describe all the basic samplers that are used by most shaders
    // in an engine; point, bilinear, trilinear, anisotropic 1-16, etc. Probably only some samplers used for shadows
    // will need to be dynammic. There can be an unlimited number of static samplers in the root signature at no
    // cost.
    std::array<D3D12_STATIC_SAMPLER_DESC, 1> staticSamplers = {};
    D3D12_STATIC_SAMPLER_DESC& sampler = staticSamplers[0];
    sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
    sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.MipLODBias = 0;
    sampler.MaxAnisotropy = 0;
    sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
    sampler.MinLOD = 0.0f;
    sampler.MaxLOD = D3D12_FLOAT32_MAX;
    sampler.ShaderRegister = 0;
    sampler.RegisterSpace = 0;
    sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_versioned_root_signature_desc
    D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
    rootSignatureDesc.Desc_1_1.NumParameters = (UINT)rootParameters.size();
    rootSignatureDesc.Desc_1_1.pParameters = rootParameters.data();
    rootSignatureDesc.Desc_1_1.NumStaticSamplers = (UINT)staticSamplers.size();
    rootSignatureDesc.Desc_1_1.pStaticSamplers = staticSamplers.data();
    rootSignatureDesc.Desc_1_1.Flags = D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED;

    auto rootSigResult = SerializeAndCreateRootSignature(rootSignatureDesc);
    if(!rootSigResult)
    {
        spdlog::critical("Failed to create raster root signature. {}", HRESULT_t(rootSigResult.error()));
        return false;
    }

    mRootSignature = std::move(rootSigResult.value());
    spdlog::info("Created raster root signature");

    return true;
}

bool RasterScene::loadShaders()
{
    { // hello triangle
        d3d12::GraphicsShaderParams shaderParams;
        shaderParams.filepaths[(size_t)GraphicsShaderStage::Vertex] = L"assets\\hello_triangle.hlsl";
        shaderParams.filepaths[(size_t)GraphicsShaderStage::Pixel] = L"assets\\hello_triangle.hlsl";
        shaderParams.entryPoints[(size_t)GraphicsShaderStage::Vertex] = "VSMain";
        shaderParams.entryPoints[(size_t)GraphicsShaderStage::Pixel] = "PSMain";
        shaderParams.stages = GraphicsShaderStageMask::VsPs;
#ifdef _DEBUG
        shaderParams.debug = true;
#endif

        // Describe and create the graphics pipeline state object (PSO).
        {
            d3d12::GraphicsPipelineStateParams psoParams = {};
            psoParams.rootSignature = mRootSignature.Get();
            psoParams.shader = std::make_shared<d3d12::GraphicsShader>(std::move(shaderParams));
            psoParams.rasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
            psoParams.rasterizerState.FrontCounterClockwise = true;
            psoParams.blendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
            psoParams.depthStencilState.DepthEnable = FALSE;
            psoParams.depthStencilState.StencilEnable = FALSE;
            psoParams.primitiveTopologyType =
                d3d12::TranslatePrimitiveTopologyType(mTriangleMesh.getPrimitiveTopology());
            psoParams.renderTargetFormats = {DXGI_FORMAT_R8G8B8A8_UNORM};

            mHelloTrianglePso = d3d12::DeviceContext::instance().createGraphicsPipelineState(std::move(psoParams));
        }
    }

    { // hello cube
        d3d12::GraphicsShaderParams shaderParams;
        shaderParams.filepaths[(size_t)GraphicsShaderStage::Vertex] = L"assets\\hello_cube.hlsl";
        shaderParams.filepaths[(size_t)GraphicsShaderStage::Pixel] = L"assets\\hello_cube.hlsl";
        shaderParams.entryPoints[(size_t)GraphicsShaderStage::Vertex] = "VSMain";
        shaderParams.entryPoints[(size_t)GraphicsShaderStage::Pixel] = "PSMain";
        shaderParams.stages = GraphicsShaderStageMask::VsPs;
#ifdef _DEBUG
        shaderParams.debug = true;
#endif

        // Describe and create the graphics pipeline state object (PSO).
        {
            d3d12::GraphicsPipelineStateParams psoParams = {};
            psoParams.rootSignature = mRootSignature;
            psoParams.shader = std::make_shared<d3d12::GraphicsShader>(std::move(shaderParams));
            psoParams.rasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
            psoParams.rasterizerState.FrontCounterClockwise = true;
            psoParams.blendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
            psoParams.depthStencilState.DepthEnable = TRUE;
            psoParams.depthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
            psoParams.depthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
            psoParams.depthStencilState.StencilEnable = FALSE;
            psoParams.primitiveTopologyType =
                d3d12::TranslatePrimitiveTopologyType(mTriangleMesh.getPrimitiveTopology());
            psoParams.renderTargetFormats = {DXGI_FORMAT_R8G8B8A8_UNORM};
            psoParams.depthStencilFormat = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

            mHelloCubePso = d3d12::DeviceContext::instance().createGraphicsPipelineState(std::move(psoParams));
        }
    }

    return true;
}

void RasterScene::createRenderTargets()
{
    // Currently using the swap chain as our render targets. Just need the depth/stencil buffer
    d3d12::Texture::Params params = {};
    params.format = gpufmt::Format::D32_SFLOAT_S8_UINT;
    params.dimension = cputex::TextureDimension::Texture2D;
    params.arraySize = 1;
    params.extents = cputex::Extent(d3d12::DeviceContext::instance().getFrameSize(), 1);
    params.mipCount = 1;
    params.accessFlags = ResourceAccessFlags::None;
    params.isRenderTarget = false;
    params.name = "DepthStencilTarget";
    params.depthClearValue = 1.0f;
    params.initialResourceState = D3D12_RESOURCE_STATE_DEPTH_WRITE;

    auto depthStencilTexture = std::make_unique<d3d12::Texture>();
    auto error = depthStencilTexture->initUninitialized(params);

    if(error.has_value())
    {
        spdlog::critical("Failed to create depth stencil target for scene. {}", error.value());
        return;
    }

    mDepthStencilTexture = std::move(depthStencilTexture);
}

void RasterScene::createFrameConstantBuffer()
{
    d3d12::Buffer::SimpleParams params;
    params.accessFlags = ResourceAccessFlags::CpuWrite | ResourceAccessFlags::GpuRead;
    params.byteSize = sizeof(FrameConstantBuffer);
    params.flags = d3d12::BufferFlags::ConstantBuffer;
    params.name = "Frame Constant Buffer";

    auto constantBuffer = std::make_unique<d3d12::Buffer>();
    constantBuffer->init(params);

    mFrameConstantBuffer = std::move(constantBuffer);
}

void RasterScene::createTriangle()
{
    { // Positions
        d3d12::Buffer::FormattedParams params;
        params.format = gpufmt::Format::R32G32_SFLOAT;
        params.numElements = 3;
        params.accessFlags = ResourceAccessFlags::GpuRead;
        params.name = "Triangle Positions Buffer";

        std::array<glm::vec2, 3> vertices{{{-0.5f, -0.5f}, {0.5f, -0.5f}, {0.0f, 0.5f}}};
        mTriangleMesh.createVertexElement(ShaderVertexSemantic::Position, 0, params,
                                          nonstd::as_bytes(nonstd::span(vertices)));
    }

    { // UVs
        d3d12::Buffer::FormattedParams params;
        params.format = gpufmt::Format::R32G32_SFLOAT;
        params.numElements = 3;
        params.accessFlags = ResourceAccessFlags::GpuRead;
        params.name = "Triangle TexCoords Buffer";

        std::array<glm::vec2, 3> vertices{{{0.0f, 1.0f}, {1.0f, 1.0f}, {0.5f, 0.0f}}};
        mTriangleMesh.createVertexElement(ShaderVertexSemantic::TexCoord, 0, params,
                                          nonstd::as_bytes(nonstd::span(vertices)));
    }

    { // Colors
        d3d12::Buffer::FormattedParams params;
        params.format = gpufmt::Format::R32G32B32A32_SFLOAT;
        params.numElements = 3;
        params.accessFlags = ResourceAccessFlags::GpuRead;
        params.name = "Triangle TexCoords Buffer";

        std::array<glm::vec4, 3> vertices{{{1.0f, 0.0, 0.0, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}}};
        mTriangleMesh.createVertexElement(ShaderVertexSemantic::Color, 0, params,
                                          nonstd::as_bytes(nonstd::span(vertices)));
    }

    { // Indices
        GpuMesh::IndexBufferParams params;
        params.format = IndexBufferFormat::UInt16;
        params.numIndices = 3;
        params.accessFlags = ResourceAccessFlags::GpuRead;
        params.initialResourceState = D3D12_RESOURCE_STATE_COMMON;
        params.name = "Triangle Index Buffer";

        std::array<uint16_t, 3> indices{0, 1, 2};
        mTriangleMesh.initIndices(params, nonstd::as_bytes(nonstd::span(indices)));
    }
}

void RasterScene::createTexture()
{
    cputex::TextureParams textureParams;
    textureParams.dimension = cputex::TextureDimension::Texture2D;
    textureParams.extent = {1024, 1024, 1};
    textureParams.faces = 1;
    textureParams.mips = cputex::maxMips(textureParams.extent);
    textureParams.arraySize = 1;
    textureParams.format = gpufmt::Format::R8G8B8A8_UNORM;
    cputex::UniqueTexture cpuTexture{textureParams};

    //-------------------------------------
    // Generate a checkboard texture
    //-------------------------------------
    for(uint32_t mip = 0; mip < textureParams.mips; ++mip)
    {
        cputex::SurfaceSpan surface = cpuTexture.accessMipSurface(0, 0, mip);
        const UINT bytesPerPixel = gpufmt::formatInfo(textureParams.format).blockByteSize;
        const UINT rowPitch = surface.extent().x * bytesPerPixel;
        const UINT cellPitch = std::max(rowPitch >> 3, 1u); // The width of a cell in the checkboard texture.
        const UINT cellHeight =
            std::max(surface.extent().x >> 3,
                     std::max(cellPitch / bytesPerPixel, 1u)); // The height of a cell in the checkerboard texture.
        const UINT textureSize = static_cast<UINT>(surface.sizeInBytes());

        UINT8* pData = surface.accessDataAs<UINT8>().data();

        for(UINT n = 0; n < textureSize; n += bytesPerPixel)
        {
            UINT x = n % rowPitch;
            UINT y = n / rowPitch;
            UINT i = x / cellPitch;
            UINT j = y / cellHeight;

            if(i % 2 == j % 2)
            {
                pData[n] = 0x00;     // R
                pData[n + 1] = 0x00; // G
                pData[n + 2] = 0x00; // B
                pData[n + 3] = 0xff; // A
            }
            else
            {
                pData[n] = 0xff;     // R
                pData[n + 1] = 0xff; // G
                pData[n + 2] = 0xff; // B
                pData[n + 3] = 0xff; // A
            }
        }
    }

    auto texture = std::make_unique<d3d12::Texture>();
    texture->initFromMemory(cpuTexture, ResourceAccessFlags::GpuRead, "Firsrt Texture");
    mTexture = std::move(texture);
}

void RasterScene::createCube()
{
    constexpr uint32_t subdivisions = 1;
    MeshSizes meshSizes = CalculateCubeMeshSizes(CubeMeshTopologyType::Triangle, subdivisions);

    CpuMesh cubeMesh(GetCubeMeshPrimitiveTopology(CubeMeshTopologyType::Triangle));
    cubeMesh.initIndices(IndexBufferFormat::UInt16, meshSizes.indexCount);
    cubeMesh.createVertexElement(ShaderVertexSemantic::Position, 0, gpufmt::Format::R32G32B32_SFLOAT,
                                 meshSizes.vertexCount);
    cubeMesh.createVertexElement(ShaderVertexSemantic::Normal, 0, gpufmt::Format::R32G32B32_SFLOAT,
                                 meshSizes.vertexCount);
    cubeMesh.createVertexElement(ShaderVertexSemantic::Tangent, 0, gpufmt::Format::R32G32B32_SFLOAT,
                                 meshSizes.vertexCount);
    cubeMesh.createVertexElement(ShaderVertexSemantic::Binormal, 0, gpufmt::Format::R32G32B32_SFLOAT,
                                 meshSizes.vertexCount);
    cubeMesh.createVertexElement(ShaderVertexSemantic::TexCoord, 0, gpufmt::Format::R32G32_SFLOAT,
                                 meshSizes.vertexCount);

    PrimitiveMesh3dParams primitiveParams(cubeMesh);
    GenerateCubeMeshTris(primitiveParams, subdivisions);

    mCubeMesh = GpuMesh(cubeMesh, ResourceAccessFlags::GpuRead, "Cube");
}

RasterScene& RasterScene::operator=(RasterScene&&) = default;

void RasterScene::preRender(const FrameInfo& frameInfo)
{
    mCamera.update(frameInfo);

    const glm::ivec2 windowSize = frameInfo.mainWindow->getDrawableSize();

    const glm::mat4x4 worldToViewMat = mCamera.worldToViewMatrix();
    const glm::mat4x4 viewToClipMat =
        glm::perspectiveFovLH_ZO(1.04719f, (float)windowSize.x, (float)windowSize.y, 0.1f, 100.0f);
    const glm::mat4x4 worldToClipMat = viewToClipMat * worldToViewMat;

    // This function is called before the copy context command list is executed for the frame.

    // Update relevant constant buffers on the copy context command list so they are ready before the frame is rendered.
    {
        d3d12::GpuBufferWriteGuard writeGuard(*mFrameConstantBuffer.get(),
                                              d3d12::DeviceContext::instance().getCopyContext().getCommandList());

        nonstd::span<FrameConstantBuffer> buffer = writeGuard.getWriteBufferAs<FrameConstantBuffer>();
        buffer.front().time = std::chrono::duration_cast<std::chrono::duration<float>>(
                                  std::chrono::steady_clock::now().time_since_epoch())
                                  .count();
        buffer.front().frameTimeDelta = frameInfo.frameDeltaSec.count();
        buffer.front().worldToView = glm::transpose(worldToViewMat);
        buffer.front().worldToClip = glm::transpose(worldToClipMat);
        buffer.front().viewToClip = glm::transpose(viewToClipMat);
    }
}

void RasterScene::drawIndexed(d3d12::GraphicsPipelineState& pso,
                              GpuMesh& mesh,
                              nonstd::span<TextureBindingDesc> textures)
{
    if(!pso.isReady() || !mesh.isReady()) { return; }

    if(textures.size() > d3d12::kMaxBindlessResources)
    {
        spdlog::warn("Binding more textures than bindless slots available.");
    }

    const d3d12::GraphicsShader& shader = *pso.getShader();

    { // bind vertex buffers
        // creating an array with one more than necessary to give a slot to assign buffers not used by the
        // shader.
        std::array<uint32_t, d3d12::kMaxBindlessVertexBuffers + 1> vertexBufferDescriptorIndices;

        for(const GpuMesh::VertexElement& vertexElement : mesh.getVertexElements())
        {
            const uint32_t constantBufferIndex =
                shader.getVertexElementIndex(vertexElement.semantic, vertexElement.semanticIndex)
                    .value_or(d3d12::kMaxBindlessVertexBuffers);

            vertexBufferDescriptorIndices[constantBufferIndex] = vertexElement.buffer->getSrvDescriptorHeapIndex();
        }

        mCommandList.get()->SetGraphicsRoot32BitConstants(
            d3d12::RasterRootParamSlot::VertexIndices,
            std::min(d3d12::kMaxBindlessVertexBuffers, (uint32_t)shader.getShaderInputs().vertexElements.size()),
            vertexBufferDescriptorIndices.data(), 0);
    }

    { // bind resources
        std::array<uint32_t, d3d12::kMaxBindlessResources + 1> resourceIndices;

        for(const TextureBindingDesc& textureBindingDesc : textures)
        {
            const uint32_t textureIndex =
                shader
                    .getResourceIndex(textureBindingDesc.nameHash, ShaderResourceType::Texture,
                                      textureBindingDesc.texture->getShaderResourceDimension())
                    .value_or(d3d12::kMaxBindlessResources);
            resourceIndices[textureIndex] = mTexture->getSrvDescriptorHeapIndex();
        }

        mCommandList.get()->SetGraphicsRoot32BitConstants(
            d3d12::RasterRootParamSlot::ResourceIndices,
            std::min(d3d12::kMaxBindlessResources, (uint32_t)shader.getShaderInputs().resources.size()),
            resourceIndices.data(), 0);
    }

    pso.markAsUsed(mCommandList.get());
    mesh.markAsUsed(mCommandList.get());
    mTexture->markAsUsed(mCommandList.get());

    const D3D_PRIMITIVE_TOPOLOGY primitiveTopology = d3d12::TranslatePrimitiveTopology(mesh.getPrimitiveTopology());
    const D3D12_INDEX_BUFFER_VIEW ibv = mesh.getIndexBuffer()->getIndexView();

    mCommandList.get()->SetPipelineState(pso.getPipelineState());
    mCommandList.get()->IASetPrimitiveTopology(primitiveTopology);
    mCommandList.get()->IASetIndexBuffer(&ibv);
    mCommandList.get()->DrawIndexedInstanced(mesh.getIndexCount(), 1, 0, 0, 0);
}

void RasterScene::render(const FrameInfo& frameInfo, d3d12::DeviceContext& d3d12Context)
{
    {
        d3d12::ScopedGpuEvent gpuEvent(mCommandList.get(), "RasterScene");

        D3D12_VIEWPORT viewport{};
        viewport.TopLeftX = 0.0f;
        viewport.TopLeftY = 0.0f;
        viewport.Width = static_cast<float>(d3d12Context.getFrameSize().x);
        viewport.Height = static_cast<float>(d3d12Context.getFrameSize().y);
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;

        D3D12_RECT scissorRect{static_cast<LONG>(viewport.TopLeftX), static_cast<LONG>(viewport.TopLeftY),
                               static_cast<LONG>(viewport.Width), static_cast<LONG>(viewport.Height)};

        // Set necessary state.

        // Descriptor heaps need to be set before setting the root signature when using dynamic resources.
        // https://microsoft.github.io/DirectX-Specs/d3d/HLSL_SM_6_6_DynamicResources.html#setdescriptorheaps-and-setrootsignature
        std::array<ID3D12DescriptorHeap*, 1> descriptorHeaps = {d3d12Context.getCbvSrvUavHeap().getGpuDescriptorHeap()};
        mCommandList.get()->SetDescriptorHeaps((UINT)descriptorHeaps.size(), descriptorHeaps.data());

        mCommandList.get()->SetGraphicsRootSignature(mRootSignature.Get());

        mCommandList.get()->SetGraphicsRootConstantBufferView(
            d3d12::RasterRootParamSlot::FrameCB, mFrameConstantBuffer->getResource()->GetGPUVirtualAddress());

        mFrameConstantBuffer->markAsUsed(mCommandList.get());

        mCommandList.get()->RSSetViewports(1, &viewport);
        mCommandList.get()->RSSetScissorRects(1, &scissorRect);

        // Indicate that the back buffer will be used as a render target.
        D3D12_RESOURCE_BARRIER renderTargetBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
            d3d12Context.getBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

        mCommandList.get()->ResourceBarrier(1, &renderTargetBarrier);

        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = d3d12Context.getBackBufferRtv();
        D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = mDepthStencilTexture->getDsvCpu();
        mCommandList.get()->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

        mDepthStencilTexture->markAsUsed(mCommandList.get());

        // Record commands.
        const float clearColor[] = {0.0f, 0.2f, 0.4f, 1.0f};
        mCommandList.get()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
        mCommandList.get()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

        std::array<TextureBindingDesc, 1> textureBindings = {
            TextureBindingDesc{std::hash<std::string_view>()("Texture"), mTexture.get()}};

        // drawIndexed(*mHelloTrianglePso, mTriangleMesh, textureBindings);
        drawIndexed(*mHelloCubePso, mCubeMesh, textureBindings);

        // Indicate that the back buffer will now be used to present.
        renderTargetBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
            d3d12Context.getBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

        mCommandList.get()->ResourceBarrier(1, &renderTargetBarrier);
    }

    mCommandList.execute(d3d12Context.getGraphicsContext().getCommandQueue());
}

void RasterScene::endFrame() {}

//=====================================
// RayTraceScene
//=====================================
RaytracingScene::RaytracingScene(): mCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT, "RaytracingScene Command List")
{
    mRayGenCpuConstantBuffer.viewport = {-1.0f, 1.0f, 1.0f, -1.0f};

    constexpr float border = 0.1f;
    auto frameSize = d3d12::DeviceContext::instance().getFrameSize();
    const float aspectRatio = (float)frameSize.x / (float)frameSize.y;

    if(frameSize.x <= frameSize.y)
    {
        mRayGenCpuConstantBuffer.stencil = {-1 + border, -1 + border * aspectRatio, 1.0f - border,
                                            1 - border * aspectRatio};
    }
    else
    {
        mRayGenCpuConstantBuffer.stencil = {-1 + border / aspectRatio, -1 + border, 1 - border / aspectRatio,
                                            1.0f - border};
    }

    mCommandList.beginRecording();

    if(!createRenderTargets())
    {
        spdlog::critical("Failed to create raytracing render target.");
        return;
    }

    if(!createRootSignatures())
    {
        spdlog::critical("Failed to create raytracing root signatures.");
        return;
    }

    if(!createPipelineState())
    {
        spdlog::critical("Failed to create raytracing pipeline state.");
        return;
    }

    if(!buildGeometry())
    {
        spdlog::critical("Failed to create raytracing geometry.");
        return;
    }

    if(!buildAccelerationStructures())
    {
        spdlog::critical("Failed to create raytracing acceleration structures.");
        return;
    }

    if(!buildShaderTables())
    {
        spdlog::critical("Failed to create raytracing shader tables.");
        return;
    }

    d3d12::DeviceContext::instance().getCopyContext().execute();
    d3d12::DeviceContext::instance().getCopyContext().waitOnGpu();

    mCommandList.execute(d3d12::DeviceContext::instance().getGraphicsContext().getCommandQueue());

    mInitialized = true;
}

RaytracingScene::RaytracingScene(RaytracingScene&&) = default;
RaytracingScene::~RaytracingScene() = default;
RaytracingScene& RaytracingScene::operator=(RaytracingScene&&) = default;

void RaytracingScene::preRender(const FrameInfo& frameInfo) {}

void RaytracingScene::render(const FrameInfo& frameInfo, d3d12::DeviceContext& d3d12Context)
{
    mCommandList.beginRecording();

    auto DispatchRays = [&](ID3D12GraphicsCommandList4* commandList, auto* stateObject, auto* dispatchDesc) {
        // Since each shader table has only one shader record, the stride is same as the size.
        dispatchDesc->HitGroupTable.StartAddress = mHitGroupShaderTable->getResource()->GetGPUVirtualAddress();
        dispatchDesc->HitGroupTable.SizeInBytes = mHitGroupShaderTable->getByteSize();
        dispatchDesc->HitGroupTable.StrideInBytes = dispatchDesc->HitGroupTable.SizeInBytes;
        dispatchDesc->MissShaderTable.StartAddress = mMissShaderTable->getResource()->GetGPUVirtualAddress();
        dispatchDesc->MissShaderTable.SizeInBytes = mMissShaderTable->getByteSize();
        dispatchDesc->MissShaderTable.StrideInBytes = dispatchDesc->MissShaderTable.SizeInBytes;
        dispatchDesc->RayGenerationShaderRecord.StartAddress =
            mRayGenShaderTable->getResource()->GetGPUVirtualAddress();
        dispatchDesc->RayGenerationShaderRecord.SizeInBytes = mRayGenShaderTable->getByteSize();
        dispatchDesc->Width = frameInfo.mainWindow->getSize().x;
        dispatchDesc->Height = frameInfo.mainWindow->getSize().y;
        dispatchDesc->Depth = 1;
        commandList->SetPipelineState1(stateObject);
        commandList->DispatchRays(dispatchDesc);
    };

    mCommandList.get()->SetComputeRootSignature(mGlobalRootSignature.Get());

    // Bind the heaps, acceleration structure and dispatch rays.
    D3D12_DISPATCH_RAYS_DESC dispatchDesc = {};
    std::array<ID3D12DescriptorHeap*, 1> descriptorHeaps = {d3d12Context.getCbvSrvUavHeap().getGpuDescriptorHeap()};
    mCommandList.get()->SetDescriptorHeaps((uint32_t)descriptorHeaps.size(), descriptorHeaps.data());
    mCommandList.get()->SetComputeRootDescriptorTable(d3d12::RaytracingGlobalRootParamSlot::OutputView,
                                                      mRenderTarget->getUavGpu());
    mCommandList.get()->SetComputeRootShaderResourceView(d3d12::RaytracingGlobalRootParamSlot::AccelerationStructure,
                                                         mTlas->getResource()->GetGPUVirtualAddress());
    DispatchRays(mCommandList.get4(), mRaytracingStateObject.Get(), &dispatchDesc);

    D3D12_RESOURCE_BARRIER preCopyBarriers[2];
    preCopyBarriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(
        d3d12Context.getBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_COPY_DEST);
    preCopyBarriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(
        mRenderTarget->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
    mCommandList.get()->ResourceBarrier(ARRAYSIZE(preCopyBarriers), preCopyBarriers);

    mCommandList.get()->CopyResource(d3d12Context.getBackBuffer(), mRenderTarget->getResource());

    D3D12_RESOURCE_BARRIER postCopyBarriers[2];
    postCopyBarriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(
        d3d12Context.getBackBuffer(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PRESENT);
    postCopyBarriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(
        mRenderTarget->getResource(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

    mCommandList.get()->ResourceBarrier(ARRAYSIZE(postCopyBarriers), postCopyBarriers);

    mCommandList.execute(d3d12::DeviceContext::instance().getGraphicsContext().getCommandQueue());
}

void RaytracingScene::endFrame() {}

bool RaytracingScene::createRenderTargets()
{
    d3d12::DeviceContext& deviceContext = d3d12::DeviceContext::instance();

    d3d12::Texture::Params textureParams = {};
    textureParams.format = gpufmt::Format::R8G8B8A8_UNORM;
    textureParams.dimension = cputex::TextureDimension::Texture2D;
    textureParams.extents = cputex::Extent(deviceContext.getFrameSize(), 1);
    textureParams.arraySize = 1;
    textureParams.mipCount = 1;
    textureParams.colorClearValue = {0.0f, 0.0f, 0.0f, 1.0f};
    textureParams.isRenderTarget = false;
    textureParams.accessFlags = ResourceAccessFlags::GpuRW;
    textureParams.initialResourceState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
    textureParams.name = "Raytracing Render Target";

    auto renderTarget = std::make_unique<d3d12::Texture>();
    auto error = renderTarget->initUninitialized(textureParams);

    if(error)
    {
        spdlog::critical("Failed to create render target. {}"), error.value();
        return false;
    }

    mRenderTarget = std::move(renderTarget);

    return true;
}

bool RaytracingScene::createRootSignatures()
{
    d3d12::DeviceContext& deviceContext = d3d12::DeviceContext::instance();

    // Global Root Signature
    // This is a root signature that is shared across all raytracing shaders invoked during a DispatchRays() call.
    {
        std::array<D3D12_ROOT_PARAMETER1, d3d12::RaytracingGlobalRootParamSlot::Count> rootParams = {};

        D3D12_DESCRIPTOR_RANGE1 descriptorRange = {};
        descriptorRange.NumDescriptors = 1;
        descriptorRange.OffsetInDescriptorsFromTableStart = 0;
        descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
        descriptorRange.BaseShaderRegister = d3d12::reservedShaderRegister::kOutputBuffer.shaderRegister;
        descriptorRange.RegisterSpace = d3d12::reservedShaderRegister::kOutputBuffer.registerSpace;
        descriptorRange.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE;

        D3D12_ROOT_PARAMETER1& uavRootParam = rootParams[d3d12::RaytracingGlobalRootParamSlot::OutputView];
        uavRootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        uavRootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        uavRootParam.DescriptorTable.NumDescriptorRanges = 1;
        uavRootParam.DescriptorTable.pDescriptorRanges = &descriptorRange;

        // tlas = top level acceleration structure
        D3D12_ROOT_PARAMETER1& tlasRootParam = rootParams[d3d12::RaytracingGlobalRootParamSlot::AccelerationStructure];
        tlasRootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
        tlasRootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        tlasRootParam.Descriptor.ShaderRegister = d3d12::reservedShaderRegister::kAccelerationStructure.shaderRegister;
        tlasRootParam.Descriptor.RegisterSpace = d3d12::reservedShaderRegister::kAccelerationStructure.registerSpace;
        tlasRootParam.Descriptor.Flags = D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC;

        D3D12_VERSIONED_ROOT_SIGNATURE_DESC globalDesc = {};
        globalDesc.Desc_1_1.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
        globalDesc.Desc_1_1.NumParameters = (uint32_t)rootParams.size();
        globalDesc.Desc_1_1.pParameters = rootParams.data();
        globalDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;

        auto rootSigResult = SerializeAndCreateRootSignature(globalDesc);
        if(!rootSigResult)
        {
            spdlog::critical("Failed to create raytracing global root signature. {}", HRESULT_t(rootSigResult.error()));
            return false;
        }

        mGlobalRootSignature = std::move(rootSigResult.value());
        spdlog::info("Created raytracing global root signature.");
    }

    // Local Root Signature
    // This is a root signature that enables a shader to have unique arguments that come from shader tables.
    {
        std::array<D3D12_ROOT_PARAMETER1, d3d12::RaytracingLocalRootParamSlot::Count> rootParams = {};
        auto& raygenCbParam = rootParams[d3d12::RaytracingLocalRootParamSlot::ViewportCB];
        raygenCbParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
        raygenCbParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        raygenCbParam.Constants.Num32BitValues = sizeof(RayGenConstantBuffer) / sizeof(uint32_t);
        raygenCbParam.Constants.ShaderRegister = d3d12::reservedShaderRegister::kRaygenCB.shaderRegister;
        raygenCbParam.Constants.RegisterSpace = d3d12::reservedShaderRegister::kRaygenCB.registerSpace;

        D3D12_VERSIONED_ROOT_SIGNATURE_DESC desc = {};
        desc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
        desc.Desc_1_1.NumParameters = (uint32_t)rootParams.size();
        desc.Desc_1_1.pParameters = rootParams.data();
        desc.Desc_1_1.Flags = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;

        auto rootSigResult = SerializeAndCreateRootSignature(desc);
        if(!rootSigResult)
        {
            spdlog::critical("Failed to create raytracing local root signature. {}", HRESULT_t(rootSigResult.error()));
            return false;
        }

        mLocalRootSignature = std::move(rootSigResult.value());
        spdlog::info("Created raytracing local root signature.");
    }

    return true;
}

bool RaytracingScene::createPipelineState()
{
    d3d12::RaytracingShaderParams shaderParams;
    shaderParams.filepath = "assets/hello_raytracing.hlsl";
    shaderParams.stages = RaytracingShaderStageMask::RgChMs;

#ifdef _DEBUG
    shaderParams.debug = true;
#endif
    d3d12::RaytracingShader shader(std::move(shaderParams));
    shader.create();

    if(shader.status() == d3d12::RaytracingShaderState::Failed)
    {
        spdlog::critical("Failed to create raytracing shader.");
        return false;
    }

    // Create 7 subobjects that combine into a RTPSO:
    // Subobjects need to be associated with DXIL exports (i.e. shaders) either by way of default or explicit
    // associations. Default association applies to every exported shader entrypoint that doesn't have any of the
    // same type of subobject associated with it. This simple sample utilizes default shader association except for
    // local root signature subobject which has an explicit association specified purely for demonstration purposes.
    //   1 - DXIL library
    //   1 - Triangle hit group
    //   1 - Shader config
    //   2 - Local root signature and association
    //   1 - Global root signature
    //   1 - Pipeline config

    std::array<D3D12_STATE_SUBOBJECT, 7> subObjects = {};

    // DXIL library
    // This contains the shaders and their entrypoints for the state object.
    // Since shaders are not considered a subobject, they need to be passed in via DXIL library subobjects.
    std::array<D3D12_EXPORT_DESC, 3> exports = {};
    exports[0].Name = kRaygenShaderName.data();
    exports[1].Name = kClosestHitShaderName.data();
    exports[2].Name = kMissShaderName.data();

    D3D12_DXIL_LIBRARY_DESC libDesc = {};
    libDesc.DXILLibrary = shader.getShaderByteCode();
    libDesc.NumExports = (uint32_t)exports.size();
    libDesc.pExports = exports.data();

    {
        D3D12_STATE_SUBOBJECT& subOject = subObjects[0];
        subOject.Type = D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY;
        subOject.pDesc = &libDesc;
    }

    // Triangle hit group
    // A hit group specifies closest hit, any hit and intersection shaders to be executed when a ray intersects the
    // geometry's triangle/AABB. In this sample, we only use triangle geometry with a closest hit shader, so others are
    // not set.

    D3D12_HIT_GROUP_DESC hitGroupDesc = {};
    hitGroupDesc.Type = D3D12_HIT_GROUP_TYPE_TRIANGLES;
    hitGroupDesc.ClosestHitShaderImport = kClosestHitShaderName.data();
    hitGroupDesc.HitGroupExport = kHitGroupName.data();

    {
        D3D12_STATE_SUBOBJECT& subObject = subObjects[1];
        subObject.Type = D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP;
        subObject.pDesc = &hitGroupDesc;
    }

    // Shader config
    // Defines the maximum sizes in bytes for the ray payload and attribute structure.
    D3D12_RAYTRACING_SHADER_CONFIG shaderConfig = {};
    shaderConfig.MaxPayloadSizeInBytes = 4 * sizeof(float);   // float4 color
    shaderConfig.MaxAttributeSizeInBytes = 2 * sizeof(float); // float2 barycentrics

    {
        D3D12_STATE_SUBOBJECT& subObject = subObjects[2];
        subObject.Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG;
        subObject.pDesc = &shaderConfig;
    }

    D3D12_STATE_OBJECT_DESC raytracingPipeline = {};
    raytracingPipeline.Type = D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE;
    raytracingPipeline.NumSubobjects = (uint32_t)subObjects.size();
    raytracingPipeline.pSubobjects = subObjects.data();

    // Local root signature to be used in a ray gen shader.
    // This is a root signature that enables a shader to have unique arguments that come from shader tables.
    D3D12_LOCAL_ROOT_SIGNATURE localRootSignature = {};
    localRootSignature.pLocalRootSignature = mLocalRootSignature.Get();

    {
        D3D12_STATE_SUBOBJECT& subObject = subObjects[3];
        subObject.Type = D3D12_STATE_SUBOBJECT_TYPE_LOCAL_ROOT_SIGNATURE;
        subObject.pDesc = &localRootSignature;
    }

    std::array<const wchar_t*, 1> localRootSigExports = {kRaygenShaderName.data()};
    D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION exportsAssociation;
    exportsAssociation.pSubobjectToAssociate = &subObjects[3];
    exportsAssociation.NumExports = 1;
    exportsAssociation.pExports = localRootSigExports.data();

    {
        D3D12_STATE_SUBOBJECT& subObject = subObjects[4];
        subObject.Type = D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION;
        subObject.pDesc = &exportsAssociation;
    }

    // Global root signature
    // This is a root signature that is shared across all raytracing shaders invoked during a DispatchRays() call.
    D3D12_GLOBAL_ROOT_SIGNATURE globalRootSignature = {};
    globalRootSignature.pGlobalRootSignature = mGlobalRootSignature.Get();

    {
        D3D12_STATE_SUBOBJECT& subObject = subObjects[5];
        subObject.Type = D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE;
        subObject.pDesc = &globalRootSignature;
    }

    // Pipeline config
    // Defines the maximum TraceRay() recursion depth.
    D3D12_RAYTRACING_PIPELINE_CONFIG1 raytracingPipelineConfig = {};
    // PERFOMANCE TIP: Set max recursion depth as low as needed
    // as drivers may apply optimization strategies for low recursion depths.
    raytracingPipelineConfig.MaxTraceRecursionDepth = 1;
    raytracingPipelineConfig.Flags = D3D12_RAYTRACING_PIPELINE_FLAG_NONE;

    {
        D3D12_STATE_SUBOBJECT& subObject = subObjects[6];
        subObject.Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG1;
        subObject.pDesc = &raytracingPipelineConfig;
    }

    HRESULT hr = d3d12::DeviceContext::instance().getDevice5()->CreateStateObject(
        &raytracingPipeline, IID_PPV_ARGS(&mRaytracingStateObject));
    if(FAILED(hr))
    {
        spdlog::critical("Failed to create raytracing pipeline state. {}", HRESULT_t(hr));
        return false;
    }

    return true;
}

bool RaytracingScene::buildGeometry()
{
    { // Positions
        d3d12::Buffer::FormattedParams params;
        params.format = gpufmt::Format::R32G32B32_SFLOAT;
        params.numElements = 3;
        params.accessFlags = ResourceAccessFlags::GpuRead;
        params.name = "Triangle Positions Buffer";

        const float depth = 1.0f;

        std::array<glm::vec3, 3> vertices{{{0.0f, 0.7f, 1.0f}, {0.7f, -0.7f, 1.0f}, {-0.7f, -0.7f, 1.0f}}};
        mTriangleMesh.createVertexElement(ShaderVertexSemantic::Position, 0, params,
                                          nonstd::as_bytes(nonstd::span(vertices)));
    }

    //{ // UVs
    //    d3d12::Buffer::FormattedParams params;
    //    params.format = gpufmt::Format::R32G32_SFLOAT;
    //    params.numElements = 3;
    //    params.accessFlags = ResourceAccessFlags::GpuRead;
    //    params.name = "Triangle TexCoords Buffer";

    //    std::array<glm::vec2, 3> vertices{{{0.0f, 1.0f}, {1.0f, 1.0f}, {0.5f, 0.0f}}};
    //    mTriangleMesh.createVertexElement(ShaderVertexSemantic::TexCoord, 0, params,
    //                                      nonstd::as_bytes(nonstd::span(vertices)));
    //}

    //{ // Colors
    //    d3d12::Buffer::FormattedParams params;
    //    params.format = gpufmt::Format::R32G32B32A32_SFLOAT;
    //    params.numElements = 3;
    //    params.accessFlags = ResourceAccessFlags::GpuRead;
    //    params.name = "Triangle TexCoords Buffer";

    //    std::array<glm::vec4, 3> vertices{{{1.0f, 0.0, 0.0, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}}};
    //    mTriangleMesh.createVertexElement(ShaderVertexSemantic::Color, 0, params,
    //                                      nonstd::as_bytes(nonstd::span(vertices)));
    //}

    { // Indices
        GpuMesh::IndexBufferParams params;
        params.format = IndexBufferFormat::UInt16;
        params.numIndices = 3;
        params.accessFlags = ResourceAccessFlags::GpuRead;
        params.initialResourceState = D3D12_RESOURCE_STATE_COMMON;
        params.name = "Triangle Index Buffer";

        std::array<uint16_t, 3> indices{0, 1, 2};
        mTriangleMesh.initIndices(params, nonstd::as_bytes(nonstd::span(indices)));
    }

    return true;
}

bool RaytracingScene::buildAccelerationStructures()
{
    auto device = d3d12::DeviceContext::instance().getDevice5();
    auto commandList = mCommandList.get4();
    auto commandQueue = d3d12::DeviceContext::instance().getGraphicsContext().getCommandQueue();

    std::shared_ptr<d3d12::Buffer> vertexBuffer = mTriangleMesh.getVertexBuffer(ShaderVertexSemantic::Position, 0);

    D3D12_RAYTRACING_GEOMETRY_DESC geometryDesc = {};
    geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
    geometryDesc.Triangles.IndexBuffer = mTriangleMesh.getIndexBuffer()->getResource()->GetGPUVirtualAddress();
    geometryDesc.Triangles.IndexCount = mTriangleMesh.getIndexCount();
    geometryDesc.Triangles.IndexFormat = mTriangleMesh.getIndexBuffer()->getIndexView().Format;
    geometryDesc.Triangles.Transform3x4 = 0;
    geometryDesc.Triangles.VertexFormat = vertexBuffer->getFormat();
    geometryDesc.Triangles.VertexCount = (uint32_t)vertexBuffer->getElementCount();
    geometryDesc.Triangles.VertexBuffer.StartAddress = vertexBuffer->getResource()->GetGPUVirtualAddress();
    geometryDesc.Triangles.VertexBuffer.StrideInBytes = vertexBuffer->getElementByteSize();

    // Mark the geometry as opaque.
    // PERFORMANCE TIP: mark geometry as opaque whenever applicable as it can enable important ray processing
    // optimizations. Note: When rays encounter opaque geometry an any hit shader will not be executed whether it is
    // present or not.
    geometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;

    // Get required sizes for an acceleration structure.
    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS buildFlags =
        D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS topLevelInputs = {};
    topLevelInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
    topLevelInputs.Flags = buildFlags;
    topLevelInputs.NumDescs = 1;
    topLevelInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;

    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO topLevelPrebuildInfo = {};
    device->GetRaytracingAccelerationStructurePrebuildInfo(&topLevelInputs, &topLevelPrebuildInfo);
    if(topLevelPrebuildInfo.ResultDataMaxSizeInBytes == 0) { return false; }

    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO bottomLevelPrebuildInfo = {};
    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS bottomLevelInputs = topLevelInputs;
    bottomLevelInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
    bottomLevelInputs.NumDescs = 1;
    bottomLevelInputs.pGeometryDescs = &geometryDesc;

    device->GetRaytracingAccelerationStructurePrebuildInfo(&bottomLevelInputs, &bottomLevelPrebuildInfo);
    if(bottomLevelPrebuildInfo.ResultDataMaxSizeInBytes == 0) { return false; }

    d3d12::Buffer::SimpleParams scratchBufferParams;
    scratchBufferParams.accessFlags = ResourceAccessFlags::None;
    scratchBufferParams.byteSize =
        std::max(topLevelPrebuildInfo.ScratchDataSizeInBytes, bottomLevelPrebuildInfo.ScratchDataSizeInBytes);
    scratchBufferParams.flags = d3d12::BufferFlags::UavEnabled;
    scratchBufferParams.name = "Scratch Resource";

    d3d12::Buffer scratchBuffer;
    scratchBuffer.init(scratchBufferParams);

    // Allocate resources for acceleration structures.
    // Acceleration structures can only be placed in resources that are created in the default heap (or custom heap
    // equivalent). Default heap is OK since the application doesn’t need CPU read/write access to them. The resources
    // that will contain acceleration structures must be created in the state
    // D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE, and must have resource flag
    // D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS. The ALLOW_UNORDERED_ACCESS requirement simply acknowledges both:
    //  - the system will be doing this type of access in its implementation of acceleration structure builds behind the
    //  scenes.
    //  - from the app point of view, synchronization of writes/reads to acceleration structures is accomplished using
    //  UAV barriers.
    {
        d3d12::Buffer::SimpleParams blasParams;
        blasParams.accessFlags = ResourceAccessFlags::None;
        blasParams.byteSize = bottomLevelPrebuildInfo.ResultDataMaxSizeInBytes;
        blasParams.flags = d3d12::BufferFlags::AccelerationStructure;
        blasParams.initialResourceState = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
        blasParams.name = "BLAS";

        mBlas = std::make_unique<d3d12::Buffer>();
        mBlas->init(blasParams);

        d3d12::Buffer::SimpleParams tlasParams = blasParams;
        tlasParams.byteSize = topLevelPrebuildInfo.ResultDataMaxSizeInBytes;
        tlasParams.name = "TLAS";

        mTlas = std::make_unique<d3d12::Buffer>();
        mTlas->init(tlasParams);
    }

    //// Create an instance desc for the bottom-level acceleration structure.
    D3D12_RAYTRACING_INSTANCE_DESC instanceDesc = {};
    instanceDesc.Transform[0][0] = instanceDesc.Transform[1][1] = instanceDesc.Transform[2][2] = 1;
    instanceDesc.InstanceMask = 1;
    instanceDesc.AccelerationStructure = mBlas->getResource()->GetGPUVirtualAddress();

    d3d12::Buffer::SimpleParams params;
    params.accessFlags = ResourceAccessFlags::None;
    params.byteSize = sizeof(instanceDesc);
    params.name = "BLAS Instance Descs";

    d3d12::Buffer instanceDescs;
    instanceDescs.init(params, nonstd::as_bytes(nonstd::span(&instanceDesc, 1)));

    //// Bottom Level Acceleration Structure desc
    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC bottomLevelBuildDesc = {};
    {
        bottomLevelBuildDesc.Inputs = bottomLevelInputs;
        bottomLevelBuildDesc.ScratchAccelerationStructureData = scratchBuffer.getResource()->GetGPUVirtualAddress();
        bottomLevelBuildDesc.DestAccelerationStructureData = mBlas->getResource()->GetGPUVirtualAddress();
    }

    //// Top Level Acceleration Structure desc
    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC topLevelBuildDesc = {};
    {
        topLevelInputs.InstanceDescs = instanceDescs.getResource()->GetGPUVirtualAddress();
        topLevelBuildDesc.Inputs = topLevelInputs;
        topLevelBuildDesc.DestAccelerationStructureData = mTlas->getResource()->GetGPUVirtualAddress();
        topLevelBuildDesc.ScratchAccelerationStructureData = scratchBuffer.getResource()->GetGPUVirtualAddress();
    }

    scratchBuffer.markAsUsed(mCommandList.get());

    //// Build acceleration structure.
    mCommandList.get4()->BuildRaytracingAccelerationStructure(&bottomLevelBuildDesc, 0, nullptr);
    D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::UAV(mBlas->getResource());
    commandList->ResourceBarrier(1, &barrier);
    mCommandList.get4()->BuildRaytracingAccelerationStructure(&topLevelBuildDesc, 0, nullptr);

    instanceDescs.markAsUsed(commandList);
    mBlas->markAsUsed(commandList);
    mTlas->markAsUsed(commandList);

    return true;
}

template<typename T>
constexpr nonstd::span<const std::byte> AsBytes(const T& value)
{
    return nonstd::span<const std::byte>(reinterpret_cast<const std::byte*>(&value), sizeof(T));
}

bool RaytracingScene::buildShaderTables()
{
    auto device = d3d12::DeviceContext::instance().getDevice5();

    static constexpr uint32_t kShaderIdentifierSize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
    static constexpr std::array<std::byte, kShaderIdentifierSize> kEmptyShaderIdentifierArray = {};
    using ShaderIdentifier = nonstd::span<const std::byte, kShaderIdentifierSize>;
    static constexpr ShaderIdentifier kEmptyShaderIdentifier(kEmptyShaderIdentifierArray);
    ShaderIdentifier rayGenShaderIdentifier = kEmptyShaderIdentifier;
    ShaderIdentifier missShaderIdentifier = kEmptyShaderIdentifier;
    ShaderIdentifier hitGroupShaderIdentifier = kEmptyShaderIdentifier;

    // Get shader identifiers.
    {
        ComPtr<ID3D12StateObjectProperties> stateObjectProperties;
        if(FAILED(mRaytracingStateObject.As(&stateObjectProperties))) { return false; }

        auto GetShaderIdentifier = [](ID3D12StateObjectProperties* stateObjectProperties,
                                      std::wstring_view shaderName) {
            return ShaderIdentifier(
                reinterpret_cast<std::byte*>(stateObjectProperties->GetShaderIdentifier(shaderName.data())),
                D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
        };

        rayGenShaderIdentifier = GetShaderIdentifier(stateObjectProperties.Get(), kRaygenShaderName);
        missShaderIdentifier = GetShaderIdentifier(stateObjectProperties.Get(), kMissShaderName);
        hitGroupShaderIdentifier = GetShaderIdentifier(stateObjectProperties.Get(), kHitGroupName);
    }

    //// Ray gen shader table
    struct ShaderRecord
    {
        ShaderIdentifier shaderIdentifier;
        nonstd::span<const std::byte> localRootArguments;
    };

    auto MakeShaderTable = [](ID3D12GraphicsCommandList* commandList, nonstd::span<const ShaderRecord> shaderRecords,
                              uint32_t shaderRecordSize, std::string_view name) {
        shaderRecordSize = AlignInteger(shaderRecordSize, uint32_t(D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT));
        d3d12::Buffer::SimpleParams params;
        params.accessFlags = ResourceAccessFlags::CpuWrite;
        params.flags = d3d12::BufferFlags::None;
        params.byteSize = shaderRecordSize * shaderRecords.size();
        params.initialResourceState = D3D12_RESOURCE_STATE_COMMON;
        params.name = name;

        auto gpuBuffer = std::make_unique<d3d12::Buffer>();
        gpuBuffer->init(params);

        {
            d3d12::GpuBufferWriteGuard writeGuard(*gpuBuffer.get(), commandList);
            nonstd::span<std::byte> writeBuffer = writeGuard.getWriteBuffer();
            auto itr = writeBuffer.begin();

            for(const ShaderRecord& shaderRecord : shaderRecords)
            {
                itr = std::copy(shaderRecord.shaderIdentifier.begin(), shaderRecord.shaderIdentifier.end(), itr);
                itr = std::copy(shaderRecord.localRootArguments.begin(), shaderRecord.localRootArguments.end(), itr);
            }
        }

        auto resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
            gpuBuffer->getResource(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COMMON);
        commandList->ResourceBarrier(1, &resourceBarrier);
        return gpuBuffer;
    };

    {
        struct RootArguments
        {
            RayGenConstantBuffer cb;
        } rootArguments;
        rootArguments.cb = mRayGenCpuConstantBuffer;

        const std::array<ShaderRecord, 1> shaderRecords = {
            ShaderRecord{rayGenShaderIdentifier, AsBytes(rootArguments)}};

        const UINT shaderRecordSize = kShaderIdentifierSize + sizeof(rootArguments);
        mRayGenShaderTable =
            MakeShaderTable(mCommandList.get(), shaderRecords, shaderRecordSize, "RayGen Shader Table");
    }

    // Miss shader table
    {
        std::array<ShaderRecord, 1> shaderRecords = {ShaderRecord{missShaderIdentifier}};

        const UINT shaderRecordSize = kShaderIdentifierSize;
        mMissShaderTable = MakeShaderTable(mCommandList.get(), shaderRecords, shaderRecordSize, "Miss Shader Table");
    }

    // Hit group shader table
    {
        std::array<ShaderRecord, 1> shaderRecords = {ShaderRecord{hitGroupShaderIdentifier}};
        const UINT shaderRecordSize = kShaderIdentifierSize;
        mHitGroupShaderTable =
            MakeShaderTable(mCommandList.get(), shaderRecords, shaderRecordSize, "Hit Group Shader Table");
    }

    return true;
}

//=====================================
// RenderScene
//=====================================

RenderScene::RenderScene()
{
    d3d12::DeviceContext& deviceContext = d3d12::DeviceContext::instance();

    mRasterScene = std::make_unique<RasterScene>();

    if(deviceContext.isRaytracingSupported()) { mRaytraceScene = std::make_unique<RaytraceScene>(); }
}

void RenderScene::preRender(const FrameInfo& frameInfo)
{
    if(mActiveScene == Scene::Raster) { mRasterScene->preRender(frameInfo); }
    else if(mRaytraceScene != nullptr && mActiveScene == Scene::Raytrace)
    {
        mRaytraceScene->preRender(frameInfo);
    }
}

void RenderScene::render(const FrameInfo& frameInfo, d3d12::DeviceContext& d3d12Context)
{
    if(mActiveScene == Scene::Raster) { mRasterScene->render(frameInfo, d3d12Context); }
    else if(mRaytraceScene != nullptr && mActiveScene == Scene::Raytrace)
    {
        mRaytraceScene->render(frameInfo, d3d12Context);
    }
}

void RenderScene::endFrame()
{
    if(mActiveScene == Scene::Raster) { mRasterScene->endFrame(); }
    else if(mRaytraceScene != nullptr && mActiveScene == Scene::Raytrace)
    {
        mRaytraceScene->endFrame();
    }
}
} // namespace scrap
