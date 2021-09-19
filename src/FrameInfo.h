#pragma once

#include <chrono>

namespace scrap
{
class Keyboard;
class Mouse;
class Window;

struct FrameInfo
{
    std::chrono::nanoseconds frameDelta{0};
    std::chrono::duration<float> frameDeltaSec{0.0f};
    Window* mainWindow = nullptr;
    Keyboard* keyboard = nullptr;
    Mouse* mouse = nullptr;
};
} // namespace scrap