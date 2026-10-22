#pragma once

#include <memory>
#include "box2d/id.h"
#include "box2d/math_types.h"
#include "box2d/types.h"

namespace physics {
    class world{
        std::shared_ptr<b2WorldId> w;

        public:
            world(b2Vec2 gravity={0,-10});
            void simulate(float timestep=1/60.0f, int substepcount=4);
            b2WorldId get();
    };
    class rigidbody{
        std::shared_ptr<b2BodyId> b;
        std::shared_ptr<b2ShapeId> s;
        
        public:
            rigidbody(b2Vec2 pos, world w, float wi, float he, b2BodyType t);
            b2BodyId get();
    };
}