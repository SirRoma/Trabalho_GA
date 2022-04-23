#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texCoord;


//layout(binding = 0) uniform sampler2D tex1;
//layout(binding = 1) uniform sampler2D tex2;


out vec4 ourColor;
out vec2 TexCoord;


uniform mat4 model;
uniform mat4 projection;


void main()
{
    gl_Position = projection * model * vec4(position, 1.0f);
    ourColor = vec4(color,1.0);
    TexCoord = vec2(texCoord.x, 1.0 - texCoord.y);
}
