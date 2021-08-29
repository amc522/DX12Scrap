#pragma once

#include "d3d12/D3D12BaseCommandContext.h"
#include "d3d12/D3D12Config.h"
#include "d3d12/D3D12FrameCodes.h"

#include <array>
#include <vector>

#include <wrl/client.h>

struct ID3D12CommandAllocator;
struct ID3D12GraphicsCommandList;
struct ID3D12CommandQueue;
struct ID3D12Fence;
struct ID3D12Resource;

namespace scrap::d3d12
{
class DeviceContext;

class CopyContext : public BaseCommandContext<CopyFrameCode>
{
public:
    CopyContext(): BaseCommandContext<CopyFrameCode>("Copy") {}

    CopyContext(const CopyContext&) = delete;
    CopyContext(CopyContext&&) = default;
    ~CopyContext() final = default;

    CopyContext& operator=(const CopyContext&) = delete;
    CopyContext& operator=(CopyContext&& other) = delete;

    HRESULT init();

    ID3D12GraphicsCommandList* getCommandList() const { return mCommandList.Get(); }

    void beginFrame() final;
    void endFrame() final;

private:
    std::array<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>, kFrameBufferCount> mCommandAllocators;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;
};
} // namespace scrap::d3d12