#include "Keyboard.h"

#include <SDL2/SDL_events.h>

namespace scrap
{
void Keyboard::beginFrame()
{
    for(auto [key, keyState] : mKeyStates)
    {
        keyState.pressed = 0;
        keyState.released = 0;
        keyState.repeat = 0;
    }
}

void Keyboard::handleEvent(const SDL_Event& sdlEvent)
{
    KeyState* keyState;

    switch(sdlEvent.type)
    {
    case SDL_KEYDOWN:
        keyState = &mKeyStates[sdlEvent.key.keysym.sym];
        keyState->keyCode = sdlEvent.key.keysym.sym;
        ++keyState->pressed;
        keyState->repeat = sdlEvent.key.repeat;
        break;
    case SDL_KEYUP:
        keyState = &mKeyStates[sdlEvent.key.keysym.sym];
        keyState->keyCode = sdlEvent.key.keysym.sym;
        ++keyState->released;
        keyState->repeat = sdlEvent.key.repeat;
        break;
    }
}

const KeyState& Keyboard::getKeyState(SDL_Keycode keyCode) const
{
    static constexpr KeyState emptyKeyState{};
    auto itr = mKeyStates.find(keyCode);
    if(itr == mKeyStates.cend()) { return emptyKeyState; }

    return itr->second;
}
} // namespace scrap