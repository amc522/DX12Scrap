#pragma once

#include <array>

#include <SDL2/SDL_mouse.h>
#include <glm/vec2.hpp>

union SDL_Event;

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
    void handleEvent(const SDL_Event& sdlEvent);

    glm::ivec2 getPosition() const { return mPosition; }
    glm::ivec2 getDelta() const { return mPosition - mLastPosition; }
    float getDistanceTravelled() const { return mDistance; }

private:
    std::array<MouseButtonState, MouseButton::Count> mMouseButtonStates{MouseButton{Left}, MouseButton{Right},
                                                                        MouseButton{Middle}};
    glm::ivec2 mPosition;
    glm::ivec2 mLastPosition;
    float mDistance = 0.0f;
};
} // namespace scrap