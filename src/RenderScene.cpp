#include "RenderScene.h"

#include "CpuMesh.h"
#include "FrameInfo.h"
#include "PrimitiveMesh.h"
#include "SpanUtility.h"
#include "Window.h"
#include "d3d12/D3D12BLAccelerationStructure.h"
#include "d3d12/D3D12Buffer.h"
#include "d3d12/D3D12Command.h"
#include "d3d12/D3D12Context.h"
#include "d3d12/D3D12Debug.h"
#include "d3d12/D3D12GraphicsPipelineState.h"
#include "d3d12/D3D12GraphicsShader.h"
#include "d3d12/D3D12RaytracingDispatchPipelineState.h"
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
// RasterRenderer
//=====================================
RasterRenderer::RasterRenderer(): mCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT, "RasterRenderer Command List")
{
    mCommandList.beginRecording();

    createRenderTargets();
    createRootSignature();
    createFrameConstantBuffer();

    d3d12::DeviceContext& deviceContext = d3d12::DeviceContext::instance();
    deviceContext.getCopyContext().execute();

    mCommandList.execute(deviceContext.getGraphicsContext().getCommandQueue());

    mInitialized = true;
}

RasterRenderer::RasterRenderer(RasterRenderer&&) = default;

RasterRenderer::~RasterRenderer() = default;

bool RasterRenderer::createRootSignature()
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

void RasterRenderer::createRenderTargets()
{
    // Currently using the swap chain as our render targets. Just need the depth/stencil buffer
    d3d12::TextureParams params = {};
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

void RasterRenderer::createFrameConstantBuffer()
{
    d3d12::BufferSimpleParams params;
    params.accessFlags = ResourceAccessFlags::CpuWrite | ResourceAccessFlags::GpuRead;
    params.byteSize = sizeof(FrameConstantBuffer);
    params.flags = d3d12::BufferFlags::ConstantBuffer;
    params.name = "Frame Constant Buffer";

    auto constantBuffer = std::make_unique<d3d12::Buffer>();
    constantBuffer->init(params);

    mFrameConstantBuffer = std::move(constantBuffer);
}

RasterRenderer& RasterRenderer::operator=(RasterRenderer&&) = default;

void RasterRenderer::preRender(const FrameInfo& frameInfo, const RenderParams& renderParams)
{
    mCommandList.beginRecording();

    {
        d3d12::GpuBufferWriteGuard<d3d12::Buffer> writeGuard(*mFrameConstantBuffer, mCommandList.get());
        auto frameCbSpan = writeGuard.getWriteBufferAs<FrameConstantBuffer>();
        frameCbSpan.front() = renderParams.frameConstants;
    }
}

void RasterRenderer::bindTextures(Material& material)
{
    const d3d12::GraphicsShader& shader = *material.mRasterPipelineState->getShader();

    // bind resources
    for(const auto& [key, texture] : material.textures)
    {
        const uint32_t textureIndex =
            shader.getResourceIndex(key, ShaderResourceType::Texture, texture->getShaderResourceDimension())
                .value_or(d3d12::kMaxBindlessResources);
        mCommandList.setBindlessResource(textureIndex, texture->getSrvDescriptorHeapIndex());
    }
}

void RasterRenderer::render(const FrameInfo& frameInfo, const RenderParams& renderParams)
{
    d3d12::DeviceContext& d3d12Context = d3d12::DeviceContext::instance();

    {
        d3d12::ScopedGpuEvent gpuEvent(mCommandList.get(), "RasterRenderer");

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

        for(RenderObject& renderObject : renderParams.renderObjects)
        {
            bindTextures(renderObject.mMaterial);
            d3d12::drawIndexed(mCommandList, d3d12::DrawIndexedParams{
                                                 .indexBuffer = renderObject.mGpuMesh->getIndexBuffer().get(),
                                                 .pipelineState = renderObject.mMaterial.mRasterPipelineState.get(),
                                                 .vertexBuffers = renderObject.mGpuMesh->getVertexElements(),
                                                 .primitiveTopology = renderObject.mGpuMesh->getPrimitiveTopology(),
                                                 .indexCount = renderObject.mGpuMesh->getIndexCount(),
                                                 .instanceCount = 1});
        }

        // Indicate that the back buffer will now be used to present.
        renderTargetBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
            d3d12Context.getBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

        mCommandList.get()->ResourceBarrier(1, &renderTargetBarrier);
    }

    mCommandList.execute(d3d12Context.getGraphicsContext().getCommandQueue());
}

void RasterRenderer::endFrame()
{
    mCommandList.endFrame();
}

std::shared_ptr<d3d12::GraphicsPipelineState>
RasterRenderer::createPipelineState(d3d12::GraphicsShaderParams&& shaderParams,
                                    d3d12::GraphicsPipelineStateParams&& pipelineStateParams)
{
    pipelineStateParams.rootSignature = mRootSignature;
    pipelineStateParams.shader = std::make_shared<d3d12::GraphicsShader>(std::move(shaderParams));
    pipelineStateParams.renderTargetFormats = {DXGI_FORMAT_R8G8B8A8_UNORM};
    pipelineStateParams.depthStencilFormat = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

    return d3d12::DeviceContext::instance().createGraphicsPipelineState(std::move(pipelineStateParams));
}

//=====================================
// RayTraceScene
//=====================================
RaytracingRenderer::RaytracingRenderer()
    : mCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT, "RaytracingRenderer Command List")
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

    if(!createInternalPipelineStates())
    {
        spdlog::critical("Failed to create raytracing pipeline state.");
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

RaytracingRenderer::RaytracingRenderer(RaytracingRenderer&&) = default;
RaytracingRenderer::~RaytracingRenderer() = default;
RaytracingRenderer& RaytracingRenderer::operator=(RaytracingRenderer&&) = default;

void RaytracingRenderer::preRender(const FrameInfo& frameInfo, const RenderParams& renderParams)
{
    mCommandList.beginRecording();

    {
        d3d12::ScopedGpuEvent gpuEvent(mCommandList.get(), "Update Constant Buffers");

        d3d12::GpuBufferWriteGuard<d3d12::Buffer> writeGuard(*mFrameConstantBuffer, mCommandList.get());
        auto frameCbSpan = writeGuard.getWriteBufferAs<FrameConstantBuffer>();
        frameCbSpan.front() = renderParams.frameConstants;
    }

    if(!mShaderTable->isReady()) { return; }

    d3d12::ScopedGpuEvent updateMaterialsEvent(mCommandList.get(), "Update Materials");
    struct BindlessIndices
    {
        std::array<uint32_t, d3d12::kMaxBindlessResources> resourceIndices = {};
        std::array<uint32_t, d3d12::kMaxBindlessVertexBuffers> vertexBufferIndices = {};
    } bindlessIndices;

    mShaderTable->beginUpdate(mCommandList);

    for(RenderObject& renderObject : renderParams.renderObjects)
    {
        mStringBuffer.clear();
        fmt::format_to(std::back_inserter(mStringBuffer), "{} {}", renderObject.name, renderObject.mId.value());
        d3d12::ScopedGpuEvent renderObjectEvent(mCommandList.get(), mStringBuffer);

        std::shared_ptr<d3d12::BLAccelerationStructure>& blas = renderObject.mGpuMesh->accessBlas();
        if(blas->getBuildState() == d3d12::AccelerationStructureState::Invalid) { blas->build(mCommandList); }

        if(!renderObject.mInstanceAllocation.isValid())
        {
            auto addResult = mTlas->addInstance(
                d3d12::TLAccelerationStructureInstanceParams{.accelerationStructure = renderObject.mGpuMesh->getBlas(),
                                                             .transform = glm::identity<glm::mat4x3>(),
                                                             .flags = d3d12::TlasInstanceFlags::TriangleFrontCcw,
                                                             .instanceId = renderObject.mId.value()});

            if(!addResult) { continue; }

            renderObject.mInstanceAllocation = std::move(addResult.value());
        }

        renderObject.mInstanceAllocation.updateTransform(renderObject.mTransform);

        if(!renderObject.mMaterial.mShaderTableAllocation.isValid())
        {
            mDispatchPipelineState->addPipelineState(renderObject.mMaterial.mRaytracingPipelineState);

            auto addResult =
                mShaderTable->addPipelineState(renderObject.mMaterial.mRaytracingPipelineState, {}, mCommandList.get());

            if(!addResult) { continue; }

            renderObject.mMaterial.mShaderTableAllocation = std::move(addResult.value());
        }

        std::shared_ptr<d3d12::RaytracingShader> shader = renderObject.mMaterial.mRaytracingPipelineState->getShader();

        { // bind vertex buffers
            // creating an array with one more than necessary to give a slot to assign buffers not used by the
            // shader.
            for(const d3d12::VertexBuffer& vertexElement : renderObject.mGpuMesh->getVertexElements())
            {
                const std::optional<uint32_t> constantBufferIndex = shader->getVertexElementIndex(
                    RaytracingShaderStage::ClosestHit, vertexElement.semantic, vertexElement.semanticIndex);

                if(!constantBufferIndex) { continue; }
                bindlessIndices.vertexBufferIndices[constantBufferIndex.value()] =
                    vertexElement.buffer->getSrvDescriptorHeapIndex();

                vertexElement.buffer->markAsUsed(mCommandList.get());
            }
        }

        { // bind index buffer
            const std::optional<uint32_t> indexBufferIndex =
                shader->getResourceIndex(RaytracingShaderStage::ClosestHit, SharedString("IndexBuffer"),
                                         ShaderResourceType::Buffer, ShaderResourceDimension::Buffer);

            if(indexBufferIndex)
            {
                bindlessIndices.resourceIndices[indexBufferIndex.value()] =
                    renderObject.mGpuMesh->getIndexBuffer()->getSrvDescriptorHeapIndex();

                renderObject.mGpuMesh->getIndexBuffer()->markAsUsed(mCommandList.get());
            }
        }

        // bind resources
        for(const auto& [key, texture] : renderObject.mMaterial.textures)
        {
            const std::optional<uint32_t> textureIndex =
                shader->getResourceIndex(RaytracingShaderStage::ClosestHit, key, ShaderResourceType::Texture,
                                         texture->getShaderResourceDimension());

            if(textureIndex)
            {
                bindlessIndices.resourceIndices[textureIndex.value()] = texture->getSrvDescriptorHeapIndex();
                texture->markAsUsed(mCommandList.get());
            }
        }

        renderObject.mMaterial.mShaderTableAllocation.updateLocalRootArguments(
            RaytracingPipelineStage::HitGroup, ToByteSpan(bindlessIndices), mCommandList.get());
    }

    mShaderTable->endUpdate(mCommandList);
}

void RaytracingRenderer::render(const FrameInfo& frameInfo, const RenderParams& renderParams)
{
    d3d12::DeviceContext& d3d12Context = d3d12::DeviceContext::instance();

    {
        d3d12::ScopedGpuEvent buildTlasEvent(mCommandList.get(), "Build TLAS");
        mTlas->build(mCommandList);
    }

    mCommandList.get()->SetComputeRootSignature(mGlobalRootSignature.Get());

    {
        d3d12::ScopedGpuEvent mainPassEvent(mCommandList.get(), "Main Pass");

        std::array<d3d12::TLAccelerationStructure*, 1> accelerationStructures{mTlas.get()};
        std::array<d3d12::Texture*, 1> rwTextures{mRenderTarget.get()};

        d3d12::EngineConstantBuffers engineConstantBuffers;
        engineConstantBuffers.frame = mFrameConstantBuffer.get();

        auto windowSize = frameInfo.mainWindow->getSize();

        d3d12::dispatchRays(mCommandList, d3d12::DispatchRaysParams{.pipelineState = mDispatchPipelineState.get(),
                                                                    .accelerationStructures = accelerationStructures,
                                                                    .shaderTable = mShaderTable.get(),
                                                                    .constantBuffers = engineConstantBuffers,
                                                                    .rwTextures = rwTextures,
                                                                    .dimensions = {windowSize.x, windowSize.y, 1}});
    }

    {
        d3d12::ScopedGpuEvent presentEvent(mCommandList.get(), "Copy To Backbuffer");

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
    }

    mCommandList.execute(d3d12::DeviceContext::instance().getGraphicsContext().getCommandQueue());
}

void RaytracingRenderer::endFrame()
{
    mCommandList.endFrame();
}

bool RaytracingRenderer::createRenderTargets()
{
    d3d12::DeviceContext& deviceContext = d3d12::DeviceContext::instance();

    d3d12::TextureParams textureParams = {};
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

bool RaytracingRenderer::createRootSignatures()
{
    d3d12::DeviceContext& deviceContext = d3d12::DeviceContext::instance();

    // Global Root Signature
    // This is a root signature that is shared across all raytracing shaders invoked during a DispatchRays() call.
    {
        std::array<D3D12_ROOT_PARAMETER1, d3d12::RaytracingGlobalRootParamSlot::Count> rootParams = {};

        // Output UAV
        D3D12_DESCRIPTOR_RANGE1 descriptorRange = {};
        descriptorRange.NumDescriptors = 1;
        descriptorRange.OffsetInDescriptorsFromTableStart = 0;
        descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
        descriptorRange.BaseShaderRegister = d3d12::reservedShaderRegister::kOutputBuffer.shaderRegister;
        descriptorRange.RegisterSpace = d3d12::reservedShaderRegister::kOutputBuffer.registerSpace;
        descriptorRange.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE;

        auto& uavRootParam = rootParams[d3d12::RaytracingGlobalRootParamSlot::OutputView];
        uavRootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        uavRootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        uavRootParam.DescriptorTable.NumDescriptorRanges = 1;
        uavRootParam.DescriptorTable.pDescriptorRanges = &descriptorRange;

        // tlas = top level acceleration structure
        auto& tlasRootParam = rootParams[d3d12::RaytracingGlobalRootParamSlot::AccelerationStructure];
        tlasRootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
        tlasRootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        tlasRootParam.Descriptor.ShaderRegister = d3d12::reservedShaderRegister::kAccelerationStructure.shaderRegister;
        tlasRootParam.Descriptor.RegisterSpace = d3d12::reservedShaderRegister::kAccelerationStructure.registerSpace;
        tlasRootParam.Descriptor.Flags = D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC;

        // Frame constant buffer
        auto& frameCbRootParam = rootParams[d3d12::RaytracingGlobalRootParamSlot::FrameCB];
        frameCbRootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        frameCbRootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        frameCbRootParam.Descriptor.ShaderRegister = d3d12::reservedShaderRegister::kFrameCB.shaderRegister;
        frameCbRootParam.Descriptor.RegisterSpace = d3d12::reservedShaderRegister::kFrameCB.registerSpace;
        frameCbRootParam.Descriptor.Flags = D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC;

        // Static samplers
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
        sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

        D3D12_VERSIONED_ROOT_SIGNATURE_DESC globalDesc = {};
        globalDesc.Desc_1_1.Flags = D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED;
        globalDesc.Desc_1_1.NumParameters = (uint32_t)rootParams.size();
        globalDesc.Desc_1_1.pParameters = rootParams.data();
        globalDesc.Desc_1_1.NumStaticSamplers = (uint32_t)staticSamplers.size();
        globalDesc.Desc_1_1.pStaticSamplers = staticSamplers.data();
        globalDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;

        auto rootSigResult = SerializeAndCreateRootSignature(globalDesc);
        if(!rootSigResult)
        {
            spdlog::critical("Failed to create raytracing global root signature. {}", HRESULT_t(rootSigResult.error()));
            return false;
        }

        rootSigResult.value()->SetName(L"Raytracing Global Root Signature");

        mGlobalRootSignature = std::move(rootSigResult.value());
        spdlog::info("Created raytracing global root signature.");
    }

    // Local Root Signatures
    // This is a root signature that enables a shader to have unique arguments that come from shader tables.
    {
        std::array<D3D12_ROOT_PARAMETER1, d3d12::RaytracingClosestHitLocalRootParamSlot::Count> rootParams = {};

        // Bindless resource indices
        auto& resourceIndicesParam = rootParams[d3d12::RaytracingClosestHitLocalRootParamSlot::ResourceIndices];
        resourceIndicesParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
        resourceIndicesParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        resourceIndicesParam.Constants.Num32BitValues = d3d12::kMaxBindlessResources;
        resourceIndicesParam.Constants.ShaderRegister = d3d12::reservedShaderRegister::kResourceCB.shaderRegister;
        resourceIndicesParam.Constants.RegisterSpace = d3d12::reservedShaderRegister::kResourceCB.registerSpace;

        // Bindless vertex buffer indices
        auto& vertexIndicesParam = rootParams[d3d12::RaytracingClosestHitLocalRootParamSlot::VertexIndices];
        vertexIndicesParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
        vertexIndicesParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        vertexIndicesParam.Constants.Num32BitValues = d3d12::kMaxBindlessVertexBuffers;
        vertexIndicesParam.Constants.ShaderRegister = d3d12::reservedShaderRegister::kVertexCB.shaderRegister;
        vertexIndicesParam.Constants.RegisterSpace = d3d12::reservedShaderRegister::kVertexCB.registerSpace;

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

        rootSigResult.value()->SetName(L"Closest Hit Local Root Signature");
        mLocalRootSignatures[(size_t)RaytracingShaderStage::ClosestHit] = std::move(rootSigResult.value());
        spdlog::info("Created raytracing local root signature.");
    }

    d3d12::BufferSimpleParams frameCbParams;
    frameCbParams.accessFlags = ResourceAccessFlags::CpuWrite;
    frameCbParams.byteSize = sizeof(FrameConstantBuffer);
    frameCbParams.flags = d3d12::BufferFlags::ConstantBuffer;

    auto frameCbBuffer = std::make_unique<d3d12::Buffer>();
    frameCbBuffer->init(frameCbParams);

    mFrameConstantBuffer = std::move(frameCbBuffer);

    return true;
}

bool RaytracingRenderer::createInternalPipelineStates()
{
    { // Dispatch pipeline state
        mDispatchPipelineState = std::make_unique<d3d12::RaytracingDispatchPipelineState>();
        mDispatchPipelineState->init(d3d12::RaytracingDispatchPipelineStateParams{
            .globalRootSignature = mGlobalRootSignature,
            .maxPayloadByteSize = 4 * sizeof(float),   // float4 color
            .maxAttributeByteSize = 2 * sizeof(float), // float2 barycentrics
        });
    }

    { // Main pass pipeline state
        SharedString raygenEntryPointName("mainPassRaygen");
        SharedString missEntryPointName("mainPassMiss");

        std::array<d3d12::RaytracingFixedStageShaderEntryPoint, 2> fixedStageShaderEntryPoints = {
            d3d12::RaytracingFixedStageShaderEntryPoint{RaytracingShaderStage::RayGen, raygenEntryPointName},
            d3d12::RaytracingFixedStageShaderEntryPoint{RaytracingShaderStage::Miss, missEntryPointName}};

        d3d12::RaytracingShaderParams shaderParams;
        shaderParams.filepath = "assets/raytracing_mainpass.hlsl";
        shaderParams.fixedStageEntryPoints = fixedStageShaderEntryPoints;

#ifdef _DEBUG
        shaderParams.debug = true;
#endif
        auto shader = std::make_shared<d3d12::RaytracingShader>(std::move(shaderParams));

        d3d12::RaytracingPipelineStateParams pipelineStateParams;
        pipelineStateParams.shader = shader;

        pipelineStateParams.fixedStages[(size_t)RaytracingShaderStage::RayGen] =
            d3d12::RaytracingPipelineStateShaderParams{.shaderEntryPointIndex = shader->getFixedStageIndex(
                                                           RaytracingShaderStage::RayGen, raygenEntryPointName)};

        pipelineStateParams.fixedStages[(size_t)RaytracingShaderStage::Miss] =
            d3d12::RaytracingPipelineStateShaderParams{
                .shaderEntryPointIndex = shader->getFixedStageIndex(RaytracingShaderStage::Miss, missEntryPointName)};

        // pipelineStateParams.hitGroupName = hitGroupName;
        pipelineStateParams.primitiveType = d3d12::RaytracingPipelineStatePrimitiveType::Triangles;
        pipelineStateParams.maxRecursionDepth = 1;

        mMainPassPipelineState = std::make_shared<d3d12::RaytracingPipelineState>(std::move(pipelineStateParams));
        mMainPassPipelineState->create();

        mDispatchPipelineState->addPipelineState(mMainPassPipelineState);
    }

    return true;
}

std::shared_ptr<d3d12::RaytracingPipelineState>
RaytracingRenderer::createPipelineState(d3d12::RaytracingShaderParams&& shaderParams)
{
    auto shader = std::make_shared<d3d12::RaytracingShader>(std::move(shaderParams));

    d3d12::RaytracingPipelineStateParams pipelineStateParams{};
    pipelineStateParams.shader = shader;

    // for now just pulling out the first shader in each stage
    for(auto stage : enumerate<RaytracingShaderStage>())
    {
        const auto* stageShaderInfo = shader->getFixedStageShader(stage, 0);

        if(stageShaderInfo == nullptr) { continue; }

        pipelineStateParams.fixedStages[(size_t)stage] =
            d3d12::RaytracingPipelineStateShaderParams{.localRootSignature = mLocalRootSignatures[(size_t)stage],
                                                       .shaderEntryPointIndex = stageShaderInfo->index};
    }

    std::span callableShaders = shader->getCallableShaders();
    std::vector<d3d12::RaytracingPipelineStateShaderParams> callableParams;
    callableParams.reserve(callableShaders.size());

    for(size_t i = 0; i < callableShaders.size(); ++i)
    {
        callableParams.emplace_back(d3d12::RaytracingPipelineStateShaderParams{.shaderEntryPointIndex = i});
    }

    std::string hitGroupName = fmt::format("{} hit group", shaderParams.filepath.generic_string());
    pipelineStateParams.hitGroupName = hitGroupName;

    auto pipelineState = std::make_shared<d3d12::RaytracingPipelineState>(std::move(pipelineStateParams));

    pipelineState->create();

    return pipelineState;
}

bool RaytracingRenderer::buildAccelerationStructures()
{
    d3d12::TLAccelerationStructureParams tlasParams;
    tlasParams.initialReservedObjects = 1;
    tlasParams.buildOption = d3d12::AccelerationStructureBuildOption::FastTrace;
    tlasParams.flags = d3d12::AccelerationStructureFlags::AllowUpdates;
    tlasParams.instanceDescs.accessFlags = ResourceAccessFlags::None;
    tlasParams.instanceDescs.bufferFlags = d3d12::BufferFlags::None;
    tlasParams.name = "TLAS";

    mTlas = std::make_unique<d3d12::TLAccelerationStructure>(tlasParams);

    return true;
}

template<typename T>
constexpr std::span<const std::byte> AsBytes(const T& value)
{
    return std::span<const std::byte>(reinterpret_cast<const std::byte*>(&value), sizeof(T));
}

bool RaytracingRenderer::buildShaderTables()
{
    struct RootArguments
    {
        FrameConstantBuffer cb;
    } rootArguments{};

    d3d12::ShaderTableParams params;
    params.raygen.entryByteStride = 0;
    params.raygen.capacity = 1;
    params.hitGroup.entryByteStride = sizeof(RootArguments);
    params.hitGroup.capacity = 1;
    params.miss.entryByteStride = 0;
    params.miss.capacity = 1;
    params.name = "Shader Table";

    mShaderTable = std::make_shared<d3d12::ShaderTable>();
    mShaderTable->init(params);

    std::array<std::span<const std::byte>, (size_t)RaytracingPipelineStage::Count> localRootArguments{
        {{}, ToByteSpan(rootArguments), {}}};

    {
        auto shaderTableResult = mShaderTable->addPipelineState(mMainPassPipelineState, {}, mCommandList.get());
        if(!shaderTableResult) { return false; }
        mMainPassShaderTableAllocation = std::move(shaderTableResult.value());
    }

    return true;
}

//=====================================
// RenderScene
//=====================================

RenderScene::RenderScene()
{
    d3d12::DeviceContext& deviceContext = d3d12::DeviceContext::instance();

    mRasterScene = std::make_unique<RasterRenderer>();

    if(deviceContext.isRaytracingSupported()) { mRaytraceScene = std::make_unique<RaytracingRenderer>(); }

    mCamera.setPosition(glm::vec3(0.0f, 0.0f, -5.0f));

    createRenderObject();
}

void RenderScene::preRender(const FrameInfo& frameInfo)
{
    if(frameInfo.keyboard->getKeyState(SDLK_SPACE).pressedCount > 0)
    {
        mActiveScene = (mActiveScene == Scene::Raster) ? Scene::Raytracing : Scene::Raster;
    }

    mCamera.update(frameInfo);

    mRenderObjects.front().mTransform =
        glm::rotate(static_cast<glm::mat4x4>(mRenderObjects.front().mTransform), 0.01f, glm::vec3(0.0f, 1.0f, 0.0f));

    const auto windowSize = frameInfo.mainWindow->getSize();

    {
        const glm::mat4x4 worldToViewMat = mCamera.worldToViewMatrix();
        const glm::mat4x4 viewToWorldMat = glm::inverse(worldToViewMat);
        const glm::mat4x4 viewToClipMat =
            glm::perspectiveFovLH_ZO(1.04719f, (float)windowSize.x, (float)windowSize.y, 0.1f, 100.0f);
        const glm::mat4x4 clipToViewMat = glm::inverse(viewToClipMat);
        const glm::mat4x4 worldToClipMat = viewToClipMat * worldToViewMat;
        const glm::mat4x4 clipToWorldMat = glm::inverse(worldToClipMat);

        FrameConstantBuffer& frameCb = mRenderParams.frameConstants;
        frameCb.worldToView = glm::transpose(worldToViewMat);
        frameCb.viewToWorld = glm::transpose(viewToWorldMat);
        frameCb.viewToClip = glm::transpose(viewToClipMat);
        frameCb.clipToView = glm::transpose(clipToViewMat);
        frameCb.worldToClip = glm::transpose(worldToClipMat);
        frameCb.clipToWorld = glm::transpose(clipToWorldMat);
        frameCb.cameraWorldPos = mCamera.getPosition();
        frameCb.time = std::chrono::duration_cast<std::chrono::duration<float>>(
                           std::chrono::steady_clock::now().time_since_epoch())
                           .count();
        frameCb.frameTimeDelta = frameInfo.frameDeltaSec.count();
    }

    mRenderParams.renderObjects = mRenderObjects;

    if(mActiveScene == Scene::Raster) { mRasterScene->preRender(frameInfo, mRenderParams); }
    else if(mRaytraceScene != nullptr && mActiveScene == Scene::Raytracing)
    {
        mRaytraceScene->preRender(frameInfo, mRenderParams);
    }
}

void RenderScene::render(const FrameInfo& frameInfo, d3d12::DeviceContext& d3d12Context)
{
    if(mActiveScene == Scene::Raster) { mRasterScene->render(frameInfo, mRenderParams); }
    else if(mRaytraceScene != nullptr && mActiveScene == Scene::Raytracing)
    {
        mRaytraceScene->render(frameInfo, mRenderParams);
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

GpuMesh RenderScene::createCube()
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

    return GpuMesh(cubeMesh, ResourceAccessFlags::GpuRead, "Cube");
}

std::shared_ptr<d3d12::Texture> RenderScene::createTexture()
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

    return texture;
}

bool RenderScene::createRenderObject()
{
    mTexture = createTexture();

    RenderObject renderObject{RenderObjectId(mNextRenderObjectId++)};
    renderObject.name = SharedString("Cube");
    renderObject.mGpuMesh = std::make_shared<GpuMesh>(createCube());
    renderObject.mTransform = glm::identity<glm::mat4x3>();

    {
        d3d12::GraphicsShaderParams shaderParams;
        shaderParams.filepaths[(size_t)GraphicsShaderStage::Vertex] = L"assets\\hello_cube.hlsl";
        shaderParams.filepaths[(size_t)GraphicsShaderStage::Pixel] = L"assets\\hello_cube.hlsl";
        shaderParams.entryPoints[(size_t)GraphicsShaderStage::Vertex] = "VSMain";
        shaderParams.entryPoints[(size_t)GraphicsShaderStage::Pixel] = "PSMain";
        shaderParams.stages = GraphicsShaderStageMask::VsPs;
#ifdef _DEBUG
        shaderParams.debug = true;
#endif

        d3d12::GraphicsPipelineStateParams pipelineStateParams = {};
        pipelineStateParams.rasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        pipelineStateParams.rasterizerState.FrontCounterClockwise = true;
        pipelineStateParams.blendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        pipelineStateParams.depthStencilState.DepthEnable = TRUE;
        pipelineStateParams.depthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
        pipelineStateParams.depthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
        pipelineStateParams.depthStencilState.StencilEnable = FALSE;
        pipelineStateParams.primitiveTopologyType =
            d3d12::TranslatePrimitiveTopologyType(renderObject.mGpuMesh->getPrimitiveTopology());

        renderObject.mMaterial.mRasterPipelineState =
            mRasterScene->createPipelineState(std::move(shaderParams), std::move(pipelineStateParams));
    }

    if(mRaytraceScene)
    {
        d3d12::RaytracingShaderParams shaderParams;
        shaderParams.filepath = "assets/raytracing_basic3d.hlsl";

        std::array fixedStageEntryPoints{
            d3d12::RaytracingFixedStageShaderEntryPoint{.stage = RaytracingShaderStage::ClosestHit,
                                                        .entryPoint = SharedString("MyClosestHitShader")}};
        shaderParams.fixedStageEntryPoints = fixedStageEntryPoints;

#ifdef _DEBUG
        shaderParams.debug = true;
#endif
        renderObject.mMaterial.mRaytracingPipelineState = mRaytraceScene->createPipelineState(std::move(shaderParams));
    }

    renderObject.mMaterial.textures.insert(eastl::make_pair(SharedString("Texture"), mTexture));
    mRenderObjects.emplace_back(std::move(renderObject));

    return true;
}

} // namespace scrap
