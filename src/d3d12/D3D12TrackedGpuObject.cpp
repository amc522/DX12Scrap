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

bool IsInUse(D3D12_COMMAND_LIST_TYPE commandListType, RenderFrameCode renderFrameCode, CopyFrameCode copyFrameCode)
{
    switch(commandListType)
    {
    case D3D12_COMMAND_LIST_TYPE_DIRECT:
        return renderFrameCode > DeviceContext::instance().getGraphicsContext().getLastCompletedFrameCode();
    case D3D12_COMMAND_LIST_TYPE_COPY:
        return copyFrameCode > DeviceContext::instance().getCopyContext().getLastCompletedFrameCode();
    default:
        assert(false);
        spdlog::critical("TrackedDeviceChild::isInUse unsupported D3D12_COMMAND_LIST_TYPE '{}'", commandListType);
        return false;
    }
}

bool TrackedDeviceChild::isInUse(ID3D12CommandQueue* commandQueue) const
{
    return IsInUse(commandQueue->GetDesc().Type, mLastUsedRenderFrameCode, mLastUsedCopyFrameCode);
}

bool TrackedDeviceChild::isInUse(ID3D12CommandList* commandList) const
{
    return IsInUse(commandList->GetType(), mLastUsedRenderFrameCode, mLastUsedCopyFrameCode);
}

void TrackedShaderResource::destroy()
{
    if(mResource == nullptr) { return; }

    uint32_t validDescriptorReservationCount = 0;
    validDescriptorReservationCount += (mCbvSrvUavDescriptorHeapReservation.isValid()) ? 1 : 0;
    validDescriptorReservationCount += (mCbvSrvUavDescriptorHeapReservation.isValid()) ? 1 : 0;
    validDescriptorReservationCount += (mCbvSrvUavDescriptorHeapReservation.isValid()) ? 1 : 0;

    DeviceContext& deviceContext = DeviceContext::instance();

    if(validDescriptorReservationCount > 0)
    {
        std::array<FixedDescriptorHeapReservation, 3> descriptorHeapReservations = {
            std::move(mCbvSrvUavDescriptorHeapReservation), std::move(mRtvDescriptorHeapReservation),
            std::move(mDsvDescriptorHeapReservation)};

        deviceContext.getGraphicsContext().queueObjectForDestruction(mResource, descriptorHeapReservations,
                                                                     mLastUsedRenderFrameCode);
    }
    else
    {
        deviceContext.getGraphicsContext().queueObjectForDestruction(mResource, mLastUsedRenderFrameCode);
    }
}

void TrackedShaderResource::setCbvSrvUavDescriptorHeapReservation(
    FixedDescriptorHeapReservation&& descriptorHeapReservation)
{
    if(mCbvSrvUavDescriptorHeapReservation.isValid())
    {
        DeviceContext::instance().getGraphicsContext().queueObjectForDestruction(
            std::move(mCbvSrvUavDescriptorHeapReservation), mLastUsedRenderFrameCode);
    }

    mCbvSrvUavDescriptorHeapReservation = std::move(descriptorHeapReservation);
}

void TrackedShaderResource::setRtvDescriptorHeapReservation(FixedDescriptorHeapReservation&& descriptorHeapReservation)
{
    if(mRtvDescriptorHeapReservation.isValid())
    {
        DeviceContext::instance().getGraphicsContext().queueObjectForDestruction(
            std::move(mRtvDescriptorHeapReservation), mLastUsedRenderFrameCode);
    }

    mRtvDescriptorHeapReservation = std::move(descriptorHeapReservation);
}

void TrackedShaderResource::setDsvDescriptorHeapReservation(FixedDescriptorHeapReservation&& descriptorHeapReservation)
{
    if(mDsvDescriptorHeapReservation.isValid())
    {
        DeviceContext::instance().getGraphicsContext().queueObjectForDestruction(
            std::move(mDsvDescriptorHeapReservation), mLastUsedRenderFrameCode);
    }

    mDsvDescriptorHeapReservation = std::move(descriptorHeapReservation);
}

void TrackedShaderResource::markAsUsed(ID3D12CommandQueue* commandQueue)
{
    UpdateFrameCode(commandQueue, mLastUsedRenderFrameCode, mLastUsedCopyFrameCode);
}

void TrackedShaderResource::markAsUsed(ID3D12CommandList* commandList)
{
    UpdateFrameCode(commandList, mLastUsedRenderFrameCode, mLastUsedCopyFrameCode);
}

bool TrackedShaderResource::isInUse(ID3D12CommandQueue* commandQueue) const
{
    return IsInUse(commandQueue->GetDesc().Type, mLastUsedRenderFrameCode, mLastUsedCopyFrameCode);
}

bool TrackedShaderResource::isInUse(ID3D12CommandList* commandList) const
{
    return IsInUse(commandList->GetType(), mLastUsedRenderFrameCode, mLastUsedCopyFrameCode);
}
} // namespace scrap::d3d12