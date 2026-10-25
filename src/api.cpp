#include "api.hpp"
#include "window.hpp"
#include <SDL_image.h>
#include <SDL_render.h>
#include <iostream>

namespace api {
    image::image(const char* name){
        SDL_Texture* tex=IMG_LoadTexture(window::getRenderer().get(), name);
        if (!tex){
            std::cerr << IMG_GetError() << "\n";
            return;
        }
        ptr.reset(tex, SDL_DestroyTexture);
        SDL_QueryTexture(ptr.get(), nullptr,nullptr, &dstrect.w, &dstrect.h);
    }
    void image::draw(){
        if (ptr.get()){
            if (autoset){
                SDL_QueryTexture(ptr.get(), nullptr,nullptr, &dstrect.w, &dstrect.h);
            }
            if (dstrect.w==0 || dstrect.h==0){
                SDL_RenderCopy(window::getRenderer().get(), ptr.get(), nullptr, nullptr);
            }
            else {
                SDL_RenderCopy(window::getRenderer().get(), ptr.get(), nullptr, &dstrect);
            }
        }
    }
    void image::setTex(SDL_Texture* tex){
        if (tex){
            ptr.reset(tex, SDL_DestroyTexture);
        }
    }
    std::shared_ptr<SDL_Texture> image::getTex(){
        return ptr;
    }
}