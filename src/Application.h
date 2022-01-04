// Application is the glue for the whole program and there should only be one instance of Application for the program.
// It collects all the high level objects into one place. It's responsible for handling the system events and passing
// them along where appropriate. Additionally, Application calls update related fuctions for all the objects that it
// holds (that need it).
//
// The event handling is routed by SDL and then handled as needed here.

#pragma once

#include "Keyboard.h"
#include "Mouse.h"

#include <chrono>
#include <memory>

namespace scrap
{
namespace d3d12
{
class DeviceContext;
}

class RenderScene;
class Window;

class Application
{
public:
    Application();
    ~Application();

    operator bool() const;

    void update();

private:
    std::unique_ptr<Window> mMainWindow;
    Keyboard mKeyboard;
    Mouse mMouse;
    std::unique_ptr<d3d12::DeviceContext> mD3D12Context;
    std::unique_ptr<RenderScene> mRenderScene;
    bool mRunning = false;

    std::chrono::steady_clock::time_point mApplicationStartTime;
    std::chrono::steady_clock::time_point mLastFrameTime;
    std::chrono::nanoseconds mFrameDelta{0};
};

} // namespace scrap