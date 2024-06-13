#include <Lavender/Scripting/C++/Mutual/ScriptableEntity.hpp>

#include <iostream>

using namespace Lavender;

class MyEntity : public Lavender::ScriptableEntity
{
public:
	void OnCreate() override
	{
		std::cout << "OnCreate: " << (uint64_t)m_UUID << std::endl;
	}

	void OnUpdate(float deltaTime) override
	{

	}
} LavenderEntity(MyEntity);