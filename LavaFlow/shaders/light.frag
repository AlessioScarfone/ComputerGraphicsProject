#version 330 core
in vec2 TexCoords;

out vec4 color;

uniform sampler2D texture_diffuse1;

void main()
{

    color = vec4(0.7f, 0.7f, 0.0f, 1.0f);
}