#include "d3d12/D3D12TrackedGpuObject.h"

#include "d3d12/D3D12Context.h"

#include <d3d12.h>
#include <spdlog/spdlog.h>

namespace scrap::d3d12
{
void UpdateFrameCode(D3D12_COMMAND_LIST_TYPE commandListType,
                     RenderFrameCode& renderFrameCode,
                     CopyFrameCode& copyFrameCode)
{
    switch(commandListType)
    {
    case D3D12_COMMAND_LIST_TYPE_DIRECT:
        renderFrameCode = DeviceContext::instance().getGraphicsContext().getCurrentFrameCode();
        break;
    case D3D12_COMMAND_LIST_TYPE_COPY:
        copyFrameCode = DeviceContext::instance().getCopyContext().getCurrentFrameCode();
        break;
    default:
        assert(false);
        spdlog::critical("TrackedDeviceChild::markAsUsed unsupported D3D12_COMMAND_LIST_TYPE '{}'", commandListType);
        break;
    }
}

void UpdateFrameCode(ID3D12CommandQueue* commandQueue, RenderFrameCode& renderFrameCode, CopyFrameCode& copyFrameCode)
{
    UpdateFrameCode(commandQueue->GetDesc().Type, renderFrameCode, copyFrameCode);
}

void UpdateFrameCode(ID3D12CommandList* commandList, RenderFrameCode& renderFrameCode, CopyFrameCode& copyFrameCode)
{
    UpdateFrameCode(commandList->GetType(), renderFrameCode, copyFrameCode);
}

void TrackedDeviceChild::destroy()
{
    if(mDeviceChild == nullptr) { return; }

    DeviceContext::instance().getGraphicsContext().queueObjectForDestruction(mDeviceChild, mLastUsedRenderFrameCode);
    DeviceContext::instance().getCopyContext().queueObjectForDestruction(std::move(mDeviceChild),
                                                                         mLastUsedCopyFrameCode);
}

void TrackedDeviceChild::markAsUsed(ID3D12CommandQueue* commandQueue)
{
    UpdateFrameCode(commandQueue, mLastUsedRenderFrameCode, mLastUsedCopyFrameCode);
}

void TrackedDeviceChild::markAsUsed(ID3D12CommandList* commandList)
{
    UpdateFrameCode(commandList, mLastUsedRenderFrameCode, mLastUsedCopyFrameCode);
}

void TrackedShaderResource::destroy()
{
    if(mResource == nullptr) { return; }

    DeviceContext& deviceContext = DeviceContext::instance();
    deviceContext.getGraphicsContext().queueObjectForDestruction(mResource, std::move(mDescriptorHeapReservation),
                                                                 mLastUsedRenderFrameCode);
    deviceContext.getCopyContext().queueObjectForDestruction(std::move(mResource), mLastUsedCopyFrameCode);
}

void TrackedShaderResource::setDescriptorHeapReservation(FixedDescriptorHeapReservation &&descriptorHeapReservation)
{
    if(mDescriptorHeapReservation.isValid())
    {
        DeviceContext::instance().getGraphicsContext().queueObjectForDestruction(std::move(mDescriptorHeapReservation),
                                                                                 mLastUsedRenderFrameCode);
    }

    mDescriptorHeapReservation = std::move(descriptorHeapReservation);
}

void TrackedShaderResource::markAsUsed(ID3D12CommandQueue* commandQueue)
{
    UpdateFrameCode(commandQueue, mLastUsedRenderFrameCode, mLastUsedCopyFrameCode);
}

void TrackedShaderResource::markAsUsed(ID3D12CommandList* commandList)
{
    UpdateFrameCode(commandList, mLastUsedRenderFrameCode, mLastUsedCopyFrameCode);
}
} // namespace scrap::d3d12