#version 460 core

layout(location = 0) out vec4 colour;
layout(location = 0) in vec2 v_TexCoord;

layout(binding = 0) uniform sampler2D u_Tex;

void main()
{
    colour = texture(u_Tex, v_TexCoord);
}
