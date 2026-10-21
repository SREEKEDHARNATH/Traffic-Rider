#include <SDL_error.h>
#include <SDL_events.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <iostream>
#include <memory>
#include <utility>
#include <SDL2/SDL.h>
#include "window.h"


std::pair<bool, const char*> init(){
    using namespace windows;
    if (SDL_Init(SDL_INIT_EVERYTHING)){
        return {false, SDL_GetError()};
    }
    auto* a=SDL_CreateWindow(getTitle(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, getW(), getH(),0);
    if (!a){
        return {false, SDL_GetError()};
    }
    getWindow().reset(a, SDL_DestroyWindow);
    auto b = SDL_CreateRenderer(getWindow().get(), -1, 0);
    if (!b){
        return {false, SDL_GetError()};
    }
    getRenderer().reset(b, SDL_DestroyRenderer);
    return {true,"Good"};
}
namespace windows {

    void run(){
        auto i=init();
        if (!i.first){
            std::cerr << i.second << "\n";
            return;
        }

        SDL_Event e;
        bool running=true;
        while (running) {
            while (SDL_PollEvent(&e)) {
                switch (e.type) {
                    case SDL_QUIT:
                        running=false;
                        break;
                }
            }

            SDL_RenderClear(getRenderer().get());
            SDL_RenderPresent(getRenderer().get());

            SDL_Delay(16);
        }
    }
    void quit(){
        SDL_Quit();
    }
    std::shared_ptr<SDL_Window>& getWindow(){
        static std::shared_ptr<SDL_Window> window(nullptr);
        return window;
    }
    std::shared_ptr<SDL_Renderer>& getRenderer(){
        static std::shared_ptr<SDL_Renderer> renderer(nullptr);
        return renderer;
    }
    int getW(){
        return 1280;
    }
    int getH(){
        return 720;
    }
    const char* getTitle(){
        return "Traffic Rider";
    }
}