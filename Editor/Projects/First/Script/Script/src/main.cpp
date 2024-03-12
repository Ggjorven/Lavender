#include <Lavender/Scripting/ScriptableEntity.hpp>

#include <iostream>

class MyEntity : public Lavender::ScriptableEntity
{
public:
	void OnCreate() override
	{
		std::cout << "OnCreate MyEntity, UUID: " << m_UUID << std::endl;
		AddOrReplaceComponent<Lavender::TagComponent>(Lavender::TagComponent("New Tag"));

		if (HasComponent<Lavender::TagComponent>())
		{
			std::cout << "Has Tag Component." << std::endl;
		}
	}

	void OnUpdate(float deltaTime) override
	{
		std::cout << "OnUpdate MyEntity, Time: " << deltaTime << std::endl;
	}

public:
	int A = 9;
};

LV_ENTITY(MyEntity)

LV_VARIABLE(MyEntity, int, A)