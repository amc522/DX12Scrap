#pragma once

#include <array>
#include <unordered_map>

#include <SDL2/SDL_keyboard.h>

struct SDL_KeyboardEvent;

namespace scrap
{
struct KeyState
{
    SDL_Keycode keyCode;
    int pressedCount = 0;
    int releasedCount = 0;
    bool down = false;
    bool repeat = false;
};

class Keyboard
{
public:
    void beginFrame();
    void handleEvent(const SDL_KeyboardEvent& sdlEvent);

    const KeyState& getKeyState(SDL_Keycode keyCode) const;

private:
    std::unordered_map<SDL_Keycode, KeyState> mKeyStates;
};
} // namespace scrap