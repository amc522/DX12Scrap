#pragma once

#include <array>
#include <unordered_map>

#include <SDL2/SDL_keyboard.h>

union SDL_Event;

namespace scrap
{
struct KeyState
{
    SDL_Keycode keyCode;
    int repeat = 0;
    int pressed = 0;
    int released = 0;
};

class Keyboard
{
public:
    void beginFrame();
    void handleEvent(const SDL_Event& sdlEvent);

    const KeyState& getKeyState(SDL_Keycode keyCode) const;

private:
    std::unordered_map<SDL_Keycode, KeyState> mKeyStates;
};
} // namespace scrap