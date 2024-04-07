#version 460 core

layout(location = 0) out vec4 o_Colour;

layout(location = 0) in vec2 v_TexCoord;
layout(location = 1) in vec3 v_Normal;

layout(set = 0, binding = 0) uniform sampler2D u_Image;

struct DirectionalLight
{
    vec4 Colour;
    vec3 Direction;
    // 4 byte padding
    float Intensity; 
    // 12 byte padding?  
};

#define MAX_DIRECTIONAL_LIGHTS 10
layout(std140, set = 1, binding = 1) uniform DirectionalLightSettings
{
    DirectionalLight u_DirectionalLights[MAX_DIRECTIONAL_LIGHTS];
};

layout(std140, set = 1, binding = 2) uniform SceneData
{
    uint AmountOfDirectionalLights;
} u_SceneData;

void main() 
{
    vec4 finalColor = vec4(0.0);
    for (int i = 0; i < u_SceneData.AmountOfDirectionalLights; ++i) 
    {
        finalColor += u_DirectionalLights[i].Colour;
    }
    o_Colour = finalColor;
}