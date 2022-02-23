#version 330 core

in vec2 textcoords;

uniform sampler2D chunk_texture;

out vec4 color;

void main()
{
    vec4 aux = texture(chunk_texture, textcoords);
    if(aux.a < 0.1)
        discard;
    color = aux;
    // color = vec4(1.0, 0.2, 0.5, 1.0);
}
