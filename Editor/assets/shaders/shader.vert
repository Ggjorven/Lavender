#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

layout(location = 0) out vec2 v_TexCoord;

struct Camera
{
    mat4 Model;
    mat4 View;
    mat4 Projection;
};

layout(binding = 1) uniform CameraSettings
{
    Camera u_Camera;
};

void main()
{
	gl_Position = u_Camera.Projection * u_Camera.View * u_Camera.Model * vec4(a_Position, 1.0);
	v_TexCoord = a_TexCoord;
}