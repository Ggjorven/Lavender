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
		}

		if (HasComponent<TransformComponent>())
		{
			m_Transform = GetComponent<TransformComponent>();
		}
		else
		{
			m_Transform = AddComponent<TransformComponent>();
		}
	}

	void OnUpdate(float deltaTime) override
	{
		//Logger::Log(Logger::Level::Trace, "A is pressed == {0}", Input::IsKeyPressed(Key::A));

		static float speed = 1.0f;

		if (Input::IsKeyPressed(Key::W))
			m_Transform.Position->z -= speed * deltaTime;
		if (Input::IsKeyPressed(Key::S))
			m_Transform.Position->z += speed * deltaTime;
		if (Input::IsKeyPressed(Key::A))
			m_Transform.Position->x -= speed * deltaTime;
		if (Input::IsKeyPressed(Key::D))
			m_Transform.Position->x += speed * deltaTime;
	}
private:
	TransformComponent m_Transform = {};
} LavenderEntity(MyEntity);
