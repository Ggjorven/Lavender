#pragma once

#include "Lavender/Core/Core.hpp"
#include "Lavender/Core/Events.hpp"
#include "Lavender/Utils/Utils.hpp"

#include "Lavender/ECS/ShaderResources.hpp"

#include <glm/glm.hpp>

namespace Lavender
{

	class Camera
	{
	public:
		Camera() = default;
		Camera(const ShaderResource::Camera& camera);
		Camera(float yaw, float pitch, float fov, const glm::vec3& position, float nearr = 0.1f, float farr = 1000.0f); // Naming sucks because of Windows #define near & far #undef doesn't work
		virtual ~Camera() = default;

		virtual void OnUpdate(float deltaTime) {}
		virtual void OnEvent(Event& e) {}

		inline glm::vec3& GetPosition() { return m_Position; }
		inline float& GetFOV() { return m_FOV; }

		inline float& GetNear() { return m_Near; }
		inline float& GetFar() { return m_Far; }

		inline float& GetYaw() { return m_Yaw; }
		inline float& GetPitch() { return m_Pitch; }

		inline glm::vec3& GetFront() { return m_Front; }
		inline glm::vec3& GetUp() { return m_Up; }
		inline glm::vec3& GetRight() { return m_Right; }

		inline glm::mat4& GetViewMatrix() { return m_View; }
		inline glm::mat4& GetProjectionMatrix() { return m_Projection; }
		inline glm::vec2& GetDepthUnPackConsts() { return m_DepthUnpackConsts; }

		ShaderResource::Camera AsResource();
		inline operator ShaderResource::Camera() { return AsResource(); }

		void Reset();

		static Ref<Camera> Create(const ShaderResource::Camera& camera = {});
		static Ref<Camera> Create(float yaw, float pitch, float fov, const glm::vec3& position, float nearr = 0.1f, float farr = 1000.0f);

	protected:
		void UpdateMatrices();

	protected:
		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		float m_FOV = 45.0f;

		float m_Near = 0.1f;
		float m_Far = 1000.0f;
		
		float m_Yaw = 0.0f;
		float m_Pitch = 0.0f;

		glm::vec3 m_Front = { 0.0f, 0.0f, -1.0f };
		glm::vec3 m_Up = { 0.0f, 1.0f, 0.0f };
		glm::vec3 m_Right = { 1.0f, 0.0f, 0.0f };

		// Matrices
		glm::mat4 m_View = {};
		glm::mat4 m_Projection = {};
		glm::vec2 m_DepthUnpackConsts = {};
	};

}