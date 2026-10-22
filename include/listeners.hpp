#pragma once

#include <SDL_events.h>
#include <SDL_keycode.h>
namespace keylistener{
    void reg(SDL_KeyboardEvent key);
    bool isPressed(SDL_Keycode k);
    bool isPressed(SDL_Scancode sc);
    bool isDown(SDL_Keycode k);
    bool isDown(SDL_Scancode sc);
    void update();
}

namespace mouselistener {
    void reg(SDL_MouseMotionEvent m);
    void reg(SDL_MouseWheelEvent w);
    void reg(SDL_MouseButtonEvent b);
    
    bool isPressed(int b);
    bool isDown(int b);
    
    void update();

    int getX();
    int getY();
    
    float getScrollX();
    float getScrollY();
    
}