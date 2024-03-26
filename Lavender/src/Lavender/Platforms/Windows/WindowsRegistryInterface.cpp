#include "lvpch.h"
#include "WindowsRegistryInterface.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Scripting/RegistryInterface.hpp"

namespace Lavender
{

	WindowsRegistryInterface::WindowsRegistryInterface(Ref<RegistryCollection> collection, Ref<ScriptLoader> loader)
		: m_Collection(collection), m_Loader(RefHelper::RefAs<WindowsScriptLoader>(loader))
	{
	}

	WindowsRegistryInterface::~WindowsRegistryInterface()
	{
	}

	void WindowsRegistryInterface::Initialize()
	{
		if (m_Loader->IsDetached())
		{
			LV_LOG_ERROR("Failed to initialize ECS Registry functions.");
			return;
		}
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Component functions
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		std::string fnName = std::string("Script_SetAddComponentFP");
		SetAddComponentFn setAddComponentFP = (SetAddComponentFn)GetProcAddress(m_Loader->GetHandle(), fnName.c_str());
		setAddComponentFP(&RegistryInterface::Native_AddComponent);

		fnName = std::string("Script_SetAddOrReplaceComponentFP");
		SetAddOrReplaceComponentFn setAddOrReplaceComponentFP = (SetAddOrReplaceComponentFn)GetProcAddress(m_Loader->GetHandle(), fnName.c_str());
		setAddOrReplaceComponentFP(&RegistryInterface::Native_AddOrReplaceComponent);

		fnName = std::string("Script_SetHasComponentFP");
		SetHasComponentFn setHasComponentFP = (SetHasComponentFn)GetProcAddress(m_Loader->GetHandle(), fnName.c_str());
		setHasComponentFP(&RegistryInterface::Native_HasComponent);

		fnName = std::string("Script_SetGetComponentFP");
		SetGetComponentFn setGetComponentFP = (SetGetComponentFn)GetProcAddress(m_Loader->GetHandle(), fnName.c_str());
		setGetComponentFP(&RegistryInterface::Native_GetComponent);

		fnName = std::string("Script_SetRemoveComponentFP");
		SetRemoveComponentFn setRemoveComponentFP = (SetRemoveComponentFn)GetProcAddress(m_Loader->GetHandle(), fnName.c_str());
		setRemoveComponentFP(&RegistryInterface::Native_RemoveComponent);
	}

}