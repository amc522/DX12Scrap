#include "d3d12/D3D12Execute.h"

#include "d3d12/D3D12CommandList.h"
#include "d3d12/D3D12Context.h"
#include "d3d12/D3D12FixedDescriptorHeap.h"
#include "d3d12/D3D12RaytracingPipelineState.h"
#include "d3d12/D3D12ShaderTable.h"
#include "d3d12/D3D12TLAccelerationStructure.h"
#include "d3d12/D3D12Texture.h"

#include <d3d12.h>

namespace scrap::d3d12
{
void bindEngineConstantBuffers(d3d12::GraphicsCommandList& commandList, const EngineConstantBuffers& engineConstantBuffers)
{
    if(engineConstantBuffers.frame)
    {
        engineConstantBuffers.frame->markAsUsed(commandList.get());
        commandList.get()->SetComputeRootConstantBufferView(
            d3d12::RaytracingGlobalRootParamSlot::FrameCB,
            engineConstantBuffers.frame->getResource()->GetGPUVirtualAddress());
    }
}

bool dispatchRays(d3d12::GraphicsCommandList& commandList, const DispatchRaysParams& params)
{
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
    params.pipelineState->markAsUsed(commandList.get());
    commandList.get4()->SetPipelineState1(params.pipelineState->getPipelineState());

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

    return true;
}
} // namespace scrap::d3d12