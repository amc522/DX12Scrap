#pragma once

#include <wrl/client.h>

struct IDXGIAdapter4;

namespace scrap
{
class D3D12Context
{
public:
    D3D12Context();
    ~D3D12Context();

    operator bool() const { return mInitialized; }

private:
    Microsoft::WRL::ComPtr<IDXGIAdapter4> mAdapter;
    bool mInitialized = false;
};

} // namespace scrap
