#version 330 core

in vec2 textcoords;

uniform sampler2D chunk_texture;

out vec4 color;

void main()
{
    color = texture(chunk_texture, textcoords);
}
