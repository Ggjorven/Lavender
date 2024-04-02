#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

layout(location = 0) out vec2 v_TexCoord;

struct CameraSpecification
{
    mat4 View;
    mat4 Projection;
};

struct ModelSpecification
{
    mat4 Model;
};

layout(set = 2, binding = 0) uniform CameraSettings
{
    CameraSpecification u_Camera;
};

layout(set = 1, binding = 0) uniform ModelSettings
{
    ModelSpecification u_Model;
};


void main()
{
	gl_Position = u_Camera.Projection * u_Camera.View * u_Model.Model * vec4(a_Position, 1.0);
	v_TexCoord = a_TexCoord;
}