#pragma once

#include "Lavender/Core/Events.hpp"

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Renderer/Viewport.hpp"
#include "Lavender/Renderer/UniformBuffer.hpp"
#include "Lavender/Renderer/Pipeline.hpp"
#include "Lavender/Renderer/RenderCommandBuffer.hpp"

#include <glm/glm.hpp>
#include <glm/detail/type_quat.hpp>
#include <glm/ext/matrix_clip_space.hpp>

namespace Lavender
{

	class Camera
	{
	public:
		glm::mat4 View = glm::mat4(1.0f);
		glm::mat4 Projection = glm::mat4(1.0f);

	};

	class EditorCamera
	{
	public:
		enum class State
		{
			None = 0, ArcBall, FlyCam
		};
		static std::string StateToString(State state);
	public:
		EditorCamera(Ref<Viewport> viewport);
		virtual ~EditorCamera();

		void OnUpdate(float deltaTime);
		void OnEvent(Event& e);

		void UpdateAndUpload();
		void BindDescriptorSet(Ref<Pipeline> pipeline, Ref<RenderCommandBuffer> cmdBuffer);

		void SwitchState();

		inline float& GetFOV() { return m_FOV; }
		inline float& GetFlyCamSpeed() { return m_MovementSpeed; }
		inline float& GetArcBallSpeed() { return m_Speed; }

		inline State GetState() const { return m_State; }
		inline Camera& GetCamera() { return m_Camera; }
		inline Ref<UniformBuffer> GetBuffer() { return m_CameraUniform; }

		static Ref<EditorCamera> Create(Ref<Viewport> viewport);

	private:
		void UpdateArcBall(float deltaTime);
		void UpdateFlyCam(float deltaTime);

		bool OnMouseScroll(MouseScrolledEvent& e);

	private:
		Ref<Viewport> m_Viewport = nullptr;
		Ref<UniformBuffer> m_CameraUniform = nullptr;

		Camera m_Camera = {};

		// Main
		State m_State = State::ArcBall;
		float m_FOV = 45.0f;

		float m_Near = 0.1f;
		float m_Far = 1000.0f;

		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };

		// Flycam
		glm::vec3 m_Front = { 0.0f, 0.0f, -1.0f };
		glm::vec3 m_Up = { 0.0f, 1.0f, 0.0f };
		glm::vec3 m_Right = { 1.0f, 0.0f, 0.0f };

		float m_Yaw = 0.0f;
		float m_Pitch = 0.0f;

		float m_MovementSpeed = 5.0f;
		float m_MouseSensitivity = 0.1f;

		bool m_FirstUpdate = true;
		
		// ArcBall
		float m_Radius = 4.0f;
		float m_Change = 0.5f;

		float m_Speed = 0.005f;
	};

}