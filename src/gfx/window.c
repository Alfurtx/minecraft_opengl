#include "../state.h"
#include "../utils/types.h"
#include "camera.h"
#include "gfx.h"
#include "window.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

// NOTE(fonsi): global window
struct Window window;

internal void          _size_callback(GLFWwindow* window_handle, int width, int height);
internal void          _cursor_callback(GLFWwindow* window_handle, double x_pos, double y_pos);
internal void          _key_callback(GLFWwindow* window_handle, int key, int scancode, int action, int mods);
internal void          _mouse_callback(GLFWwindow* window_handle, int button, int action, int mods);
internal void APIENTRY messsage_callback(GLenum        source,
                                         GLenum        type,
                                         GLuint        id,
                                         GLenum        severity,
                                         GLsizei       length,
                                         const GLchar* message,
                                         const void*   userParam);

internal void proccess_input(GLFWwindow* window_handle);

void
window_init(window_func init, window_func destroy, window_func update, window_func render)
{
        window.init    = init;
        window.destroy = destroy;
        window.update  = update;
        window.render  = render;

        window.deltatime = 0.0f;
        window.lastframe = 0.0f;

        window.wireframe = false;

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
        glfwSetCursorPosCallback(window.handle, _cursor_callback);

        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
        {
                fprintf(stderr, "Failed to inititialize GLAD\n");
                exit(-1);
        }

        glEnable(GL_DEPTH_TEST);
        // glEnable(GL_CULL_FACE);
        // glCullFace(GL_FRONT);

        if (GLAD_GL_ARB_debug_output)
        {
                int flags;
                glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
                if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
                {
                        glEnable(GL_DEBUG_OUTPUT);
                        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
                        glDebugMessageCallback(messsage_callback, 0);
                        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
                }
        }
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

                proccess_input(window.handle);

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
        camera_proccess_mouse(state.renderer.current_camera, x_pos, y_pos);
}

internal void
_key_callback(GLFWwindow* window_handle, int key, int scancode, int action, int mods)
{
        if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
                glfwSetWindowShouldClose(window.handle, GLFW_TRUE);

        if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
        {
                glPolygonMode(GL_FRONT_AND_BACK, window.wireframe ? GL_FILL : GL_LINE);
                window.wireframe = !window.wireframe;
        }
}

internal void
_mouse_callback(GLFWwindow* window_handle, int button, int action, int mods)
{
        // TODO(fonsi): lidiar con los mouse_callbacks
}

internal void APIENTRY
messsage_callback(GLenum        source,
                  GLenum        type,
                  GLuint        id,
                  GLenum        severity,
                  GLsizei       length,
                  const GLchar* message,
                  const void*   userParam)
{
        if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
                return;

        // clang-format off
        fprintf(stderr, "--------------------\n");
        fprintf(stderr, "Debug Message ( %d )\n", id);

        switch (source)
        {
        case GL_DEBUG_SOURCE_API: fprintf(stderr, "Source: API"); break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: fprintf(stderr, "Source: Window System"); break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: fprintf(stderr, "Source: Shader Compiler"); break;
        case GL_DEBUG_SOURCE_THIRD_PARTY: fprintf(stderr, "Source: Third Party"); break;
        case GL_DEBUG_SOURCE_APPLICATION: fprintf(stderr, "Source: Application"); break;
        case GL_DEBUG_SOURCE_OTHER: fprintf(stderr, "Source: Other"); break;
        }
        fprintf(stderr, "\n");

        switch (type)
        {
        case GL_DEBUG_TYPE_ERROR: fprintf(stderr, "Type: Error"); break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: fprintf(stderr, "Type: Deprecated Behaviour"); break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: fprintf(stderr, "Type: Undefined Behaviour"); break;
        case GL_DEBUG_TYPE_PORTABILITY: fprintf(stderr, "Type: Portability"); break;
        case GL_DEBUG_TYPE_PERFORMANCE: fprintf(stderr, "Type: Performance"); break;
        case GL_DEBUG_TYPE_MARKER: fprintf(stderr, "Type: Marker"); break;
        case GL_DEBUG_TYPE_PUSH_GROUP: fprintf(stderr, "Type: Push Group"); break;
        case GL_DEBUG_TYPE_POP_GROUP: fprintf(stderr, "Type: Pop Group"); break;
        case GL_DEBUG_TYPE_OTHER: fprintf(stderr, "Type: Other"); break;
        }
        fprintf(stderr, "\n");

        switch (severity)
        {
        case GL_DEBUG_SEVERITY_HIGH: fprintf(stderr, "Severity: high"); break;
        case GL_DEBUG_SEVERITY_MEDIUM: fprintf(stderr, "Severity: medium"); break;
        case GL_DEBUG_SEVERITY_LOW: fprintf(stderr, "Severity: low"); break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: fprintf(stderr, "Severity: notification"); break;
        }
        fprintf(stderr, "\n");
        fprintf(stderr, "Message: %s\n", message);
        // clang-format on
}

internal void
proccess_input(GLFWwindow* window_handle)
{
        if (glfwGetKey(window_handle, GLFW_KEY_W) == GLFW_PRESS)
                camera_proccess_keyboard(state.renderer.current_camera, CAM_MOVE_FORWARD, window.deltatime);

        if (glfwGetKey(window_handle, GLFW_KEY_S) == GLFW_PRESS)
                camera_proccess_keyboard(state.renderer.current_camera, CAM_MOVE_BACKWARD, window.deltatime);

        if (glfwGetKey(window_handle, GLFW_KEY_A) == GLFW_PRESS)
                camera_proccess_keyboard(state.renderer.current_camera, CAM_MOVE_LEFT, window.deltatime);

        if (glfwGetKey(window_handle, GLFW_KEY_D) == GLFW_PRESS)
                camera_proccess_keyboard(state.renderer.current_camera, CAM_MOVE_RIGHT, window.deltatime);

        if (glfwGetKey(window_handle, GLFW_KEY_SPACE) == GLFW_PRESS)
                camera_proccess_keyboard(state.renderer.current_camera, CAM_MOVE_UP, window.deltatime);

        if (glfwGetKey(window_handle, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
                camera_proccess_keyboard(state.renderer.current_camera, CAM_MOVE_DOWN, window.deltatime);
}
