#ifndef STATE_H_
#define STATE_H_

#include "world/world.h"
#include "gfx/renderer.h"
#include "gfx/camera.h"

struct State
{
        struct World world;
        struct Renderer renderer;
};

extern struct State state;

#endif // STATE_H_
