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
}

internal void
render()
{
}

int
main(void)
{
        renderer_init(&state.renderer);
        window_init(init, destroy, update, render);
        window_loop();
        return (0);
}
