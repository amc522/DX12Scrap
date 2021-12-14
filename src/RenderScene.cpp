#include "RenderScene.h"

#include "CpuMesh.h"
#include "FrameInfo.h"
#include "PrimitiveMesh.h"
#include "SpanUtility.h"
#include "Window.h"
#include "d3d12/D3D12BLAccelerationStructure.h"
#include "d3d12/D3D12Buffer.h"
#include "d3d12/D3D12Context.h"
#include "d3d12/D3D12Debug.h"
#include "d3d12/D3D12GraphicsPipelineState.h"
#include "d3d12/D3D12GraphicsShader.h"
#include "d3d12/D3D12RaytracingPipelineState.h"
#include "d3d12/D3D12RaytracingShader.h"
#include "d3d12/D3D12Strings.h"
#include "d3d12/D3D12TLAccelerationStructure.h"
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

void RasterScene::endFrame()
{
    mCommandList.endFrame();
}

//=====================================
// RayTraceScene
//=====================================
RaytracingScene::RaytracingScene(): mCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT, "RaytracingScene Command List")
{
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

    mCamera.setPosition(glm::vec3(0.0f, 0.0f, -5.0f));

    mInitialized = true;
}

RaytracingScene::RaytracingScene(RaytracingScene&&) = default;
RaytracingScene::~RaytracingScene() = default;
RaytracingScene& RaytracingScene::operator=(RaytracingScene&&) = default;

void RaytracingScene::preRender(const FrameInfo& frameInfo)
{
    mCamera.update(frameInfo);
    const auto windowSize = frameInfo.mainWindow->getSize();

    const glm::mat4x4 worldToViewMat = mCamera.worldToViewMatrix();
    const glm::mat4x4 viewToClipMat =
        glm::perspectiveFovLH_ZO(1.04719f, (float)windowSize.x, (float)windowSize.y, 0.1f, 100.0f);
    const glm::mat4x4 worldToClipMat = viewToClipMat * worldToViewMat;
    const glm::mat4x4 clipToWorldMat = glm::inverse(worldToClipMat);

    RayGenConstantBuffer raygenCb;
    raygenCb.clipToWorld = glm::transpose(clipToWorldMat);
    raygenCb.cameraWorldPos = mCamera.getPosition();

    mShaderTableAllocation.updateLocalRootArguments(RaytracingPipelineStage::RayGen, ToByteSpan(raygenCb),
                                                    d3d12::DeviceContext::instance().getCopyContext().getCommandList());

    mCubeMeshTransform = glm::rotate(mCubeMeshTransform, 0.01f, glm::vec3(0.0f, 1.0f, 0.0f));
    mTlasInstance.updateTransform(static_cast<glm::mat4x3>(mCubeMeshTransform));
}

void RaytracingScene::render(const FrameInfo& frameInfo, d3d12::DeviceContext& d3d12Context)
{
    mCommandList.beginRecording();

    mTlas->build(mCommandList);

    auto DispatchRays = [&](ID3D12GraphicsCommandList4* commandList, ID3D12StateObject* stateObject,
                            D3D12_DISPATCH_RAYS_DESC* dispatchDesc) {
        mShaderTable->markAsUsed(commandList);

        dispatchDesc->HitGroupTable = mShaderTable->getHitGroupTableAddressRangeAndStride(0);
        dispatchDesc->MissShaderTable = mShaderTable->getMissTableAddressRangeAndStride(0);
        dispatchDesc->RayGenerationShaderRecord = mShaderTable->getRaygenRecordAddressAndStride(0);
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
    mCommandList.get()->SetComputeRootShaderResourceView(
        d3d12::RaytracingGlobalRootParamSlot::AccelerationStructure,
        mTlas->getAccelerationStructureBuffer().getResource()->GetGPUVirtualAddress());

    mPipelineState->markAsUsed(mCommandList.get());
    mTlas->markAsUsed(mCommandList);

    DispatchRays(mCommandList.get4(), mPipelineState->getPipelineState(), &dispatchDesc);

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

void RaytracingScene::endFrame()
{
    mCommandList.endFrame();
}

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
        auto& raygenCbParam = rootParams[d3d12::RaytracingLocalRootParamSlot::RayGenCb];
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
    std::array<d3d12::RaytracingFixedStageShaderEntryPoint, 3> fixedStageShaderEntryPoints = {
        d3d12::RaytracingFixedStageShaderEntryPoint{RaytracingShaderStage::RayGen, SharedString("MyRaygenShader")},
        d3d12::RaytracingFixedStageShaderEntryPoint{RaytracingShaderStage::ClosestHit,
                                                    SharedString("MyClosestHitShader")},
        d3d12::RaytracingFixedStageShaderEntryPoint{RaytracingShaderStage::Miss, SharedString("MyMissShader")}};

    d3d12::RaytracingShaderParams shaderParams;
    shaderParams.filepath = "assets/raytracing_basic3d.hlsl";
    shaderParams.fixedStageEntryPoints = fixedStageShaderEntryPoints;

#ifdef _DEBUG
    shaderParams.debug = true;
#endif
    mShader = std::make_shared<d3d12::RaytracingShader>(std::move(shaderParams));

    std::array localRootSignatures = {mLocalRootSignature};
    d3d12::RaytracingPipelineStateParams pipelineStateParams;
    pipelineStateParams.globalRootSignature = mGlobalRootSignature;
    pipelineStateParams.localRootSignatures = localRootSignatures;
    pipelineStateParams.shaders = nonstd::span(&mShader, 1);

    pipelineStateParams.fixedStages.raygen.shaderIndex = 0;
    pipelineStateParams.fixedStages.raygen.shaderEntryPointIndex = 0;
    pipelineStateParams.fixedStages.raygen.localRootSignatureIndex = 0;

    pipelineStateParams.fixedStages.closestHit.shaderIndex = 0;
    pipelineStateParams.fixedStages.closestHit.shaderEntryPointIndex = 1;

    pipelineStateParams.fixedStages.miss.shaderIndex = 0;
    pipelineStateParams.fixedStages.miss.shaderEntryPointIndex = 2;

    pipelineStateParams.hitGroupName = "MyHitGroup";
    pipelineStateParams.primitiveType = d3d12::RaytracingPipelineStatePrimitiveType::Triangles;
    pipelineStateParams.maxAttributeByteSize = 2 * sizeof(float); // float2 barycentrics
    pipelineStateParams.maxPayloadByteSize = 4 * sizeof(float);   // float4 color
    pipelineStateParams.maxRecursionDepth = 1;

    mPipelineState = std::make_shared<d3d12::RaytracingPipelineState>(std::move(pipelineStateParams));
    mPipelineState->create();

    return true;
}

bool RaytracingScene::buildGeometry()
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

    return true;
}

bool RaytracingScene::buildAccelerationStructures()
{
    {
        d3d12::BLAccelerationStructureParams blasParams;
        blasParams.initialReservedObjects = 1;
        blasParams.buildOption = d3d12::AccelerationStructureBuildOption::FastTrace;
        blasParams.flags = d3d12::AccelerationStructureFlags::None;
        blasParams.name = "BLAS";

        mBlas = std::make_shared<d3d12::BLAccelerationStructure>(blasParams);

        d3d12::BLAccelerationStructure::GeometryParams geomParams;
        geomParams.flags = d3d12::RaytracingGeometryFlags::None;
        geomParams.indexBuffer = mCubeMesh.getIndexBuffer();
        geomParams.vertexBuffer = mCubeMesh.getVertexBuffer(ShaderVertexSemantic::Position, 0);

        mBlas->addMesh(geomParams);
        mBlas->build(mCommandList);
    }

    {
        d3d12::TLAccelerationStructureParams tlasParams;
        tlasParams.initialReservedObjects = 1;
        tlasParams.buildOption = d3d12::AccelerationStructureBuildOption::FastTrace;
        tlasParams.flags = d3d12::AccelerationStructureFlags::AllowUpdates;
        tlasParams.instanceDescs.accessFlags = ResourceAccessFlags::None;
        tlasParams.instanceDescs.bufferFlags = d3d12::BufferFlags::None;
        tlasParams.name = "TLAS";

        mTlas = std::make_unique<d3d12::TLAccelerationStructure>(tlasParams);

        d3d12::TLAccelerationStructure::InstanceParams instanceParams;
        instanceParams.accelerationStructure = mBlas;
        instanceParams.flags = d3d12::TlasInstanceFlags::TriangleFrontCcw;
        instanceParams.instanceContributionToHitGroupIndex = 0;
        instanceParams.instanceId = 0;
        instanceParams.instanceMask = 1;
        instanceParams.transform = glm::identity<glm::mat4x3>();

        auto instanceAddResult = mTlas->addInstance(instanceParams);
        if(!instanceAddResult) { return false; }

        mTlasInstance = std::move(instanceAddResult.value());
    }

    return true;
}

template<typename T>
constexpr nonstd::span<const std::byte> AsBytes(const T& value)
{
    return nonstd::span<const std::byte>(reinterpret_cast<const std::byte*>(&value), sizeof(T));
}

bool RaytracingScene::buildShaderTables()
{
    struct RootArguments
    {
        RayGenConstantBuffer cb;
    } rootArguments{};

    d3d12::ShaderTableParams params;
    params.raygen.entryByteStride = sizeof(RootArguments);
    params.raygen.capacity = 1;
    params.hitGroup.entryByteStride = 0;
    params.hitGroup.capacity = 1;
    params.miss.entryByteStride = 0;
    params.miss.capacity = 1;
    params.name = "Shader Table";

    mShaderTable = std::make_shared<d3d12::ShaderTable>();
    mShaderTable->init(params);

    std::array<nonstd::span<const std::byte>, (size_t)RaytracingPipelineStage::Count> localRootArguments{
        ToByteSpan(rootArguments),
        {},
        {}};

    auto shaderTableResult = mShaderTable->addPipelineState(mPipelineState, localRootArguments, mCommandList.get());

    if(!shaderTableResult) { return false; }

    mShaderTableAllocation = std::move(shaderTableResult.value());

    return true;
}

//=====================================
// RenderScene
//=====================================

RenderScene::RenderScene()
{
    d3d12::DeviceContext& deviceContext = d3d12::DeviceContext::instance();

    mRasterScene = std::make_unique<RasterScene>();

    if(deviceContext.isRaytracingSupported()) { mRaytraceScene = std::make_unique<RaytracingScene>(); }
}

void RenderScene::preRender(const FrameInfo& frameInfo)
{
    if(mActiveScene == Scene::Raster) { mRasterScene->preRender(frameInfo); }
    else if(mRaytraceScene != nullptr && mActiveScene == Scene::Raytracing)
    {
        mRaytraceScene->preRender(frameInfo);
    }
}

void RenderScene::render(const FrameInfo& frameInfo, d3d12::DeviceContext& d3d12Context)
{
    if(mActiveScene == Scene::Raster) { mRasterScene->render(frameInfo, d3d12Context); }
    else if(mRaytraceScene != nullptr && mActiveScene == Scene::Raytracing)
    {
        mRaytraceScene->render(frameInfo, d3d12Context);
    }
}

void RenderScene::endFrame()
{
    if(mActiveScene == Scene::Raster) { mRasterScene->endFrame(); }
    else if(mRaytraceScene != nullptr && mActiveScene == Scene::Raytracing)
    {
        mRaytraceScene->endFrame();
    }
}
} // namespace scrap
