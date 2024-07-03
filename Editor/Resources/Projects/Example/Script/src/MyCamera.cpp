#include <Lavender/Scripting/C++/Mutual/Core/Logger.hpp>
#include <Lavender/Scripting/C++/Mutual/Core/Events.hpp>
#include <Lavender/Scripting/C++/Mutual/Input/Input.hpp>
#include <Lavender/Scripting/C++/Mutual/ECS/Entity.hpp>

#include <Lavender/Scripting/C++/Mutual/Core/Window.hpp>

using namespace Lavender::Script;

class MyCamera : public Entity
{
public:
	void OnCreate() override
	{
		if (HasComponent<TransformComponent>())
			m_Transform = GetComponent<TransformComponent>();
		else
			m_Transform = AddComponent<TransformComponent>();

		if (HasComponent<CameraComponent>())
			m_Camera = GetComponent<CameraComponent>();
		else
			m_Camera = AddComponent<CameraComponent>();
	}

	void OnUpdate(float deltaTime) override
	{
		if (!m_Update)
			return;

		// Get Front, Up & Right
		Vec3 front = Vec3(1.0f);
		front.x = glm::cos(glm::radians(*m_Camera.Yaw)) * glm::cos(glm::radians(*m_Camera.Pitch));
		front.y = glm::sin(glm::radians(*m_Camera.Pitch));
		front.z = glm::sin(glm::radians(*m_Camera.Yaw)) * glm::cos(glm::radians(*m_Camera.Pitch));
		
		front = glm::normalize(front);
		Vec3 up = { 0.0f, 1.0f, 0.0f };
		Vec3 right = glm::normalize(glm::cross(front, up));


		// Update camera logic
		float velocity = m_MovementSpeed * deltaTime;
		glm::vec3 moveDirection = { 0.0f, 0.0f, 0.0f };

		// Calculate forward/backward and left/right movement.
		if (Input::IsKeyPressed(Key::W))
			moveDirection += front;
		if (Input::IsKeyPressed(Key::S))
			moveDirection -= front;
		if (Input::IsKeyPressed(Key::A))
			moveDirection -= right;
		if (Input::IsKeyPressed(Key::D))
			moveDirection += right;

		// Calculate up/down movement.
		if (Input::IsKeyPressed(Key::Space))
			moveDirection += up;
		if (Input::IsKeyPressed(Key::LeftShift))
			moveDirection -= up;

		if (glm::length(moveDirection) > 0.0f)
			moveDirection = glm::normalize(moveDirection);

		// Update the camera position.
		static glm::vec2 lastMousePosition = { 0.0f, 0.0f };
		*m_Transform.Position += (moveDirection * velocity);
		if (m_FirstUpdate)
		{
			lastMousePosition = Input::GetCursorPosition();
			m_FirstUpdate = false;
		}

		// Mouse movement
		glm::vec2 mousePosition = Input::GetCursorPosition();
		float xOffset = static_cast<float>(mousePosition.x - lastMousePosition.x);
		float yOffset = static_cast<float>(lastMousePosition.y - mousePosition.y);

		//Reset cursor
		Input::SetCursorPosition({ Window::GetWidth() / 2.0f + (float)Window::GetXPosition(), Window::GetHeight() / 2.0f + (float)Window::GetYPosition() });
		Input::SetCursorMode(CursorMode::Hidden);

		lastMousePosition.x = static_cast<float>(Window::GetWidth() / 2.f) + (float)Window::GetXPosition();
		lastMousePosition.y = static_cast<float>(Window::GetHeight() / 2.f) + (float)Window::GetYPosition();

		xOffset *= m_MouseSensitivity;
		yOffset *= m_MouseSensitivity;

		//Set new settings
		*m_Camera.Yaw += xOffset;
		*m_Camera.Pitch += yOffset;

		// Cap movement
		if (*m_Camera.Pitch > 89.0f)
			m_Camera.Pitch = 89.0f;
		if (*m_Camera.Pitch < -89.0f)
			m_Camera.Pitch = -89.0f;
	}

	void OnEvent(Event& event) override
	{
		EventHandler handler(event);

		handler.Handle<KeyPressedEvent>([this](KeyPressedEvent& event)
		{
			if (event.GetKeyCode() == Key::Escape)
			{
				Input::SetCursorMode(CursorMode::Shown);

				m_Update = !m_Update;
				m_FirstUpdate = true;
			}

			return false;
		});
	}

private:
	TransformComponent m_Transform = {};
	CameraComponent m_Camera = {};

	// Camera Settings
	bool m_Update = true;
	bool m_FirstUpdate = true;

	float m_MovementSpeed = 5.0f;
	float m_MouseSensitivity = 0.1f;

} LavenderEntity(MyCamera);
