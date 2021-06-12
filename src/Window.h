#pragma once

#include <Windows.h>
#include <glm/vec2.hpp>
#include <string_view>

struct SDL_Window;

namespace scrap
{
class Window
{
public:
    Window(std::string_view title, glm::i32vec2 size);
    ~Window();

    operator bool() const;

    void setSize(glm::i32vec2 size);
    glm::i32vec2 getSize() const;

    HWND getHwnd() const;

    void show();

private:
    SDL_Window* mSdlWindow = nullptr;
};

} // namespace scrap