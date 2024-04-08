#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec3 a_Normal;

layout(location = 0) out vec3 v_Position;
layout(location = 1) out vec2 v_TexCoord;
layout(location = 2) out vec3 v_Normal;

struct CameraSpecification
{
    mat4 View;
    mat4 Projection;
};

struct ModelSpecification
{
    mat4 Model;
};

layout(std140, set = 1, binding = 0) uniform CameraSettings
{
    CameraSpecification u_Camera;
};

layout(std140, set = 0, binding = 1) uniform ModelSettings
{
    ModelSpecification u_Model;
};


void main()
{
	gl_Position = u_Camera.Projection * u_Camera.View * u_Model.Model * vec4(a_Position, 1.0);
	
    v_Position = a_Position;
    v_TexCoord = a_TexCoord;
    v_Normal = a_Normal;
}