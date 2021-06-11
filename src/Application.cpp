#include "Application.h"

#include <SDL2/SDL.h>
#include <spdlog/spdlog.h>

namespace scrap
{
Application::Application()
{
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
    {
        spdlog::critical("Failed to initialize SDL '{}'", SDL_GetError());
        return;
    }

    mMainWindow = std::make_unique<Window>("DX12Scrap", glm::i32vec2{1280, 720});

    if(!mMainWindow) { return; }

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
        case SDL_QUIT: mRunning = false; break;
        }
    }
}

} // namespace scrap