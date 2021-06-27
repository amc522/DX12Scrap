// Window encapsulates the functionality of an OS window. This includes creating/destroying the window, resizing, and
// reporting the status of the window. All window events from Application are routed to the approriate Window instance.
// There can be as many Window instances as desired.

#pragma once

#include <string_view>

#include <Windows.h>
#include <glm/vec2.hpp>

struct SDL_Window;
struct SDL_WindowEvent;

namespace scrap
{
class Window
{
public:
    Window(std::string_view title, glm::i32vec2 size);
    ~Window();

    bool initialized() const { return mSdlWindow != nullptr; }

    void setSize(glm::i32vec2 size);
    glm::i32vec2 getSize() const { return mSize; }
    glm::i32vec2 getDrawableSize() const { return mDrawableSize; }

    HWND getHwnd() const;
    SDL_Window* sdlWindow() const { return mSdlWindow; }

    void show();

    void handleEvent(const SDL_WindowEvent& windowEvent);

private:
    SDL_Window* mSdlWindow = nullptr;

    // This is the window size that was requested. This may not be the actual pixel size because of hight dpi scaling.
    glm::i32vec2 mSize{0, 0};

    // This is the actual pixel size of the window content area. This may be different than the requested window size.
    glm::i32vec2 mDrawableSize{0, 0};
};

} // namespace scrap