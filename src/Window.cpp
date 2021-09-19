#include "Window.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <spdlog/spdlog.h>

namespace scrap
{
Window::Window(std::string_view title, glm::i32vec2 size)
{
    mSdlWindow = SDL_CreateWindow(title.data(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, size.x, size.y,
                                  SDL_WindowFlags::SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_HIDDEN);

    if(mSdlWindow == nullptr)
    {
        spdlog::critical("Failed to create window '{}' ({}, {})", title, size.x, size.y);
        return;
    }

    const int displayIndex = SDL_GetWindowDisplayIndex(mSdlWindow);
    float diagonalDpi, horizontalDpi, verticalDpi;
    const bool dpiSuccess = SDL_GetDisplayDPI(displayIndex, &diagonalDpi, &horizontalDpi, &verticalDpi) == 0;

    const int dpi = static_cast<int>(diagonalDpi);

    if(dpiSuccess && dpi != 96)
    {
        SDL_DestroyWindow(mSdlWindow);

        const float scale = dpi / 96.0f;
        glm::ivec2 dpiAwareSize{size.x * scale, size.y * scale};
        mSdlWindow = SDL_CreateWindow(title.data(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, dpiAwareSize.x,
                                      dpiAwareSize.y, SDL_WindowFlags::SDL_WINDOW_ALLOW_HIGHDPI);

        if(mSdlWindow == nullptr)
        {
            spdlog::critical("Failed to create high dpi aware window '{}'. Requested: ({}, {}), Hight Dpi: ({}, {})",
                             title, size.x, size.y, dpiAwareSize.x, dpiAwareSize.y);
            return;
        }

        SDL_GetWindowSize(mSdlWindow, &mDrawableSize.x, &mDrawableSize.y);
        mSize.x = static_cast<uint32_t>(mDrawableSize.x / scale);
        mSize.y = static_cast<uint32_t>(mDrawableSize.y / scale);
    }
    else
    {
        SDL_GetWindowSize(mSdlWindow, &mSize.x, &mSize.y);
        mDrawableSize = mSize;
        SDL_ShowWindow(mSdlWindow);
    }

    spdlog::info("Created new window '{}'. Size: ({}, {}) Drawable: ({}, {}). SDL id: {}, HWND: {}", title, mSize.x,
                 mSize.y, mDrawableSize.x, mDrawableSize.y, SDL_GetWindowID(mSdlWindow), (void*)getHwnd());
}

Window::~Window()
{
    if(mSdlWindow != nullptr)
    {
        spdlog::info("Destroying window");
        SDL_DestroyWindow(mSdlWindow);
    }
}

void Window::beginFrame()
{
    mKeyboard.beginFrame();
    mMouse.beginFrame();
}

void Window::setSize(glm::i32vec2 size)
{
    SDL_SetWindowSize(mSdlWindow, size.x, size.y);
}

HWND Window::getHwnd() const
{
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(mSdlWindow, &wmInfo);

    return wmInfo.info.win.window;
}

void Window::show()
{
    SDL_ShowWindow(mSdlWindow);
}

void Window::handleEvent(const SDL_KeyboardEvent& keyboardEvent)
{
    mKeyboard.handleEvent(keyboardEvent);
}

void Window::handleEvent(const SDL_MouseButtonEvent& mouseButtonEvent)
{
    mMouse.handleEvent(mouseButtonEvent);
}

void Window::handleEvent(const SDL_MouseMotionEvent& mouseMotionEvent)
{
    mMouse.handleEvent(mouseMotionEvent);
}

void Window::handleEvent(const SDL_MouseWheelEvent& mouseWheelEvent)
{
    mMouse.handleEvent(mouseWheelEvent);
}

void Window::handleEvent(const SDL_WindowEvent& windowEvent)
{
    switch(windowEvent.event)
    {
    case SDL_WINDOWEVENT_SHOWN: spdlog::debug("SDL_WINDOWEVENT_SHOWN: window {}", windowEvent.windowID); break;
    case SDL_WINDOWEVENT_HIDDEN: spdlog::debug("SDL_WINDOWEVENT_HIDDEN: window {}", windowEvent.windowID); break;
    case SDL_WINDOWEVENT_EXPOSED: spdlog::debug("SDL_WINDOWEVENT_EXPOSED: window {}", windowEvent.windowID); break;
    case SDL_WINDOWEVENT_MOVED:
        spdlog::debug("SDL_WINDOWEVENT_MOVED: window {} pos ({}, {})", windowEvent.windowID, windowEvent.data1,
                      windowEvent.data2);
        break;
    case SDL_WINDOWEVENT_RESIZED:
        spdlog::debug("SDL_WINDOWEVENT_RESIZED: window {} size({}, {})", windowEvent.windowID, windowEvent.data1,
                      windowEvent.data2);
        break;
    case SDL_WINDOWEVENT_SIZE_CHANGED:
        spdlog::debug("SDL_WINDOWEVENT_SIZE_CHANGED: window {} size({}, {})", windowEvent.windowID, windowEvent.data1,
                      windowEvent.data2);
        break;
    case SDL_WINDOWEVENT_MINIMIZED: spdlog::debug("SDL_WINDOWEVENT_MINIMIZED: window {}", windowEvent.windowID); break;
    case SDL_WINDOWEVENT_MAXIMIZED: spdlog::debug("SDL_WINDOWEVENT_MAXIMIZED: window {}", windowEvent.windowID); break;
    case SDL_WINDOWEVENT_RESTORED: spdlog::debug("SDL_WINDOWEVENT_RESTORED: window {}", windowEvent.windowID); break;
    case SDL_WINDOWEVENT_ENTER: spdlog::debug("SDL_WINDOWEVENT_ENTER: window {}", windowEvent.windowID); break;
    case SDL_WINDOWEVENT_LEAVE:
        spdlog::debug("SDL_WINDOWEVENT_LEAVE: window {}", windowEvent.windowID);
        mMouse.invalidatePosition();
        break;
    case SDL_WINDOWEVENT_FOCUS_GAINED:
        spdlog::debug("SDL_WINDOWEVENT_FOCUS_GAINED: window {}", windowEvent.windowID);
        break;
    case SDL_WINDOWEVENT_FOCUS_LOST:
        spdlog::debug("SDL_WINDOWEVENT_FOCUS_LOST: window {}", windowEvent.windowID);
        mMouse.invalidatePosition();
        break;
    case SDL_WINDOWEVENT_CLOSE: spdlog::debug("SDL_WINDOWEVENT_CLOSE: window {}", windowEvent.windowID); break;
    case SDL_WINDOWEVENT_TAKE_FOCUS:
        spdlog::debug("SDL_WINDOWEVENT_TAKE_FOCUS: window {}", windowEvent.windowID);
        break;
    case SDL_WINDOWEVENT_HIT_TEST: spdlog::debug("SDL_WINDOWEVENT_HIT_TEST: window {}", windowEvent.windowID); break;
    default:
        spdlog::debug("Unknown SDL_WindowEvent: event {}, window {}, data1 {}, data2 {}", windowEvent.event,
                      windowEvent.windowID, windowEvent.windowID, windowEvent.data1, windowEvent.data2);
        break;
    }
}

} // namespace scrap