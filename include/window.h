#pragma once

#include <SDL_render.h>
#include <SDL_video.h>
#include <memory>
namespace windows {
    void run();
    void quit();
    std::shared_ptr<SDL_Window>& getWindow();
    std::shared_ptr<SDL_Renderer>& getRenderer();
    int getW();
    int getH();
    const char* getTitle();
}