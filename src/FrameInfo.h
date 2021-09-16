#pragma once

#include <chrono>

namespace scrap
{
class Keyboard;
class Mouse;

struct FrameInfo
{
    std::chrono::nanoseconds frameDelta{0};
    std::chrono::duration<float> frameDeltaSec{0.0f};
    Keyboard* keyboard = nullptr;
    Mouse* mouse = nullptr;
    glm::ivec2 windowSize;
};
} // namespace scrap