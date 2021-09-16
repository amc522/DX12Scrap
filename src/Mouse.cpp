#include "Mouse.h"

#include <SDL2/SDL_events.h>
#include <glm/glm.hpp>

namespace scrap
{
MouseButton TranslateSdlMouseButton(Uint8 button)
{
    switch(button)
    {
    case SDL_BUTTON_LEFT: return MouseButton::Left;
    case SDL_BUTTON_RIGHT: return MouseButton::Right;
    case SDL_BUTTON_MIDDLE: return MouseButton::Middle;
    default: return Unknown;
    }
}

void Mouse::beginFrame()
{
    for(MouseButtonState& state : mMouseButtonStates)
    {
        state.clicks = 0;
        state.pressed = 0;
        state.released = 0;
    }

    mDistance = 0.0f;

    mLastPosition = mPosition;
}

void Mouse::handleEvent(const SDL_Event& sdlEvent)
{
    MouseButtonState* mouseButtonState = nullptr;

    switch(sdlEvent.type)
    {
    case SDL_MOUSEBUTTONDOWN:
        mouseButtonState = &mMouseButtonStates[TranslateSdlMouseButton(sdlEvent.button.button)];
        ++mouseButtonState->pressed;
        mouseButtonState->clicks = sdlEvent.button.clicks;
        break;
    case SDL_MOUSEBUTTONUP:
        mouseButtonState = &mMouseButtonStates[TranslateSdlMouseButton(sdlEvent.button.button)];
        ++mouseButtonState->released;
        mouseButtonState->clicks = sdlEvent.button.clicks;
        break;
    case SDL_MOUSEMOTION:
        mPosition.x = sdlEvent.motion.x;
        mPosition.y = sdlEvent.motion.y;
        mDistance += glm::length(glm::vec2(sdlEvent.motion.xrel, sdlEvent.motion.yrel));
        break;
    case SDL_MOUSEWHEEL: break;
    }
}
} // namespace scrap