#include <Lavender/Scripting/Logger.hpp>
#include <Lavender/Scripting/Input.hpp>
#include <Lavender/Scripting/ScriptableEntity.hpp>

using namespace Lavender;

class Ray : public ScriptableEntity
{
public:
	void OnCreate() override
	{
		m_Transform = &GetComponent<TransformComponent>();
	}

	void OnUpdate(float deltaTime) override
	{
		UpdateMovement(deltaTime);
		HandleJumps(deltaTime);
	}

private:
	void UpdateMovement(float deltaTime)
	{
		static float speed = 2.0f;

		if (Input::IsKeyPressed(Key::W))
			m_Transform->Position.z += speed * deltaTime;
		if (Input::IsKeyPressed(Key::S))
			m_Transform->Position.z -= speed * deltaTime;
		if (Input::IsKeyPressed(Key::A))
			m_Transform->Position.x -= speed * deltaTime;
		if (Input::IsKeyPressed(Key::D))
			m_Transform->Position.x += speed * deltaTime;
	}

	void HandleJumps(float deltaTime)
	{
		if (!m_Jumping && Input::IsKeyPressed(Key::Space))
		{
			X = 0.0f;
			m_Jumping = true;
		}

		if (m_Jumping)
		{
			X += deltaTime;
			m_Transform->Position.y = -(16.0f / 3.0f) * (X * X) + 8 * X;

			// Stop jumping when below 0.0f
			if (m_Transform->Position.y <= 0.0f)
			{
				m_Transform->Position.y = 0.0f;
				m_Jumping = false;
			}
		}
	}

public:
	TransformComponent* m_Transform = nullptr;

	float X = 0.0f;
	bool m_Jumping = false;
};

LV_ENTITY(Ray)