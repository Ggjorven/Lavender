#include <Lavender/Scripting/C++/Mutual/Core/Core.hpp>

#include <Lavender/Scripting/C++/Mutual/Core/Logger.hpp>
#include <Lavender/Scripting/C++/Mutual/Input/Input.hpp>
#include <Lavender/Scripting/C++/Mutual/ECS/Entity.hpp>

using namespace Lavender::Script;

class Global : public Entity
{
public:
	void OnCreate() override 
	{
	}

	void OnUpdate(float deltaTime) override
	{
	}
} LavenderEntity(Global);

/*
class GlobalTest : public Entity
{
public:
	void OnCreate() override
	{
	}

	void OnUpdate(float deltaTime) override
	{
	}
} LavenderEntity(GlobalTest);
*/