#include "Application.h"

#include "d3d12/D3D12Context.h"
#include "RenderScene.h"

#include <SDL2/SDL.h>
#include <spdlog/spdlog.h>

namespace scrap
{
Application::Application()
{
    spdlog::info("Starting application");

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
    {
        spdlog::critical("Failed to initialize SDL '{}'", SDL_GetError());
        return;
    }

    mMainWindow = std::make_unique<Window>("DX12Scrap", glm::i32vec2{1280, 720});

    if(!mMainWindow->initialized()) { return; }

    mD3D12Context = std::make_unique<D3D12Context>(*mMainWindow, GpuPreference::None);

    if(!mD3D12Context->initialized()) { return; }

    mRenderScene = std::make_unique<RenderScene>(*mD3D12Context);

    if(!mRenderScene->initialized()) { return; }

    spdlog::info("Application initialized");
    mRunning = true;
}

Application::~Application()
{
    if(SDL_WasInit(0) != 0) { SDL_Quit(); }
}

Application::operator bool() const { return mRunning; }

void Application::update()
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
        case SDL_QUIT: 
            spdlog::info("Shutting down application");
            mRunning = false; break;
        }
    }
}

} // namespace scrap