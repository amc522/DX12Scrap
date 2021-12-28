#include "d3d12/D3D12Command.h"

#include "d3d12/D3D12CommandList.h"
#include "d3d12/D3D12Context.h"
#include "d3d12/D3D12FixedDescriptorHeap.h"
#include "d3d12/D3D12GraphicsPipelineState.h"
#include "d3d12/D3D12GraphicsShader.h"
#include "d3d12/D3D12RaytracingDispatchPipelineState.h"
#include "d3d12/D3D12ShaderTable.h"
#include "d3d12/D3D12TLAccelerationStructure.h"
#include "d3d12/D3D12Texture.h"
#include "d3d12/D3D12Translations.h"
#include "d3d12/D3D12VertexBuffer.h"

#include <d3d12.h>

namespace scrap::d3d12
{
void bindEngineConstantBuffers(d3d12::GraphicsCommandList& commandList,
                               const EngineConstantBuffers& engineConstantBuffers)
{
    if(engineConstantBuffers.frame)
    {
        engineConstantBuffers.frame->markAsUsed(commandList.get());
        commandList.get()->SetComputeRootConstantBufferView(
            d3d12::RaytracingGlobalRootParamSlot::FrameCB,
            engineConstantBuffers.frame->getResource()->GetGPUVirtualAddress());
    }
}

std::optional<CommandError> drawIndexed(d3d12::GraphicsCommandList& commandList, const DrawIndexedParams& params)
{
    if(!params.pipelineState->isReady()) return CommandError::ResourcesNotReady;
    if(!params.indexBuffer->isReady()) return CommandError::ResourcesNotReady;

    for(const VertexBuffer& vertexBuffer : params.vertexBuffers)
    {
        if(vertexBuffer.buffer == nullptr) { continue; }
        if(!vertexBuffer.buffer->isReady()) { return CommandError::ResourcesNotReady; }
    }

    const d3d12::GraphicsShader& shader = *params.pipelineState->getShader();

    std::array<uint32_t, d3d12::kMaxBindlessVertexBuffers + 1> vertexBufferDescriptorIndices;

    for(const VertexBuffer& vertexBuffer : params.vertexBuffers)
    {
        const uint32_t constantBufferIndex =
            shader.getVertexElementIndex(vertexBuffer.semantic, vertexBuffer.semanticIndex)
                .value_or(d3d12::kMaxBindlessVertexBuffers);

        vertexBufferDescriptorIndices[constantBufferIndex] = vertexBuffer.buffer->getSrvDescriptorHeapIndex();

        vertexBuffer.buffer->markAsUsed(commandList.get());
    }

    commandList.get()->SetGraphicsRoot32BitConstants(
        d3d12::RasterRootParamSlot::VertexIndices,
        std::min((uint32_t)params.vertexBuffers.size(), d3d12::kMaxBindlessVertexBuffers),
        vertexBufferDescriptorIndices.data(), 0);

    commandList.commitBindlessResources();

    params.indexBuffer->markAsUsed(commandList.get());
    params.pipelineState->markAsUsed(commandList.get());

    const D3D_PRIMITIVE_TOPOLOGY primitiveTopology = d3d12::TranslatePrimitiveTopology(params.primitiveTopology);
    const D3D12_INDEX_BUFFER_VIEW ibv = params.indexBuffer->getIndexView();

    commandList.get()->SetPipelineState(params.pipelineState->getPipelineState());
    commandList.get()->IASetPrimitiveTopology(primitiveTopology);
    commandList.get()->IASetIndexBuffer(&ibv);
    commandList.get()->DrawIndexedInstanced(params.indexCount, params.instanceCount, params.indexOffset,
                                            params.vertexOffset, params.instanceOffset);

    return std::nullopt;
}

std::optional<CommandError> dispatchRays(d3d12::GraphicsCommandList& commandList, const DispatchRaysParams& params)
{
    for(d3d12::TLAccelerationStructure* tlas : params.accelerationStructures)
    {
        if(!tlas->isReady()) { return CommandError::ResourcesNotReady; }
    }

    for(d3d12::Texture* rwTexture : params.rwTextures)
    {
        if(!rwTexture->isReady()) { return CommandError::ResourcesNotReady; }
    }

    d3d12::DeviceContext& d3d12Context = d3d12::DeviceContext::instance();

    // Setup descriptor heaps for that bindless life
    std::array<ID3D12DescriptorHeap*, 1> descriptorHeaps = {d3d12Context.getCbvSrvUavHeap().getGpuDescriptorHeap()};
    commandList.get()->SetDescriptorHeaps((uint32_t)descriptorHeaps.size(), descriptorHeaps.data());

    // Texture UAVs (render targets)
    for(d3d12::Texture* rwTexture : params.rwTextures)
    {
        rwTexture->markAsUsed(commandList.get());
    }

    commandList.get()->SetComputeRootDescriptorTable(d3d12::RaytracingGlobalRootParamSlot::OutputView,
                                                     params.rwTextures[0]->getUavGpu());

    // Acceleration structure
    for(d3d12::TLAccelerationStructure* tlas : params.accelerationStructures)
    {
        tlas->markAsUsed(commandList);
    }

    commandList.get()->SetComputeRootShaderResourceView(
        d3d12::RaytracingGlobalRootParamSlot::AccelerationStructure,
        params.accelerationStructures[0]->getAccelerationStructureBuffer().getResource()->GetGPUVirtualAddress());

    // Constant buffers
    bindEngineConstantBuffers(commandList, params.constantBuffers);

    // Pipeline state
    params.pipelineState->markAsUsed(commandList);
    commandList.get4()->SetPipelineState1(params.pipelineState->getStateObject());

    // Dispatch
    D3D12_DISPATCH_RAYS_DESC dispatchDesc = {};
    dispatchDesc.HitGroupTable = params.shaderTable->getHitGroupTableAddressRangeAndStride(0);
    dispatchDesc.MissShaderTable = params.shaderTable->getMissTableAddressRangeAndStride(0);
    dispatchDesc.RayGenerationShaderRecord = params.shaderTable->getRaygenRecordAddressAndStride(0);
    dispatchDesc.Width = params.dimensions.x;
    dispatchDesc.Height = params.dimensions.y;
    dispatchDesc.Depth = params.dimensions.z;

    params.shaderTable->markAsUsed(commandList.get());

    commandList.get4()->DispatchRays(&dispatchDesc);

    return std::nullopt;
}
} // namespace scrap::d3d12