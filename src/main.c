#include "gfx/window.h"
#include "utils/types.h"

internal void
init()
{
}

internal void
destroy()
{
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
        window_init(init, destroy, update, render);
        window_loop();
        return (0);
}
