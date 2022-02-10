#include "gfx/window.h"
#include "state.h"
#include "utils/types.h"
#include "world/block.h"

struct State state;

internal void
init()
{
        block_list_init();
        direction_init();
        renderer_init(&state.renderer);
        world_init(&state.world, &state.renderer);
}

internal void
destroy()
{
        renderer_destroy(&state.renderer);
        world_destroy(&state.world);
}

internal void
update()
{
        world_update(&state.world);
}

internal void
render()
{
        world_render(&state.world);
}

int
main(void)
{
        window_init(init, destroy, update, render);
        window_loop();
        return (0);
}
