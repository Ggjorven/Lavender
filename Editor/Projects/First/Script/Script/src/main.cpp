#include <Lavender/Scripting/Logger.hpp>
#include <Lavender/Scripting/Input.hpp>
#include <Lavender/Scripting/ScriptableEntity.hpp>


using namespace Lavender;

class MyEntity : public ScriptableEntity
{
public:
	void OnCreate() override
	{
		m_Tag = &AddComponent<TagComponent>(TagComponent("New Tag"));
		m_Tranform = &AddComponent<TransformComponent>(TransformComponent());

		ScriptLogger::LogMessage(ScriptLogger::Level::Trace, "Wagwan G");
	}

	void OnUpdate(float deltaTime) override
	{
		if (Input::IsKeyPressed(Key::W))
		{
			ScriptLogger::LogMessage(ScriptLogger::Level::Trace, "W was pressed {0}", A);
		}
	}

private:
	TagComponent* m_Tag = {};
	TransformComponent* m_Tranform = {};

	float A = 1.0f;

public:
	LV_DEFINE_PRIVATE_VARIABLE(float, A)
};

LV_ENTITY(MyEntity)

LV_PRIVATE_VARIABLE(MyEntity, float, A)