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
    default: return MouseButton::Unknown;
    }
}

void Mouse::beginFrame()
{
    for(MouseButtonState& state : mMouseButtonStates)
    {
        state.clicks = 0;
        state.pressedCount = 0;
        state.releasedCount = 0;
    }

    mDistance = 0.0f;

    mLastPosition = mPosition;
}

void Mouse::handleEvent(const SDL_MouseButtonEvent& mouseButtonEvent)
{
    MouseButtonState* mouseButtonState = nullptr;

    switch(mouseButtonEvent.type)
    {
    case SDL_MOUSEBUTTONDOWN:
        mouseButtonState = &mMouseButtonStates[(size_t)TranslateSdlMouseButton(mouseButtonEvent.button)];
        ++mouseButtonState->pressedCount;
        mouseButtonState->clicks = mouseButtonEvent.clicks;
        mouseButtonState->down = true;
        break;
    case SDL_MOUSEBUTTONUP:
        mouseButtonState = &mMouseButtonStates[(size_t)TranslateSdlMouseButton(mouseButtonEvent.button)];
        ++mouseButtonState->releasedCount;
        mouseButtonState->clicks = mouseButtonEvent.clicks;
        mouseButtonState->down = false;
        break;

    case SDL_MOUSEWHEEL: break;
    }
}

void Mouse::handleEvent(const SDL_MouseMotionEvent& mouseMotionEvent)
{
    switch(mouseMotionEvent.type)
    {
    case SDL_MOUSEMOTION:
        mPosition.x = mouseMotionEvent.x;
        mPosition.y = mouseMotionEvent.y;
        if(!mLastPositionSet)
        {
            mLastPosition = mPosition;
            mLastPositionSet = true;
        }
        mDistance += glm::length(glm::vec2(mouseMotionEvent.xrel, mouseMotionEvent.yrel));
        break;
    }
}

void Mouse::handleEvent(const SDL_MouseWheelEvent& sdlEvent) {}
} // namespace scrap