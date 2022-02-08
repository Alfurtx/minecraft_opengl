#version 330 core

in vec2 textcoords;

uniform sampler2D chunk_texture;

out vec4 color;

void main()
{
    color = texture(chunk_texture, textcoords);
    // color = vec4(1.0, 0.2, 0.5, 1.0);
}
