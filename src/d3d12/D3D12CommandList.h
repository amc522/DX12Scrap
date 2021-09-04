#pragma once

#include "d3d12/D3D12Config.h"
#include "d3d12/D3D12TrackedGpuObject.h"

#include <array>

#include <wrl/client.h>

enum D3D12_COMMAND_LIST_TYPE;
struct ID3D12CommandAllocator;
struct ID3D12GraphicsCommandList;
struct ID3D12CommandQueue;

namespace scrap::d3d12
{
class GraphicsCommandList
{
public:
    GraphicsCommandList(D3D12_COMMAND_LIST_TYPE type, std::string_view debugName);
    GraphicsCommandList(const GraphicsCommandList&) = delete;
    GraphicsCommandList(GraphicsCommandList&&) = default;
    ~GraphicsCommandList() = default;

    GraphicsCommandList& operator=(const GraphicsCommandList&) = delete;
    GraphicsCommandList& operator=(GraphicsCommandList&&) = default;

    ID3D12GraphicsCommandList* get() const { return mCommandList.Get(); }

    HRESULT reset();
    HRESULT close();

    void markAsUsed(ID3D12CommandQueue* commandQueue) { mCommandAllocators[mFrameIndex].markAsUsed(commandQueue); }

private:
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;
    std::array<TrackedGpuObject<ID3D12CommandAllocator>, kFrameBufferCount> mCommandAllocators;
    uint32_t mFrameIndex = 0;
};
} // namespace scrap::d3d12