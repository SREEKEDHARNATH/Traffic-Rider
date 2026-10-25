#pragma once

#include <SDL_render.h>
#include <SDL_video.h>
#include <memory>
namespace window {
    void run();
    void quit();
    std::shared_ptr<SDL_Window>& getWindow();
    std::shared_ptr<SDL_Renderer>& getRenderer();
    int getW();
    int getH();
    const char* getTitle();
    float& getCameraX();
    float& getCameraY();
    
}