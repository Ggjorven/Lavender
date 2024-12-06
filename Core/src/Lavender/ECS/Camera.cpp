#include "lvpch.h"
#include "Camera.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/RendererConfig.hpp"

#include "Lavender/WorkSpace/EngineTracker.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace Lavender
{

	Camera::Camera(const ShaderResource::Camera& camera)
		: m_View(camera.View), m_Projection(camera.Projection), m_DepthUnpackConsts(camera.DepthUnpackConsts)
	{
	}

	Camera::Camera(float yaw, float pitch, float fov, const glm::vec3& position, float nearr, float farr) 
		: m_Yaw(yaw), m_Pitch(pitch), m_FOV(fov), m_Position(position), m_Near(nearr), m_Far(farr)
	{
		UpdateMatrices();
	}

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

	void Camera::Reset()
	{
		m_Position = { 0.0f, 0.0f, 0.0f };
		m_FOV = 45.0f;

		m_Near = 0.1f;
		m_Far = 1000.0f;

		m_Yaw = 0.0f;
		m_Pitch = 0.0f;

		m_Front = { 0.0f, 0.0f, -1.0f };
		m_Up = { 0.0f, 1.0f, 0.0f };
		m_Right = { 1.0f, 0.0f, 0.0f };

		m_View = {};
		m_Projection = {};
		m_DepthUnpackConsts = {};
	}

	Ref<Camera> Camera::Create(const ShaderResource::Camera& camera)
	{
		return RefHelper::Create<Camera>(camera);
	}

	Ref<Camera> Camera::Create(float yaw, float pitch, float fov, const glm::vec3& position, float nearr, float farr)
	{
		return RefHelper::Create<Camera>(yaw, pitch, fov, position, nearr, farr);
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
		
		if constexpr (RendererSpecification::API == RenderingAPI::Vulkan)
			m_Projection[1][1] *= -1;
	}

}