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