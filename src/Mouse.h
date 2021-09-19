#pragma once

#include <array>

#include <SDL2/SDL_mouse.h>
#include <glm/vec2.hpp>

struct SDL_MouseButtonEvent;
struct SDL_MouseMotionEvent;
struct SDL_MouseWheelEvent;

namespace scrap
{
enum class MouseButton
{
    Left,
    Right,
    Middle,
    Count,
    Unknown
};

struct MouseButtonState
{
    MouseButton button = MouseButton::Unknown;
    int pressedCount = 0;
    int releasedCount = 0;
    int clicks = 0;
    bool down = false;
};

class Mouse
{
public:
    void beginFrame();
    void handleEvent(const SDL_MouseButtonEvent& sdlEvent);
    void handleEvent(const SDL_MouseMotionEvent& sdlEvent);
    void handleEvent(const SDL_MouseWheelEvent& sdlEvent);

    glm::ivec2 getPosition() const { return mPosition; }
    glm::ivec2 getDelta() const { return mPosition - mLastPosition; }
    float getDistanceTravelled() const { return mDistance; }

    const MouseButtonState& getButtonState(MouseButton button) const { return mMouseButtonStates[(size_t)button]; }
    bool isButtonDown(MouseButton button) const { return mMouseButtonStates[(size_t)button].down; }

    void invalidatePosition() { mLastPositionSet = false; }

private:
    std::array<MouseButtonState, (size_t)MouseButton::Count> mMouseButtonStates{MouseButtonState{MouseButton::Left},
                                                                                MouseButtonState{MouseButton::Right},
                                                                                MouseButtonState{MouseButton::Middle}};
    glm::ivec2 mPosition;
    glm::ivec2 mLastPosition;
    float mDistance = 0.0f;
    bool mLastPositionSet = false;
};
} // namespace scrap