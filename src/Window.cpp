#include "Window.h"

#include <SDL2/SDL.h>
namespace scrap
{
Window::Window(std::string_view title, glm::i32vec2 size)
{
    mSdlWindow = SDL_CreateWindow(title.data(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, size.x, size.y,
                                  SDL_WindowFlags::SDL_WINDOW_ALLOW_HIGHDPI);
    if(mSdlWindow == nullptr)
    {
        SDL_Log("Failed to create window '%s' (%d, %d)", title.data(), size.x, size.y);
        return;
    }
}

Window::~Window()
{
    if(mSdlWindow != nullptr) { SDL_DestroyWindow(mSdlWindow); }
}

Window::operator bool() const { return mSdlWindow != nullptr; }

void Window::setSize(glm::i32vec2 size) { SDL_SetWindowSize(mSdlWindow, size.x, size.y); }

void Window::show() { SDL_ShowWindow(mSdlWindow); }

} // namespace scrap