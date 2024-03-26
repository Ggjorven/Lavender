#include "lvpch.h"
#include "EntityInterface.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Platforms/Windows/WindowsEntityInterface.hpp"

namespace Lavender
{

	Ref<EntityInterface> EntityInterface::Create(Entity& entity, Ref<ScriptLoader> loader, const std::string& classname)
	{
		#if defined(LV_PLATFORM_WINDOWS)
		return RefHelper::Create<WindowsEntityInterface>(entity, loader, classname);
		#endif
	}

	bool ScriptableEntityFunctions::Validate() const
	{
		if (Create == nullptr)
		{
			LV_LOG_WARN("Create function has invalid handle.");
			return false;
		}

		if (Destroy == nullptr)
		{
			LV_LOG_WARN("Destroy function has invalid handle.");
			return false;
		}

		if (OnCreate == nullptr)
		{
			LV_LOG_WARN("OnCreate function has invalid handle.");
			return false;
		}

		if (OnUpdate == nullptr)
		{
			LV_LOG_WARN("OnUpdate function has invalid handle.");
			return false;
		}

		if (GetVariableList == nullptr)
		{
			LV_LOG_WARN("GetVariableList function has invalid handle.");
			return false;
		}

		if (GetUUID == nullptr)
		{
			LV_LOG_WARN("GetUUID function has invalid handle.");
			return false;
		}

		if (SetUUID == nullptr)
		{
			LV_LOG_WARN("SetUUID function has invalid handle.");
			return false;
		}

		return true;
	}

	void ScriptableEntityFunctions::Clear()
	{
		Create = nullptr;
		Destroy = nullptr;
		OnCreate = nullptr;
		OnUpdate = nullptr;
		GetVariableList = nullptr;
		GetUUID = nullptr;
		SetUUID = nullptr;
	}

}