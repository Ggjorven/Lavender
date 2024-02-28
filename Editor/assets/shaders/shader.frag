#version 460 core

layout(location = 0) out vec4 o_Colour;

layout(std140, set = 0, binding = 0) uniform ColourBlock 
{
    vec4 u_Colour;
} m_ColourBlock;

void main() 
{
    o_Colour = m_ColourBlock.u_Colour;
}
