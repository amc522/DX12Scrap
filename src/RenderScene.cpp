#include "RenderScene.h"

#include "CpuMesh.h"
#include "d3d12/D3D12Buffer.h"
#include "d3d12/D3D12Context.h"
#include "d3d12/D3D12Debug.h"
#include "d3d12/D3D12GraphicsPipelineState.h"
#include "d3d12/D3D12GraphicsShader.h"
#include "d3d12/D3D12Texture.h"
#include "d3d12/D3D12Translations.h"

#include <cputex/utility.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <d3dx12.h>
#include <spdlog/spdlog.h>

using namespace Microsoft::WRL;

namespace scrap
{
RenderScene::RenderScene(d3d12::DeviceContext& d3d12Context)
    : mCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT, "RenderScene")
{
    // Create an empty root signature.
    {
        D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

        // This is the highest version the sample supports. If CheckFeatureSupport succeeds, the HighestVersion returned
        // will not be greater than this.
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

        if(FAILED(d3d12Context.getDevice()->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData,
                                                                sizeof(featureData))))
        {
            featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
        }

        // https://docs.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_root_parameter1
        // The root parameter describes a parameter or argument being passed into the shader as described or declared by
        // the root signautre. Here we are saying we have a constant buffer with 3 constants.
        std::array<D3D12_ROOT_PARAMETER1, 3> rootParameters = {};

        D3D12_ROOT_PARAMETER1& frameConstantBuffer = rootParameters[d3d12::RootParamIndex::kRootParamIndex_FrameCB];
        frameConstantBuffer.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        frameConstantBuffer.Descriptor.ShaderRegister = d3d12::reservedShaderRegister::kFrameCB.shaderRegister;
        frameConstantBuffer.Descriptor.RegisterSpace = d3d12::reservedShaderRegister::kFrameCB.registerSpace;
        frameConstantBuffer.Descriptor.Flags = D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE;
        frameConstantBuffer.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

        D3D12_ROOT_PARAMETER1& resourceRootConstants = rootParameters[d3d12::kRootParamIndex_ResourceIndices];
        resourceRootConstants.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
        resourceRootConstants.Constants.Num32BitValues = d3d12::kMaxBindlessResources;
        resourceRootConstants.Constants.ShaderRegister = d3d12::reservedShaderRegister::kResourceCB.shaderRegister;
        resourceRootConstants.Constants.RegisterSpace = d3d12::reservedShaderRegister::kResourceCB.registerSpace;
        resourceRootConstants.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

        D3D12_ROOT_PARAMETER1& vertexRootConstants = rootParameters[d3d12::kRootParamIndex_VertexIndices];
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

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        HRESULT hr =
            D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error);
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
            return;
        }

        hr = d3d12Context.getDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(),
                                                           IID_PPV_ARGS(&mRootSignature));

        if(FAILED(hr))
        {
            spdlog::critical("Failed to create d3d12 root signature");
            return;
        }

        spdlog::info("Created d3d12 root signature");
    }

    createFrameConstantBuffer();
    createTriangle();
    createTexture();

    if(FAILED(mCommandList.close())) { spdlog::error("Failed to close graphics command list"); }

    // Execute the command list.
    std::array<ID3D12CommandList*, 1> commandLists = {mCommandList.get()};
    d3d12Context.getGraphicsContext().getCommandQueue()->ExecuteCommandLists(static_cast<UINT>(commandLists.size()),
                                                                             commandLists.data());

    // Wait for the command list to execute; we are reusing the same command
    // list in our main loop but for now, we just want to wait for setup to
    // complete before continuing.
    d3d12Context.getGraphicsContext().waitOnGpu();

    if(!loadShaders()) { return; }

    mInitialized = true;
}

RenderScene::RenderScene(RenderScene&&) = default;

RenderScene::~RenderScene() = default;

bool RenderScene::loadShaders()
{
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
        psoParams.primitiveTopologyType = d3d12::TranslatePrimitiveTopologyType(mTriangleMesh.getPrimitiveTopology());
        psoParams.renderTargetFormats = {DXGI_FORMAT_R8G8B8A8_UNORM};

        mHelloTrianglePso = d3d12::DeviceContext::instance().createGraphicsPipelineState(std::move(psoParams));
    }

    return true;
}

void RenderScene::createFrameConstantBuffer()
{
    d3d12::Buffer::SimpleParams params;
    params.accessFlags = ResourceAccessFlags::CpuWrite | ResourceAccessFlags::GpuRead;
    params.byteSize = sizeof(FrameConstantBuffer);
    params.isConstantBuffer = true;
    params.name = "Frame Constant Buffer";

    auto constantBuffer = std::make_unique<d3d12::Buffer>();
    constantBuffer->init(params);

    mFrameConstantBuffer = std::move(constantBuffer);
}

void RenderScene::createTriangle()
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
        d3d12::Buffer::FormattedParams params;
        params.format = gpufmt::Format::R16_UINT;
        params.numElements = 3;
        params.accessFlags = ResourceAccessFlags::GpuRead;
        params.name = "Index Buffer";

        std::array<uint16_t, 3> indices{0, 1, 2};
        mTriangleMesh.initIndices(IndexBufferFormat::UInt16, 3, ResourceAccessFlags::GpuRead, "Index Buffer",
                                  nonstd::as_bytes(nonstd::span(indices)));
    }
}

void RenderScene::createTexture()
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

RenderScene& RenderScene::operator=(RenderScene&&) = default;

void RenderScene::preRender()
{
    // This function is called before the copy context command list is executed for the frame.

    // Update relevant constant buffers on the copy context command list so they are ready before the frame is rendered.
    {
        d3d12::GpuBufferWriteGuard writeGuard(*mFrameConstantBuffer.get(),
                                              d3d12::DeviceContext::instance().getCopyContext().getCommandList());

        nonstd::span<FrameConstantBuffer> buffer = writeGuard.getWriteBufferAs<FrameConstantBuffer>();
        buffer.front().time = std::chrono::duration_cast<std::chrono::duration<float>>(
                                  std::chrono::steady_clock::now().time_since_epoch())
                                  .count();
    }
}

void RenderScene::drawIndexed(d3d12::GraphicsPipelineState& pso,
                              GpuMesh& mesh,
                              nonstd::span<TextureBindingDesc> textures)
{
    if(!pso.isReady() || !mesh.isReady()) { return; }

    if(textures.size() > d3d12::kMaxBindlessResources)
    {
        spdlog::warn("Binding more textures than bindless slots available.");
    }

    const d3d12::GraphicsShader& shader = *mHelloTrianglePso->getShader();

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
            (UINT)d3d12::kRootParamIndex_VertexIndices,
            std::min(d3d12::kMaxBindlessVertexBuffers, (uint32_t)shader.getShaderInputs().vertexElements.size()),
            vertexBufferDescriptorIndices.data(), 0);
    }

    { // bind resources
        std::array<uint32_t, d3d12::kMaxBindlessResources + 1> resourceIndices;

        for(const TextureBindingDesc& textureBindingDesc : textures)
        {
            const uint32_t textureIndex =
                shader.getResourceIndex(textureBindingDesc.nameHash).value_or(d3d12::kMaxBindlessResources);
            resourceIndices[textureIndex] = mTexture->getSrvDescriptorHeapIndex();
        }

        mCommandList.get()->SetGraphicsRoot32BitConstants(
            d3d12::kRootParamIndex_ResourceIndices,
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

void RenderScene::render(d3d12::DeviceContext& d3d12Context)
{
    mCommandList.reset();

    {
        d3d12::ScopedGpuEvent gpuEvent(mCommandList.get(), "RenderScene");

        D3D12_VIEWPORT viewport{};
        viewport.TopLeftX = 0.0f;
        viewport.TopLeftY = 0.0f;
        viewport.Width = static_cast<float>(d3d12Context.frameSize().x);
        viewport.Height = static_cast<float>(d3d12Context.frameSize().y);
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
            d3d12::RootParamIndex::kRootParamIndex_FrameCB,
            mFrameConstantBuffer->getResource()->GetGPUVirtualAddress());

        mCommandList.get()->RSSetViewports(1, &viewport);
        mCommandList.get()->RSSetScissorRects(1, &scissorRect);

        // Indicate that the back buffer will be used as a render target.
        D3D12_RESOURCE_BARRIER renderTargetBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
            d3d12Context.getBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

        mCommandList.get()->ResourceBarrier(1, &renderTargetBarrier);

        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = d3d12Context.getBackBufferRtv();
        mCommandList.get()->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

        // Record commands.
        const float clearColor[] = {0.0f, 0.2f, 0.4f, 1.0f};
        mCommandList.get()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

        std::array<TextureBindingDesc, 1> textureBindings = {
            TextureBindingDesc{std::hash<std::string_view>()("Texture"), mTexture.get()}};

        drawIndexed(*mHelloTrianglePso, mTriangleMesh, textureBindings);

        // Indicate that the back buffer will now be used to present.
        renderTargetBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
            d3d12Context.getBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

        mCommandList.get()->ResourceBarrier(1, &renderTargetBarrier);
    }

    if(FAILED(mCommandList.close()))
    {
        spdlog::error("Failed to close d3d12 command list");
        return;
    }

    // Execute the command list.
    std::array<ID3D12CommandList*, 1> commandLists = {mCommandList.get()};
    d3d12Context.getGraphicsContext().getCommandQueue()->ExecuteCommandLists(static_cast<UINT>(commandLists.size()),
                                                                             commandLists.data());
}

} // namespace scrap
