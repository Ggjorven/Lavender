#include <Lavender/Scripting/C++/Mutual/Core/Core.hpp>

#include <Lavender/Scripting/C++/Mutual/Core/Logger.hpp>
#include <Lavender/Scripting/C++/Mutual/Input/Input.hpp>
#include <Lavender/Scripting/C++/Mutual/ECS/Entity.hpp>

using namespace Lavender::Script;

class MyEntity : public Entity
{
public:
	void OnCreate() override
	{
		Logger::Log(Logger::Level::Trace, "OnCreate: {0}", (uint64_t)m_UUID);

		if (HasComponent<TagComponent>())
		{
			TagComponent tag = GetComponent<TagComponent>();
			Logger::Log(Logger::Level::Trace, "Script: Tag: {0}", (std::string)tag.Tag);

			tag.Tag = "New Script Tag??";
		}

		if (HasComponent<TransformComponent>())
		{
			m_Transform = GetComponent<TransformComponent>();
		}
	}

	void OnUpdate(float deltaTime) override
	{
		//Logger::Log(Logger::Level::Trace, "A is pressed == {0}", Input::IsKeyPressed(Key::A));

		m_Transform.Position->x += deltaTime * 0.1f;
	}
private:
	TransformComponent m_Transform = {};
} LavenderEntity(MyEntity);