#include "../utils/types.h"
#include "camera.h"
#include "gfx.h"
#include "../state.h"
#include "window.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

// NOTE(fonsi): global window
struct Window window;

internal void _size_callback(GLFWwindow* window_handle, int width, int height);
internal void _cursor_callback(GLFWwindow* window_handle, double x_pos, double y_pos);
internal void _key_callback(GLFWwindow* window_handle, int key, int scancode, int action, int mods);
internal void _mouse_callback(GLFWwindow* window_handle, int button, int action, int mods);
internal void _error_callback(int code, const char* description);

void
window_init(window_func init, window_func destroy, window_func update, window_func render)
{
        window.init    = init;
        window.destroy = destroy;
        window.update  = update;
        window.render  = render;

        window.deltatime = 0.0f;
        window.lastframe = 0.0f;

        glfwSetErrorCallback(_error_callback);

        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#if __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

        window.handle = glfwCreateWindow(WINDOW_SCREEN_WIDTH, WINDOW_SCREEN_HEIGHT, "Minecraft OpenGL", NULL, NULL);
        if (!window.handle)
        {
                fprintf(stderr, "\nFailed to create window\n");
                glfwTerminate();
                exit(-1);
        }
        glfwMakeContextCurrent(window.handle);

        glfwSetInputMode(window.handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetFramebufferSizeCallback(window.handle, _size_callback);
        glfwSetKeyCallback(window.handle, _key_callback);

        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
        {
                fprintf(stderr, "Failed to inititialize GLAD\n");
                exit(-1);
        }

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        glfwSwapInterval(1);
}

void
window_loop()
{
        window.init();

        while (!glfwWindowShouldClose(window.handle))
        {
                window.currentframe = glfwGetTime();
                window.deltatime    = window.currentframe - window.lastframe;
                window.lastframe    = window.currentframe;

                window.update();

                glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                window.render();

                glfwSwapBuffers(window.handle);
                glfwPollEvents();
        }

        window.destroy();
        glfwTerminate();
}

internal void
_size_callback(GLFWwindow* window_handle, int width, int height)
{
        glViewport(0, 0, width, height);
}

internal void
_cursor_callback(GLFWwindow* window_handle, double x_pos, double y_pos)
{
        camera_process_mouse(state.renderer.current_camera, x_pos, y_pos);
}

internal void
_key_callback(GLFWwindow* window_handle, int key, int scancode, int action, int mods)
{
        if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
                glfwSetWindowShouldClose(window.handle, GLFW_TRUE);
        camera_process_keyboard(&state.renderer.cameras[RENDERER_CHUNK], key, action, window.deltatime);
}

internal void
_mouse_callback(GLFWwindow* window_handle, int button, int action, int mods)
{
        // TODO(fonsi): lidiar con los mouse_callbacks
}

internal void
_error_callback(int code, const char* description)
{
        fprintf(stderr, "OpenGL ERROR\nCode: %d\nDescription: %s", code, description);
}
