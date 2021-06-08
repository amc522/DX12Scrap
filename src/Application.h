#pragma once

#include "Window.h"

#include <SDL2/SDL_video.h>
#include <memory>

namespace scrap
{
class Application
{
  public:
    Application();
    ~Application();

    operator bool() const;

    void update();

  private:
    std::unique_ptr<Window> mMainWindow;
    bool mRunning = false;
};

} // namespace scrap