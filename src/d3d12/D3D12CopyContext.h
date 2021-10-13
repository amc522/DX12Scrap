#pragma once

#include "d3d12/D3D12BaseCommandContext.h"
#include "d3d12/D3D12CommandList.h"
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
    CopyContext();

    CopyContext(const CopyContext&) = delete;
    CopyContext(CopyContext&&) = default;
    ~CopyContext() final = default;

    CopyContext& operator=(const CopyContext&) = delete;
    CopyContext& operator=(CopyContext&& other) = delete;

    HRESULT init();
    void releaseResources();

    ID3D12GraphicsCommandList* getCommandList() const { return mCommandList->get(); }

    void beginFrame() final;
    void endFrame() final;

    void execute();

private:
    std::unique_ptr<GraphicsCommandList> mCommandList;
};
} // namespace scrap::d3d12