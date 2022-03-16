#ifndef WORLD_H_
#define WORLD_H_

#include "../gfx/window.h"
#include "../utils/types.h"
#include "../utils/utils.h"
#include "chunk.h"

extern vec3 WORLD_CHUNK_SURROUNDINGS[];

struct World
{
};

extern void          world_init(struct World* world, struct Renderer* renderer);
extern void          world_destroy(struct World* world);
extern void          world_update(struct World* world);
extern void          world_render(struct World* world);
extern void          world_tick(struct World* world);

#endif // WORLD_H_
