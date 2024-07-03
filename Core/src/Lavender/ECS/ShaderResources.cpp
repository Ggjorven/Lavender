#include "lvpch.h"
#include "ShaderResources.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/RendererConfig.hpp"

#include "Lavender/WorkSpace/EngineTracker.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Lavender::ShaderResource
{

	Camera::Camera(float yaw, float pitch, float fov, const glm::vec3& position, float nearr, float farr)
	{
		// Update front, up & right
		glm::vec3 front = glm::vec3(1.0f);
		front.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
		front.y = glm::sin(glm::radians(pitch));
		front.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));

		front = glm::normalize(front);
		glm::vec3 up = { 0.0f, 1.0f, 0.0f };
		glm::vec3 right = glm::normalize(glm::cross(front, up));

		// View & Projection
		View = glm::lookAt(position, position + front, up);
		Projection = glm::perspective(glm::radians(fov), (float)Track::Viewport::Width / (float)Track::Viewport::Height, nearr, farr);

		if constexpr (RendererSpecification::API == RenderingAPI::Vulkan)
			Projection[1][1] *= -1;

		// Depth unpack consts
		{
			float depthLinearizeMul = (-Projection[3][2]);
			float depthLinearizeAdd = (Projection[2][2]);
			// Correct the handedness issue.
			if (depthLinearizeMul * depthLinearizeAdd < 0)
				depthLinearizeAdd = -depthLinearizeAdd;
			DepthUnpackConsts = { depthLinearizeMul, depthLinearizeAdd };
		}
	}

}