#pragma once

#include <array>

#include <SDL2/SDL_mouse.h>
#include <glm/vec2.hpp>

struct SDL_MouseButtonEvent;
struct SDL_MouseMotionEvent;
struct SDL_MouseWheelEvent;

namespace scrap
{
enum MouseButton
{
    Left,
    Right,
    Middle,
    Count,
    Unknown
};

struct MouseButtonState
{
    MouseButton button = Unknown;
    int pressed = 0;
    int released = 0;
    int clicks = 0;
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

    void invalidatePosition() { mLastPositionSet = false; }

private:
    std::array<MouseButtonState, MouseButton::Count> mMouseButtonStates{MouseButtonState{Left}, MouseButtonState{Right},
                                                                        MouseButtonState{Middle}};
    glm::ivec2 mPosition;
    glm::ivec2 mLastPosition;
    float mDistance = 0.0f;
    bool mLastPositionSet = false;
};
} // namespace scrap