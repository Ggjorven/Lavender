#pragma once

#include "Lavender/Core/Core.hpp"
#include "Lavender/Utils/Utils.hpp"

#include <glm/glm.hpp>

namespace Lavender::ShaderResource
{

	struct Camera
	{
	public:
		glm::mat4 View = {};
		glm::mat4 Projection = {};
		glm::vec2 DepthUnpackConsts = {};
		PUBLIC_PADDING(8)

	public:
		Camera() = default;
		Camera(float yaw, float pitch, float fov, const glm::vec3& position, float nearr = 0.1f, float farr = 1000.0f);  // Naming sucks because of Windows #define near & far #undef doesn't work
		~Camera() = default;
	};

	struct PointLight
	{
	public:
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		float Radius = 5.0f;

		glm::vec3 Colour = { 1.0f, 1.0f, 1.0f };
		float Intensity = 1.0f;
	};

}