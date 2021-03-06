#include "Application.h"

#include "FrameInfo.h"
#include "RenderScene.h"
#include "Window.h"
#include "d3d12/D3D12Context.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <fmt/chrono.h>
#include <spdlog/spdlog.h>

namespace scrap
{
Application::Application()
    : mApplicationStartTime(std::chrono::steady_clock::now())
{
    spdlog::info("Starting application");

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
    {
        spdlog::critical("Failed to initialize SDL '{}'", SDL_GetError());
        return;
    }

    SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);

    mMainWindow = std::make_unique<Window>("DX12Scrap", glm::i32vec2{1280, 720});

    if(!mMainWindow->initialized())
    {
        spdlog::critical("Failed to initialize main window.");
        return;
    }

    mD3D12Context = std::make_unique<d3d12::DeviceContext>(*mMainWindow, GpuPreference::None);

    if(!mD3D12Context->isInitialized())
    {
        spdlog::critical("Failed to initialize d3d12::DeviceContext.");
        return;
    }

    mRenderScene = std::make_unique<RenderScene>();

    if(!mRenderScene->isInitialized())
    {
        spdlog::critical("Failed to initialize RenderScene.");
        return;
    }

    mD3D12Context->getCopyContext().endFrame();
    mD3D12Context->getCopyContext().beginFrame();

    spdlog::info("Application initialized");
    mRunning = true;
}

Application::~Application()
{
    mRenderScene.reset();
    mD3D12Context.reset();
    if(SDL_WasInit(0) != 0) { SDL_Quit(); }
}

Application::operator bool() const
{
    return mRunning;
}

void Application::update()
{
    auto now = std::chrono::steady_clock::now();
    mFrameDelta = now - mLastFrameTime;
    mLastFrameTime = now;

    mKeyboard.beginFrame();
    mMouse.beginFrame();

    mMainWindow->beginFrame();

    FrameInfo frameInfo;
    frameInfo.runtime = now - mApplicationStartTime;
    frameInfo.runtimeSec = std::chrono::duration_cast<std::chrono::duration<float>>(frameInfo.runtime);
    frameInfo.frameDelta = mFrameDelta;
    frameInfo.frameDeltaSec = std::chrono::duration_cast<std::chrono::duration<float>>(mFrameDelta);
    frameInfo.mainWindow = mMainWindow.get();
    frameInfo.keyboard = &mKeyboard;
    frameInfo.mouse = &mMouse;

    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
        case SDL_QUIT:
            spdlog::debug("Event SDL_QUIT");
            mRunning = false;
            break;
        case SDL_APP_TERMINATING: spdlog::debug("Event SDL_APP_TERMINATING"); break;
        case SDL_APP_LOWMEMORY: spdlog::debug("Event SDL_APP_LOWMEMORY"); break;
        case SDL_APP_WILLENTERBACKGROUND: spdlog::debug("Event SDL_APP_WILLENTERBACKGROUND"); break;
        case SDL_APP_DIDENTERBACKGROUND: spdlog::debug("Event SDL_APP_DIDENTERBACKGROUND"); break;
        case SDL_APP_WILLENTERFOREGROUND: spdlog::debug("Event SDL_APP_WILLENTERFOREGROUND"); break;
        case SDL_APP_DIDENTERFOREGROUND: spdlog::debug("Event SDL_APP_DIDENTERFOREGROUND"); break;
        case SDL_LOCALECHANGED: spdlog::debug("Event SDL_LOCALECHANGED"); break;
        case SDL_DISPLAYEVENT: spdlog::debug("Event SDL_DISPLAYEVENT"); break;
        case SDL_WINDOWEVENT:
            if(event.window.windowID == SDL_GetWindowID(mMainWindow->sdlWindow()))
            {
                mMainWindow->handleEvent(event.window);
            }
            else
            {
                spdlog::debug("Event SDL_WINDOWEVENT. WindowEventId {}, data1 {}, data2 {}", event.window.event,
                              event.window.data1, event.window.data2);
            }
            break;
        case SDL_SYSWMEVENT:
            if(event.syswm.msg->msg.win.msg == WM_DPICHANGED)
            {
                // these both should always be the same
                const DWORD dpiX = LOWORD(event.syswm.msg->msg.win.wParam);
                const DWORD dpiY = HIWORD(event.syswm.msg->msg.win.wParam);
                const HWND windowHandle = event.syswm.msg->msg.win.hwnd;

                spdlog::debug("Event SDL_SYSWMEVENT. WM_DPICHANGED: dpiX {}, dpiY {}, HWND {}", dpiX, dpiY,
                              (void*)windowHandle);

                // TODO: set the new window size based on the dpi
                // TODO: remake the swap chain
                // TODO: When the other two TODOs are handled, make this project per monitor high dpi aware
                break;
            }
            break;
        case SDL_KEYDOWN:
            mKeyboard.handleEvent(event.key);
            if(event.key.windowID == SDL_GetWindowID(mMainWindow->sdlWindow())) { mMainWindow->handleEvent(event.key); }
            break;
        case SDL_KEYUP:
            mKeyboard.handleEvent(event.key);
            if(event.key.windowID == SDL_GetWindowID(mMainWindow->sdlWindow())) { mMainWindow->handleEvent(event.key); }
            break;
        case SDL_TEXTEDITING: spdlog::debug("Event SDL_TEXTEDITING"); break;
        case SDL_TEXTINPUT: spdlog::debug("Event SDL_TEXTINPUT"); break;
        case SDL_KEYMAPCHANGED: spdlog::debug("Event SDL_KEYMAPCHANGED"); break;
        case SDL_MOUSEMOTION:
            spdlog::debug("Event SDL_MOUSEMOTION");
            mMouse.handleEvent(event.motion);
            if(event.motion.windowID == SDL_GetWindowID(mMainWindow->sdlWindow()))
            {
                mMainWindow->handleEvent(event.motion);
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            spdlog::debug("Event SDL_MOUSEBUTTONDOWN");
            mMouse.handleEvent(event.button);
            if(event.button.windowID == SDL_GetWindowID(mMainWindow->sdlWindow()))
            {
                mMainWindow->handleEvent(event.button);
            }
            break;
        case SDL_MOUSEBUTTONUP:
            spdlog::debug("Event SDL_MOUSEBUTTONUP");
            mMouse.handleEvent(event.button);
            if(event.button.windowID == SDL_GetWindowID(mMainWindow->sdlWindow()))
            {
                mMainWindow->handleEvent(event.button);
            }
            break;
        case SDL_MOUSEWHEEL:
            spdlog::debug("Event SDL_MOUSEWHEEL");
            mMouse.handleEvent(event.wheel);
            if(event.wheel.windowID == SDL_GetWindowID(mMainWindow->sdlWindow()))
            {
                mMainWindow->handleEvent(event.wheel);
            }
            break;
        case SDL_JOYAXISMOTION: spdlog::debug("Event SDL_JOYAXISMOTION"); break;
        case SDL_JOYBALLMOTION: spdlog::debug("Event SDL_JOYBALLMOTION"); break;
        case SDL_JOYHATMOTION: spdlog::debug("Event SDL_JOYHATMOTION"); break;
        case SDL_JOYBUTTONDOWN: spdlog::debug("Event SDL_JOYBUTTONDOWN"); break;
        case SDL_JOYBUTTONUP: spdlog::debug("Event SDL_JOYBUTTONUP"); break;
        case SDL_JOYDEVICEADDED: spdlog::debug("Event SDL_JOYDEVICEADDED"); break;
        case SDL_JOYDEVICEREMOVED: spdlog::debug("Event SDL_JOYDEVICEREMOVED"); break;
        case SDL_CONTROLLERAXISMOTION: spdlog::debug("Event SDL_CONTROLLERAXISMOTION"); break;
        case SDL_CONTROLLERBUTTONDOWN: spdlog::debug("Event SDL_CONTROLLERBUTTONDOWN"); break;
        case SDL_CONTROLLERBUTTONUP: spdlog::debug("Event SDL_CONTROLLERBUTTONUP"); break;
        case SDL_CONTROLLERDEVICEADDED: spdlog::debug("Event SDL_CONTROLLERDEVICEADDED"); break;
        case SDL_CONTROLLERDEVICEREMOVED: spdlog::debug("Event SDL_CONTROLLERDEVICEREMOVED"); break;
        case SDL_CONTROLLERDEVICEREMAPPED: spdlog::debug("Event SDL_CONTROLLERDEVICEREMAPPED"); break;
        case SDL_CONTROLLERTOUCHPADDOWN: spdlog::debug("Event SDL_CONTROLLERTOUCHPADDOWN"); break;
        case SDL_CONTROLLERTOUCHPADMOTION: spdlog::debug("Event SDL_CONTROLLERTOUCHPADMOTION"); break;
        case SDL_CONTROLLERTOUCHPADUP: spdlog::debug("Event SDL_CONTROLLERTOUCHPADUP"); break;
        case SDL_CONTROLLERSENSORUPDATE: spdlog::debug("Event SDL_CONTROLLERSENSORUPDATE"); break;
        case SDL_FINGERDOWN: spdlog::debug("Event SDL_FINGERDOWN"); break;
        case SDL_FINGERUP: spdlog::debug("Event SDL_FINGERUP"); break;
        case SDL_FINGERMOTION: spdlog::debug("Event SDL_FINGERMOTION"); break;
        case SDL_DOLLARGESTURE: spdlog::debug("Event SDL_DOLLARGESTURE"); break;
        case SDL_DOLLARRECORD: spdlog::debug("Event SDL_DOLLARRECORD"); break;
        case SDL_MULTIGESTURE: spdlog::debug("Event SDL_MULTIGESTURE"); break;
        case SDL_CLIPBOARDUPDATE: spdlog::debug("Event SDL_CLIPBOARDUPDATE"); break;
        case SDL_DROPFILE: spdlog::debug("Event SDL_DROPFILE"); break;
        case SDL_DROPTEXT: spdlog::debug("Event SDL_DROPTEXT"); break;
        case SDL_DROPBEGIN: spdlog::debug("Event SDL_DROPBEGIN"); break;
        case SDL_DROPCOMPLETE: spdlog::debug("Event SDL_DROPCOMPLETE"); break;
        case SDL_AUDIODEVICEADDED: spdlog::debug("Event SDL_AUDIODEVICEADDED"); break;
        case SDL_AUDIODEVICEREMOVED: spdlog::debug("Event SDL_AUDIODEVICEREMOVED"); break;
        case SDL_SENSORUPDATE: spdlog::debug("Event SDL_SENSORUPDATE"); break;
        case SDL_RENDER_TARGETS_RESET: spdlog::debug("Event SDL_RENDER_TARGETS_RESET"); break;
        case SDL_RENDER_DEVICE_RESET: spdlog::debug("Event SDL_RENDER_DEVICE_RESET"); break;
        case SDL_USEREVENT: spdlog::debug("Event SDL_USEREVENT"); break;
        default: spdlog::debug("Unknown SDL event: {0} ({0:#x})", event.type);
        }
    }

    mRenderScene->preRender(frameInfo);
    mD3D12Context->beginFrame();
    mRenderScene->render(frameInfo, *mD3D12Context);
    mRenderScene->endFrame();
    mD3D12Context->endFrame();
}

} // namespace scrap