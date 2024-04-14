#version 460 core

layout(location = 0) out vec4 o_Colour;

layout(location = 0) in vec3 v_Position;
layout(location = 1) in vec2 v_TexCoord;
layout(location = 2) in vec3 v_Normal;

// Structs
struct PointLight 
{
    vec3 Position;

    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
    
    float Constant;
    float Linear;
    float Quadratic;
};

struct Material
{
    vec4 AlbedoColour;
    vec4 SpecularColour;
    float Shininess;
};

// Inputs
// [ Set 0 ]
layout(set = 0, binding = 0) uniform sampler2D u_AlbedoImage;
layout(set = 0, binding = 1) uniform sampler2D u_SpecularImage;

layout(std140, set = 0, binding = 3) uniform MaterialSettings
{
    Material u_Material;
};

// [ Set 1 ]
#define MAX_POINTLIGHTS 10
layout(std140, set = 1, binding = 1) uniform LightSettings
{
    PointLight PointLights[MAX_POINTLIGHTS];
    uint AmountOfPointLights;
} u_LightSettings;

layout(std140, set = 1, binding = 2) uniform SceneData
{
    vec3 ViewPosition;
} u_SceneData;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main() 
{
    // Properties
    vec3 norm = normalize(v_Normal);
    vec3 viewDir = normalize(u_SceneData.ViewPosition - v_Position);
    
    vec3 result = vec3(0.0);

    // Point Lights
    for(int i = 0; i < u_LightSettings.AmountOfPointLights; i++)
        result += CalcPointLight(u_LightSettings.PointLights[i], norm, v_Position, viewDir);    
    
    // Check if there were any lights // TODO: When adding more light types add to this
    if (u_LightSettings.AmountOfPointLights != 0)
    {
        o_Colour = vec4(result, 1.0);
    }
    else
    {
        o_Colour =  (texture(u_AlbedoImage, v_TexCoord) * u_Material.AlbedoColour);
    }
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 albedoCol = (texture(u_AlbedoImage, v_TexCoord) * u_Material.AlbedoColour).rgb;
    vec3 specCol = (texture(u_SpecularImage, v_TexCoord) * u_Material.SpecularColour).rgb;

    vec3 lightDir = normalize(light.Position - v_Position);

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.Shininess);

    // Attenuation
    float distance = length(light.Position - v_Position);
    float attenuation = 1.0 / (light.Constant + light.Linear * distance + light.Quadratic * (distance * distance));    

    // Combine results
    vec3 ambient = light.Ambient * albedoCol;
    vec3 diffuse = light.Diffuse * diff * albedoCol;
    vec3 specular = light.Specular * spec * specCol;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}