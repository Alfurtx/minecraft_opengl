#version 330 core

layout (location = 0) in vec3 position_coords;
layout (location = 1) in vec2 texture_coords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 textcoords;

void main()
{
    gl_Position = projection * view * model * vec4(position_coords, 1.0);
    textcoords = texture_coords
;
}
