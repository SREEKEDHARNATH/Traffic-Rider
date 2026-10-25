#include <SDL_error.h>
#include <SDL_events.h>
#include <SDL_keycode.h>
#include <SDL_rect.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <cstdlib>
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
    using namespace window;
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
bool areidentical(b2BodyId bida, b2BodyId body){
    return (bida.generation==body.generation && bida.index1==body.index1 && bida.world0==body.world0);
}
namespace window {

    void run(){
        auto i=init();
        if (!i.first){
            std::cerr << i.second << "\n";
            return;
        }

        SDL_Event e;
        bool running=true;

        getCameraX()=(getW()/2.0);
        api::image im("assets/images/car.jpg");
        api::image road("assets/images/road.jpg");
        road.autoset=false;
        road.dstrect.h=getH();
        road.dstrect.w=getH()/2.0;
        road.dstrect.x=getCameraX()-128;

        im.autoset=false;
        im.dstrect.w=32;
        im.dstrect.h=32;
        im.dstrect.x=(getW()/2.0);
        im.dstrect.y=getH()/2.0;
        physics::world world((b2Vec2){0,3.0f});
        physics::rigidbody body((b2Vec2){im.dstrect.x/32.0f,im.dstrect.y/32.0f}, world, im.dstrect.w/32.0f, im.dstrect.h/32.0f, b2_dynamicBody);
        physics::rigidbody wall1((b2Vec2){road.dstrect.x/32.0f,800/64.0f}, world, 8/32.0f, getH()+128/32.0f, b2_staticBody);
        physics::rigidbody wall2((b2Vec2){(road.dstrect.x+280)/32.0f,800/64.0f}, world, 8/32.0f, getH()+128/32.0f, b2_staticBody);

        api::image bush("assets/images/bush.jpg");
        bush.dstrect.x = ((rand()%4)*50)+300;
        bush.dstrect.w*=2;
        bush.dstrect.h*=2;

        physics::rigidbody bodyBush((b2Vec2){bush.dstrect.x/32.0f,bush.dstrect.y/32.0f}, world, bush.dstrect.w/32.0f, bush.dstrect.h/32.0f, b2_dynamicBody);
        b2Body_SetLinearVelocity(bodyBush.get(), (b2Vec2){0,1});

        float xForce=0,yForce=0;
        float absForce=12;
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
            if (im.dstrect.y < 0 || im.dstrect.y > 570){
                running=false;
            }
            if (bush.dstrect.y > 570){
                bush.dstrect.x = ((rand()%4)*50)+300;
                bush.dstrect.y=0;
                b2Body_SetTransform(bodyBush.get(), (b2Pos){(float)bush.dstrect.x/32.0f, (float)bush.dstrect.y/32.0f}, b2DefaultBodyDef().rotation);
                b2Body_SetLinearVelocity(bodyBush.get(), (b2Vec2){0,1});
            }
            if (keylistener::isDown(SDLK_ESCAPE)){
                running=false;
            }
            auto vel=b2Body_GetLinearVelocity(body.get());
            if (keylistener::isDown(SDLK_LEFT)){
                xForce=-1*absForce;
                b2Body_ApplyForceToCenter(body.get(), (b2Vec2){xForce, 0},false);
            }
            else if (keylistener::isDown(SDLK_RIGHT)){
                xForce=1*absForce;
                b2Body_ApplyForceToCenter(body.get(), (b2Vec2){xForce, 0},false);
            }
            if (keylistener::isDown(SDLK_UP)){
                yForce=-1*absForce;
                b2Body_ApplyForceToCenter(body.get(), (b2Vec2){0, yForce},false);
            }
            else if (keylistener::isDown(SDLK_DOWN)){
                yForce=1*absForce;
                b2Body_ApplyForceToCenter(body.get(), (b2Vec2){0, yForce},false);
            }
            
            world.simulate();
            b2ContactEvents events = b2World_GetContactEvents(world.get());
            for (int i=0;i<events.beginCount;i++){
                b2ContactBeginTouchEvent ev=events.beginEvents[i];
                
                auto bida=b2Shape_GetBody(ev.shapeIdA);
                auto bidb=b2Shape_GetBody(ev.shapeIdB);
                
                if ((areidentical(bida, body.get()) && areidentical(bidb, bodyBush.get())) || areidentical(bidb, body.get()) && areidentical(bida, bodyBush.get())){
                    running=false;
                }
            }
            {
                b2Vec2 pos=b2Body_GetPosition(body.get());
                im.dstrect.x=pos.x*32;
                im.dstrect.y=pos.y*32;
            }
            {
                b2Vec2 pos=b2Body_GetPosition(bodyBush.get());
                bush.dstrect.x=pos.x*32;
                bush.dstrect.y=pos.y*32;
            }

            getCameraY()=im.dstrect.y;

            SDL_RenderClear(getRenderer().get());
            road.dstrect.y=(getH()/2.0)-300;
            road.draw();
            bush.draw();
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
    float& getCameraX(){
        static float x=0.0f;
        return x;
    }
    float& getCameraY(){
        static float y=0.0f;
        return y;
    }
}