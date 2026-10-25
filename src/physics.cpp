#include "physics.hpp"
#include "box2d/box2d.h"
#include "box2d/id.h"
#include "box2d/types.h"
#include <iostream>
#include <memory>

void del(b2ShapeId* sh){
    if (sh){
        if (b2Shape_IsValid(*sh)){
            b2DestroyShape(*sh, false);
        }
        delete sh;
    }
}
void delB(b2BodyId* b){
    if (b){
        if (b2Body_IsValid(*b)){
            b2DestroyBody(*b);
        }
        delete b;
    }
}
void delW(b2WorldId* w){
    if (w){
        if (b2World_IsValid(*w)) {
            b2DestroyWorld(*w);
        }
        delete w;
    }
}

namespace physics {
    world::world(b2Vec2 gravity){
        b2WorldDef wd = b2DefaultWorldDef();
        wd.gravity=gravity;
        b2WorldId* wr=new b2WorldId();
        *wr=b2CreateWorld(&wd);
        if (!b2World_IsValid(*wr)){
            std::cerr << "World creation failed!";
            return;
        }
        w.reset(wr, delW);
    }
    void world::simulate(float timestep, int substepcount){
        if (b2World_IsValid(get())){
            b2World_Step(get(), timestep, substepcount);
        }
        else {
            std::cerr << "Invalid World!\n";
        }
    }
    b2WorldId world::get(){
        return *(w.get());
    }

    rigidbody::rigidbody(b2Vec2 pos, world w, float wi, float he, b2BodyType t){
        if (b2World_IsValid(w.get())){
            b2BodyDef bd = b2DefaultBodyDef();
            bd.position=pos;
            bd.type=t;
            bd.isAwake=true;
            b2BodyId* id=new b2BodyId();
            *id=b2CreateBody(w.get(), &bd);
            b.reset(id, delB);
    
            b2Polygon poly = b2MakeBox(wi/2, he/2);
            b2ShapeDef sh=b2DefaultShapeDef();
            sh.density=1.0;
            sh.enableContactEvents = true; 
            b2ShapeId* shape=new b2ShapeId();
            *shape=b2CreatePolygonShape(*id, &sh, &poly);
    
            s.reset(shape,del);
        }
        else {
            std::cerr << "World Is Inavlid!";
        }
    }

    b2BodyId rigidbody::get(){
        return *(b.get());
    }
}