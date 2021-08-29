#pragma once

#include "d3d12/D3D12BaseCommandContext.h"
#include "d3d12/D3D12FrameCodes.h"

namespace scrap::d3d12
{
class GraphicsContext : public BaseCommandContext<RenderFrameCode>
{
public:
    GraphicsContext();

    GraphicsContext(const GraphicsContext&) = delete;
    GraphicsContext(GraphicsContext&&) = default;
    virtual ~GraphicsContext() final = default;

    GraphicsContext& operator=(const GraphicsContext&) = delete;
    GraphicsContext& operator=(GraphicsContext&&) = default;

    HRESULT init();

private:
};
} // namespace scrap::d3d12