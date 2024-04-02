#include "lvpch.h"
#include "Camera.hpp"

#include "Lavender/Core/Logging.hpp"
#include "Lavender/Core/Input/Input.hpp"

#include "Lavender/Utils/Profiler.hpp"

#include "Lavender/Renderer/Renderer.hpp"
#include "Lavender/Renderer/Pipeline.hpp"
#include "Lavender/Renderer/DescriptorSet.hpp"
#include "Lavender/Renderer/FrameResources.hpp"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Lavender
{

	std::string EditorCamera::StateToString(State state)
	{
		switch (state)
		{
		case State::ArcBall:
			return "ArcBall";
		case State::FlyCam:
			return "FlyCam";

		default:
			LV_LOG_ERROR("No proper camera state selected");
			break;
		}

		return "Invalid Selection";
	}

	EditorCamera::EditorCamera(Ref<Viewport> viewport)
		: m_Viewport(viewport)
	{
		m_CameraUniform = UniformBuffer::Create(sizeof(Camera));
	}

	EditorCamera::~EditorCamera()
	{
	}

	void EditorCamera::OnUpdate(float deltaTime)
	{
		switch (m_State)
		{
		case State::ArcBall:
			UpdateArcBall(deltaTime);
			break;
		case State::FlyCam:
			UpdateFlyCam(deltaTime);
			break;

		default:
			LV_LOG_ERROR("No proper camera state selected");
			break;
		}

		UpdateAndUpload();
	}


	void EditorCamera::OnEvent(Event& e)
	{
		EventHandler handler(e);

		handler.Handle<MouseScrolledEvent>(LV_BIND_EVENT_FN(EditorCamera::OnMouseScroll));
	}

	void EditorCamera::UpdateAndUpload()
	{
		m_CameraUniform->SetData((void*)&m_Camera, sizeof(Camera));

		auto pipeline = FrameResources::GetPipeline();
		auto element = pipeline->GetSpecification().Uniformlayout.GetElementByName(2, "u_Camera");
		auto set = pipeline->GetDescriptorSets()->GetSets(2)[0];
		m_CameraUniform->Upload(set, element); 
	}

	void EditorCamera::BindDescriptorSet(Ref<Pipeline> pipeline, Ref<RenderCommandBuffer> cmdBuffer)
	{
		LV_PROFILE_SCOPE("EditorCamera::BindDescriptorSet");
		// Note(Jorben): This is the Project/View (Camera) set
		auto set = pipeline->GetDescriptorSets()->GetSets(2)[0];
		set->Bind(pipeline, cmdBuffer);
	}

	void EditorCamera::SwitchState()
	{
		switch (m_State)
		{
		case State::ArcBall:
			m_State = State::FlyCam;

			// Reset camera
			glm::vec3 direction = glm::normalize(glm::vec3(0.0f) - m_Position);
			m_Yaw = glm::degrees(atan2(direction.z, direction.x));
			m_Pitch = glm::degrees(asin(direction.y));
			break;
		case State::FlyCam:
			m_State = State::ArcBall;
			break;

		default:
			LV_LOG_ERROR("No proper camera state selected");
			break;
		}
	}

	Ref<EditorCamera> EditorCamera::Create(Ref<Viewport> viewport)
	{
		return RefHelper::Create<EditorCamera>(viewport);
	}

	void EditorCamera::UpdateArcBall(float deltaTime)
	{
		if (m_Viewport->GetWidth() != 0 && m_Viewport->GetHeight() != 0)
		{
			static float theta = 0.0f;
			static float phi = 0.0f;

			static glm::vec2 lastPosition = { 0.0f, 0.0f };
			glm::vec2 position = Input::GetMousePosition();

			if (m_Viewport->InView(Input::GetMousePosition()) && Input::IsMousePressed(MouseButton::Right))
			{
				glm::vec2 delta = position - lastPosition;

				float dTheta = delta.x * m_Speed;
				float dPhi = delta.y * m_Speed;

				theta += dTheta;
				phi += dPhi;

				float epsilon = 0.001f; // To prevent the camera from flipping
				phi = glm::clamp(phi, -glm::half_pi<float>() + epsilon, glm::half_pi<float>() - epsilon);
			}

			lastPosition = position;

			m_Position = glm::vec3(
				m_Radius * glm::sin(theta) * glm::cos(phi),
				m_Radius * glm::sin(phi),
				m_Radius * glm::cos(theta) * glm::cos(phi)
			);

			m_Camera.View = glm::lookAt(m_Position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

			m_Camera.Projection = glm::perspective(glm::radians(m_FOV), (float)m_Viewport->GetWidth() / (float)m_Viewport->GetHeight(), m_Near, m_Far);
			if (Renderer::GetAPI() == RenderingAPI::Vulkan)
				m_Camera.Projection[1][1] *= -1;
		}
	}

	void EditorCamera::UpdateFlyCam(float deltaTime)
	{
		if (m_Viewport->GetWidth() != 0 && m_Viewport->GetHeight() != 0)
		{
			if (m_Viewport->InView(Input::GetMousePosition()) && Input::IsMousePressed(MouseButton::Right))
			{
				float velocity = m_MovementSpeed * deltaTime;
				glm::vec3 moveDirection = { 0.0f, 0.0f, 0.0f };

				// Calculate forward/backward and left/right movement.
				if (Input::IsKeyPressed(Key::W))
					moveDirection += m_Front;
				if (Input::IsKeyPressed(Key::S))
					moveDirection -= m_Front;
				if (Input::IsKeyPressed(Key::A))
					moveDirection += m_Right;
				if (Input::IsKeyPressed(Key::D))
					moveDirection -= m_Right;

				// Calculate up/down movement.
				if (Input::IsKeyPressed(Key::Space))
					moveDirection += m_Up;
				if (Input::IsKeyPressed(Key::LeftShift))
					moveDirection -= m_Up;

				if (glm::length(moveDirection) > 0.0f)
					moveDirection = glm::normalize(moveDirection);

				// Update the camera position.
				static glm::vec2 lastMousePosition = { 0.0f, 0.0f };
				m_Position += moveDirection * velocity;
				if (m_FirstUpdate)
				{
					lastMousePosition = Input::GetMousePosition();
					m_FirstUpdate = false;
				}

				// Mouse movement
				glm::vec2 mousePosition = Input::GetMousePosition();
				float xOffset = static_cast<float>(mousePosition.x - lastMousePosition.x);
				float yOffset = static_cast<float>(lastMousePosition.y - mousePosition.y);

				//Reset cursor
				Input::SetCursorPosition({ m_Viewport->GetWidth() / 2.0f + (float)m_Viewport->GetPosition().x, m_Viewport->GetHeight() / 2.0f + (float)m_Viewport->GetPosition().y });
				Input::SetCursorMode(CursorMode::Disabled);

				lastMousePosition.x = static_cast<float>(m_Viewport->GetWidth() / 2.f) + (float)m_Viewport->GetPosition().x;
				lastMousePosition.y = static_cast<float>(m_Viewport->GetHeight() / 2.f) + (float)m_Viewport->GetPosition().y;

				xOffset *= m_MouseSensitivity;
				yOffset *= m_MouseSensitivity;

				//Set new settings
				m_Yaw -= xOffset;
				m_Pitch += yOffset;

				// Cap movement
				if (m_Pitch > 89.0f)
					m_Pitch = 89.0f;
				if (m_Pitch < -89.0f)
					m_Pitch = -89.0f;
			}
			else
			{
				Input::SetCursorMode(CursorMode::Shown);
				m_FirstUpdate = true;
			}

			glm::vec3 newFront(1.0f);
			newFront.x = glm::cos(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch));
			newFront.y = glm::sin(glm::radians(m_Pitch));
			newFront.z = glm::sin(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch));

			m_Front = glm::normalize(newFront);
			m_Right = glm::normalize(glm::cross(m_Front, m_Up));

			// Update everything
			m_Camera.View = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
			m_Camera.Projection = glm::perspective(glm::radians(m_FOV), (float)m_Viewport->GetWidth() / (float)m_Viewport->GetHeight(), m_Near, m_Far);
			if (Renderer::GetAPI() == RenderingAPI::Vulkan)
				m_Camera.Projection[1][1] *= -1;
		}
	}

	bool EditorCamera::OnMouseScroll(MouseScrolledEvent& e)
	{
		if (Input::IsMousePressed(MouseButton::Right))
		{
			float change = m_Change * e.GetYOffset();
			m_Radius -= change;
		}

		return false;
	}

}