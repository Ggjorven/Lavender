#include <Lavender/Scripting/C++/Mutual/ScriptableEntity.hpp>

using namespace Lavender;

class MyEntity : public Lavender::ScriptableEntity
{
public:
	void OnCreate() override
	{

	}

	void OnUpdate(float deltaTime) override
	{

	}
} LavenderEntity(MyEntity);