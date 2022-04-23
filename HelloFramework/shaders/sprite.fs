#version 450 core
in vec4 ourColor;
in vec2 TexCoord;

out vec4 color;

// pixels da textura
uniform sampler2D tex;

void main()
{
    color = texture(tex, TexCoord);
}