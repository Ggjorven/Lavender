#pragma once

#include "Lavender/Core/Core.hpp"
#include "Lavender/Core/Events.hpp"
#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Renderer/Buffers.hpp"
#include "Lavender/Renderer/Pipeline.hpp"
#include "Lavender/Renderer/CommandBuffer.hpp"

#include "Lavender/ECS/Camera.hpp"

#include <glm/glm.hpp>

namespace Lavender
{

	class EditorCamera : public Camera
	{
	public:
		enum class State
		{
			None = 0, ArcBall, FlyCam
		};
		static std::string StateToString(State state);
	public:
		EditorCamera();
		virtual ~EditorCamera();

		void OnUpdate(float deltaTime) override;
		void OnEvent(Event& e) override;

		void SwitchState();

		inline float& GetFOV() { return m_FOV; }
		inline float& GetFlyCamSpeed() { return m_MovementSpeed; }
		inline float& GetArcBallSpeed() { return m_Speed; }

		inline State GetState() const { return m_State; }

		static Ref<EditorCamera> Create();

	private:
		void UpdateArcBall(float deltaTime);
		void UpdateFlyCam(float deltaTime);

		bool OnMouseScroll(MouseScrolledEvent& e);

	private:
		// Main
		State m_State = State::ArcBall;

		// Flycam
		float m_MovementSpeed = 5.0f;
		float m_MouseSensitivity = 0.1f;

		bool m_FirstUpdate = true;
		
		// ArcBall
		float m_Radius = 4.0f;
		float m_Change = 0.5f;

		float m_Speed = 0.005f;
	};

}