#include "lvpch.h"
#include "WindowsEntityInterface.hpp"

#include "Lavender/Core/Application.hpp"
#include "Lavender/Core/Logging.hpp"
#include "Lavender/Utils/Utils.hpp"
#include "Lavender/Utils/Profiler.hpp"

#include "Lavender/ECS/Entity.hpp"

#include "Lavender/Platforms/Windows/WindowsScriptLoader.hpp"

namespace Lavender
{

	WindowsEntityInterface::WindowsEntityInterface(Entity& entity, Ref<ScriptLoader> loader, const std::string& classname)
		: m_Entity(entity), m_Loader(RefHelper::RefAs<WindowsScriptLoader>(loader)), m_ClassName(classname)
	{
		Reload();
	}

	WindowsEntityInterface::~WindowsEntityInterface()
	{
		if (m_Loader->IsDetached())
			return;

		m_Functions.Destroy(m_Instance);
		m_Functions.Clear();
		m_Instance = nullptr;
	}

	void WindowsEntityInterface::Reload()
	{
		if (m_Loader->IsDetached())
		{
			LV_LOG_ERROR("Failed to initialize entity ({0})...", m_ClassName);
			return;
		}

		std::string fnName = std::string("Script_CreateEntity") + m_ClassName;
		m_Functions.Create = (CreateFn)GetProcAddress(m_Loader->GetHandle(), fnName.c_str());

		fnName = std::string("Script_DestroyEntity") + m_ClassName;
		m_Functions.Destroy = (DestroyFn)GetProcAddress(m_Loader->GetHandle(), fnName.c_str());

		fnName = std::string("Script_OnCreateEntity") + m_ClassName;
		m_Functions.OnCreate = (OnCreateFn)GetProcAddress(m_Loader->GetHandle(), fnName.c_str());

		fnName = std::string("Script_OnUpdateEntity") + m_ClassName;
		m_Functions.OnUpdate = (OnUpdateFn)GetProcAddress(m_Loader->GetHandle(), fnName.c_str());

		fnName = std::string("Script_GetVariables") + m_ClassName;
		m_Functions.GetVariableList = (GetVariableListFn)GetProcAddress(m_Loader->GetHandle(), fnName.c_str());

		fnName = std::string("Script_GetUUID") + m_ClassName;
		m_Functions.GetUUID = (GetUUIDFn)GetProcAddress(m_Loader->GetHandle(), fnName.c_str());

		fnName = std::string("Script_SetUUID") + m_ClassName;
		m_Functions.SetUUID = (SetUUIDFn)GetProcAddress(m_Loader->GetHandle(), fnName.c_str());

		if (!m_Functions.Validate())
		{
			LV_LOG_ERROR("Failed to reload entity ({0}) interface.", m_ClassName);
			return;
		}

		// Create instance
		m_Instance = m_Functions.Create();
		m_Functions.SetUUID(m_Instance, m_Entity.GetUUID().Get());
	}

	void WindowsEntityInterface::InvokeOnCreate()
	{
		LV_PROFILE_SCOPE("WindowsEntityInterface::OnCreate");
		m_Functions.OnCreate(m_Instance);
	}

	void WindowsEntityInterface::InvokeOnUpdate(float deltaTime)
	{
		LV_PROFILE_SCOPE("WindowsEntityInterface::OnUpdate");
		m_Functions.OnUpdate(m_Instance, deltaTime);
	}

}