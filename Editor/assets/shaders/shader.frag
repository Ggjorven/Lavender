#version 460 core

layout(location = 0) out vec4 o_Colour;

layout(location = 0) in vec3 v_Position;
layout(location = 1) in vec2 v_TexCoord;
layout(location = 2) in vec3 v_Normal;

// Structs
struct PointLight 
{
    vec3 Position;
    vec3 Rotation;

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

mat3 rotationMatrix(vec3 rotation)
{
    float cosX = cos(rotation.x);
    float sinX = sin(rotation.x);
    float cosY = cos(rotation.y);
    float sinY = sin(rotation.y);
    float cosZ = cos(rotation.z);
    float sinZ = sin(rotation.z);

    mat3 rotateX = mat3(1.0, 0.0, 0.0,
                        0.0, cosX, -sinX,
                        0.0, sinX, cosX);

    mat3 rotateY = mat3(cosY, 0.0, sinY,
                        0.0, 1.0, 0.0,
                        -sinY, 0.0, cosY);

    mat3 rotateZ = mat3(cosZ, -sinZ, 0.0,
                        sinZ, cosZ, 0.0,
                        0.0, 0.0, 1.0);

    return rotateX * rotateY * rotateZ;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 albedoCol = (texture(u_AlbedoImage, v_TexCoord) * u_Material.AlbedoColour).rgb;
    vec3 specCol = (texture(u_SpecularImage, v_TexCoord) * u_Material.SpecularColour).rgb;

    // Apply rotation to the light direction
    vec3 lightDir = normalize(light.Position - v_Position);
    lightDir = mat3(rotationMatrix(light.Rotation)) * lightDir;

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