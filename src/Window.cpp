#include "Window.h"

#include <SDL2/SDL.h>
#include <spdlog/spdlog.h>

namespace scrap
{
Window::Window(std::string_view title, glm::i32vec2 size)
{
    mSdlWindow = SDL_CreateWindow(title.data(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, size.x, size.y,
                                  SDL_WindowFlags::SDL_WINDOW_ALLOW_HIGHDPI);
    if(mSdlWindow == nullptr)
    {
        spdlog::critical("Failed to create window '{}' ({}, {})", title, size.x, size.y);
        return;
    }

    spdlog::info("Created new window '{}' ({}, {})", title, size.x, size.y);
}

Window::~Window()
{
    if(mSdlWindow != nullptr)
    {
        spdlog::info("Destroying window");
        SDL_DestroyWindow(mSdlWindow);
    }
}

Window::operator bool() const { return mSdlWindow != nullptr; }

void Window::setSize(glm::i32vec2 size) { SDL_SetWindowSize(mSdlWindow, size.x, size.y); }

void Window::show() { SDL_ShowWindow(mSdlWindow); }

} // namespace scrap