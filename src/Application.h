#pragma once

#include "Window.h"

#include <SDL2/SDL_video.h>
#include <memory>

namespace scrap
{
class D3D12Context;

class Application
{
public:
    Application();
    ~Application();

    operator bool() const;

    void update();

private:
    std::unique_ptr<Window> mMainWindow;
    std::unique_ptr<D3D12Context> mD3D12Context;
    bool mRunning = false;
};

} // namespace scrap