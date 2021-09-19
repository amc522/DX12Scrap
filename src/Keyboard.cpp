#include "Keyboard.h"

#include <SDL2/SDL_events.h>
#include <spdlog/spdlog.h>

namespace scrap
{
void Keyboard::beginFrame()
{
    for(auto& [key, keyState] : mKeyStates)
    {
        keyState.pressedCount = 0;
        keyState.releasedCount = 0;
    }
}

void Keyboard::handleEvent(const SDL_KeyboardEvent& keyboardEvent)
{
    KeyState* keyState;

    switch(keyboardEvent.type)
    {
    case SDL_KEYDOWN:
        spdlog::debug("Event SDL_KEYDOWN: key={}, state={}, repeat={}", keyboardEvent.keysym.sym, keyboardEvent.state,
                      keyboardEvent.repeat);
        keyState = &mKeyStates[keyboardEvent.keysym.sym];
        keyState->keyCode = keyboardEvent.keysym.sym;
        ++keyState->pressedCount;
        keyState->down = true;
        keyState->repeat |= keyboardEvent.repeat > 0;
        break;
    case SDL_KEYUP:
        spdlog::debug("Event SDL_KEYUP: key={}, state={}, repeat={}", keyboardEvent.keysym.sym, keyboardEvent.state,
                      keyboardEvent.repeat);
        keyState = &mKeyStates[keyboardEvent.keysym.sym];
        keyState->keyCode = keyboardEvent.keysym.sym;
        ++keyState->releasedCount;
        keyState->down = false;
        keyState->repeat |= keyboardEvent.repeat > 0;
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