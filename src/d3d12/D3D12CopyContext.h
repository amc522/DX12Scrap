#pragma once

#include "d3d12/D3D12BaseCommandContext.h"
#include "d3d12/D3D12CommandList.h"
#include "d3d12/D3D12Config.h"
#include "d3d12/D3D12FrameCodes.h"
#include "d3d12/D3D12Fwd.h"

#include <array>
#include <vector>

#include <wrl/client.h>

namespace scrap::d3d12
{
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