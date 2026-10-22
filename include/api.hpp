#pragma once

#include <SDL_rect.h>
#include <SDL_render.h>
#include <memory>
namespace api{
    class image{
        std::shared_ptr<SDL_Texture> ptr;
        public:
            bool autoset=true;
            SDL_Rect dstrect={0,0,0,0};
            image(const char* name);
            void draw();
            void setTex(SDL_Texture* tex);
            std::shared_ptr<SDL_Texture> getTex();
    };
}