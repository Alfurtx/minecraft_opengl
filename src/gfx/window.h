#ifndef WINDOW_H
#define WINDOW_H

#include "gfx.h"
#include <cglm/cglm.h>
#include <cglm/types.h>
#include <stdbool.h>

#define WINDOW_SCREEN_WIDTH 1920
#define WINDOW_SCREEN_HEIGHT 1080

typedef void (*window_func)();

struct Button
{
        bool down, last, pressed;
};

struct Mouse
{
        struct Button buttons[GLFW_MOUSE_BUTTON_LAST];
        vec2 position;
        vec2 delta;
};

struct Keyboard
{
        struct Button keys[GLFW_KEY_LAST];
};

struct Window
{
        GLFWwindow* handle;
        struct Mouse mouse;
        struct Keyboard keyboard;

        window_func init;
        window_func destroy;
        window_func update;
        window_func render;

        float deltatime;
        float lastframe;
        float currentframe;
};

extern void window_init(window_func init, window_func destroy, window_func update, window_func render);
extern void window_loop();

extern struct Window window;

#endif // WINDOW_H
