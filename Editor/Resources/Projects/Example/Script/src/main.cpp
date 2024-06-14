#include <Lavender/Scripting/C++/Mutual/ScriptableEntity.hpp>
#include <Lavender/Scripting/C++/Mutual/ScriptLogger.hpp>
#include <Lavender/Scripting/C++/Mutual/ScriptInput.hpp>

#include <iostream>

using namespace Lavender;

class MyEntity : public Lavender::ScriptableEntity
{
public:
	void OnCreate() override
	{
		ScriptLogger::Log(ScriptLogger::Level::Trace, "OnCreate: {0}", (uint64_t)m_UUID);

		if (HasComponent<TagComponent>())
		{
			TagComponent& tag = GetComponent<TagComponent>();
			ScriptLogger::Log(ScriptLogger::Level::Trace, "Tag: {0}", tag.Tag);

			tag.Tag = "Script Tag??";
		}
	}

	void OnUpdate(float deltaTime) override
	{
		ScriptLogger::Log(ScriptLogger::Level::Trace, "A is pressed == {0}", ScriptInput::IsKeyPressed(Key::A));
	}
} LavenderEntity(MyEntity);