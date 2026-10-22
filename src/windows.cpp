#include <SDL_error.h>
#include <SDL_events.h>
#include <SDL_keycode.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <iostream>
#include <memory>
#include <utility>
#include <SDL2/SDL.h>
#include "api.hpp"
#include "box2d/box2d.h"
#include "box2d/math_types.h"
#include "box2d/types.h"
#include "listeners.hpp"
#include "physics.hpp"
#include "window.hpp"
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

std::pair<bool, const char*> init(){
    using namespace windows;
    int img_flags=IMG_INIT_JPG;
    int sound_flags=MIX_INIT_MP3;
    if (SDL_Init(SDL_INIT_EVERYTHING)){
        return {false, SDL_GetError()};
    }
    if ((IMG_Init(img_flags) & img_flags) != img_flags){
        return {false, IMG_GetError()};
    }
    if (TTF_Init()){
        return {false, TTF_GetError()};
    }
    if ((Mix_Init(sound_flags) & sound_flags) != sound_flags){
        return {false, Mix_GetError()};
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

        api::image im("/home/sree/Downloads/Flappy-Bird.jpg");
        im.autoset=false;
        im.dstrect.w=32;
        im.dstrect.h=32;
        physics::world world((b2Vec2){0,10.0f});
        physics::rigidbody body((b2Vec2){im.dstrect.x/32.0f,im.dstrect.y/32.0f}, world, im.dstrect.w/32.0f, im.dstrect.h/32.0f, b2_dynamicBody);
        
        while (running) {
            while (SDL_PollEvent(&e)) {
                switch (e.type) {
                    case SDL_QUIT:
                        running=false;
                        break;
                    case SDL_KEYUP:
                    case SDL_KEYDOWN:
                        keylistener::reg(e.key);
                        break;
                    
                    case SDL_MOUSEMOTION:
                        mouselistener::reg(e.motion);
                        break;
                    case SDL_MOUSEWHEEL:
                        mouselistener::reg(e.wheel);
                        break;
                    case SDL_MOUSEBUTTONUP:
                    case SDL_MOUSEBUTTONDOWN:
                        mouselistener::reg(e.button);
                        break;
                }
            }

            if (keylistener::isDown(SDLK_ESCAPE)){
                running=false;
            }

            world.simulate();

            b2Vec2 pos=b2Body_GetPosition(body.get());
            im.dstrect.x=pos.x*32;
            im.dstrect.y=pos.y*32;
            SDL_RenderClear(getRenderer().get());
            im.draw();
            SDL_RenderPresent(getRenderer().get());

            keylistener::update();
            mouselistener::update();

            SDL_Delay(16);
        }
    }
    void quit(){
        IMG_Quit();
        TTF_Quit();
        Mix_Quit();
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
        return 800;
    }
    int getH(){
        return 600;
    }
    const char* getTitle(){
        return "Traffic Rider";
    }
}