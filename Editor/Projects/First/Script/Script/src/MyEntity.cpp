#include <Lavender/Scripting/Logger.hpp>
#include <Lavender/Scripting/Input.hpp>
#include <Lavender/Scripting/ScriptableEntity.hpp>

using namespace Lavender;

class MyEntity : public ScriptableEntity
{
public:
	void OnCreate() override
	{
		ScriptLogger::LogMessage(ScriptLogger::Level::Trace, "MyEntity OnCreate");
	}

	void OnUpdate(float deltaTime) override
	{
		TransformComponent& transform = GetComponent<TransformComponent>();

		float speed = 1.0f;

		if (Input::IsKeyPressed(Key::W))
			transform.Position.z += speed * deltaTime;
		if (Input::IsKeyPressed(Key::S))
			transform.Position.z -= speed * deltaTime;
		if (Input::IsKeyPressed(Key::A))
			transform.Position.x -= speed * deltaTime;
		if (Input::IsKeyPressed(Key::D))
			transform.Position.x += speed * deltaTime;
	}

private:
	TagComponent* m_Tag = {};
	TransformComponent* m_Tranform = {};

public:
	float A = 1.0f;
};

LV_ENTITY(MyEntity)

LV_VARIABLE(MyEntity, float, A)