#include "lvpch.h"
#include "Camera.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/RendererConfig.hpp"

#include "Lavender/WorkSpace/EngineTracker.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace Lavender
{

	ShaderResource::Camera Camera::AsResource()
	{
		ShaderResource::Camera camera = {};
		camera.View = m_View;
		camera.Projection = m_Projection;

		{
			float depthLinearizeMul = (-m_Projection[3][2]);
			float depthLinearizeAdd = (m_Projection[2][2]);
			// Correct the handedness issue.
			if (depthLinearizeMul * depthLinearizeAdd < 0)
				depthLinearizeAdd = -depthLinearizeAdd;
			m_DepthUnpackConsts = { depthLinearizeMul, depthLinearizeAdd };
		}

		camera.DepthUnpackConsts = m_DepthUnpackConsts;

		return camera;
	}

	void Camera::UpdateMatrices()
	{
		glm::vec3 newFront(1.0f);
		newFront.x = glm::cos(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch));
		newFront.y = glm::sin(glm::radians(m_Pitch));
		newFront.z = glm::sin(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch));

		m_Front = glm::normalize(newFront);
		m_Right = glm::normalize(glm::cross(m_Front, m_Up));

		// Update everything
		m_View = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
		m_Projection = glm::perspective(glm::radians(m_FOV), (float)Track::Viewport::Width / (float)Track::Viewport::Height, m_Near, m_Far);
		
		if constexpr (RendererSpecification::API == RendererSpecification::RenderingAPI::Vulkan)
			m_Projection[1][1] *= -1;
	}

}