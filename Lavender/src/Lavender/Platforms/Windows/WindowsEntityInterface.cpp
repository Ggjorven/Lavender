#include "lvpch.h"
#include "WindowsEntityInterface.hpp"

#include "Lavender/Core/Application.hpp"
#include "Lavender/Core/Logging.hpp"
#include "Lavender/Utils/Utils.hpp"
#include "Lavender/Utils/Profiler.hpp"

#include "Lavender/ECS/Entity.hpp"

#include "Lavender/Platforms/Windows/WindowsScriptLoader.hpp"

#include "Lavender/Workspace/Project.hpp"

namespace Lavender
{

	WindowsEntityInterface::WindowsEntityInterface(const UUID& uuid, Ref<ScriptLoader> loader, const std::string& classname)
		: m_Loader(RefHelper::RefAs<WindowsScriptLoader>(loader)), m_ClassName(classname)
	{
		m_Entity = Entity::Create(Project::Get()->GetSceneCollection().GetActive()->GetCollection(), uuid, false);
		Reload();
	}

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
			LV_LOG_ERROR("Failed to initialize entity ({0}) since script is detached.", m_ClassName);
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
			LV_LOG_ERROR("Failed to reload entity ({0}) interface. ClassName is probably invalid.", m_ClassName);
			return;
		}

		// Create instance
		m_Instance = m_Functions.Create();
		m_Functions.SetUUID(m_Instance, m_Entity.GetUUID().Get());

		InitVariableFunctions();
	}

	void WindowsEntityInterface::InvokeOnCreate()
	{
		LV_PROFILE_SCOPE("WindowsEntityInterface::OnCreate");
		if (m_Functions.OnCreate) 
			m_Functions.OnCreate(m_Instance);
	}

	void WindowsEntityInterface::InvokeOnUpdate(float deltaTime)
	{
		LV_PROFILE_SCOPE("WindowsEntityInterface::OnUpdate");
		if (m_Functions.OnUpdate)
			m_Functions.OnUpdate(m_Instance, deltaTime);
	}

	bool WindowsEntityInterface::HasVariable(const std::string& name)
	{
		auto i = m_Variables.find(name);
		return i != m_Variables.end();
	}

	void WindowsEntityInterface::InitVariableFunctions()
	{
		VariableList list = *m_Functions.GetVariableList();

		for (auto& var : list.Variables)
		{
			// Quick message
			LV_LOG_TRACE("(Script) Found Variable '{0}' of type {1} in {2}", var.Name, VariableInfo::VariableTypeToString(var.Type), m_ClassName);

			// Actual function retrieval
			EntityVariableFunctions functions = { };

			switch (var.Type)
			{
			case VariableType::Char:
			{
				std::string fnName = "Script_GetValueOf" + var.Name + m_ClassName;
				GetCharFn getter = (GetCharFn)GetProcAddress(m_Loader->GetHandle(), fnName.c_str());
				functions.Get = (void*)getter;

				fnName = "Script_SetValueOf" + var.Name + m_ClassName;
				SetCharFn setter = (SetCharFn)GetProcAddress(m_Loader->GetHandle(), fnName.c_str());
				functions.Set = (void*)setter;
				break;
			}
			case VariableType::SChar:
			{
				std::string fnName = "Script_GetValueOf" + var.Name + m_ClassName;
				GetSCharFn getter = (GetSCharFn)GetProcAddress(m_Loader->GetHandle(), fnName.c_str());
				functions.Get = (void*)getter;

				fnName = "Script_SetValueOf" + var.Name + m_ClassName;
				SetSCharFn setter = (SetSCharFn)GetProcAddress(m_Loader->GetHandle(), fnName.c_str());
				functions.Set = (void*)setter;
				break;
			}
			case VariableType::UChar:
			{
				std::string fnName = "Script_GetValueOf" + var.Name + m_ClassName;
				GetUCharFn getter = (GetUCharFn)GetProcAddress(m_Loader->GetHandle(), fnName.c_str());
				functions.Get = (void*)getter;

				fnName = "Script_SetValueOf" + var.Name + m_ClassName;
				SetUCharFn setter = (SetUCharFn)GetProcAddress(m_Loader->GetHandle(), fnName.c_str());
				functions.Set = (void*)setter;
				break;
			}
			case VariableType::Short:
			{
				std::string fnName = "Script_GetValueOf" + var.Name + m_ClassName;
				GetShortFn getter = (GetShortFn)GetProcAddress(m_Loader->GetHandle(), fnName.c_str());
				functions.Get = (void*)getter;

				fnName = "Script_SetValueOf" + var.Name + m_ClassName;
				SetShortFn setter = (SetShortFn)GetProcAddress(m_Loader->GetHandle(), fnName.c_str());
				functions.Set = (void*)setter;
				break;
			}
			case VariableType::UShort:
			{
				std::string fnName = "Script_GetValueOf" + var.Name + m_ClassName;
				GetUShortFn getter = (GetUShortFn)GetProcAddress(m_Loader->GetHandle(), fnName.c_str());
				functions.Get = (void*)getter;

				fnName = "Script_SetValueOf" + var.Name + m_ClassName;
				SetUShortFn setter = (SetUShortFn)GetProcAddress(m_Loader->GetHandle(), fnName.c_str());
				functions.Set = (void*)setter;
				break;
			}
			case VariableType::Int:
			{
				std::string fnName = "Script_GetValueOf" + var.Name + m_ClassName;
				GetIntFn getter = (GetIntFn)GetProcAddress(m_Loader->GetHandle(), fnName.c_str());
				functions.Get = (void*)getter;

				fnName = "Script_SetValueOf" + var.Name + m_ClassName;
				SetIntFn setter = (SetIntFn)GetProcAddress(m_Loader->GetHandle(), fnName.c_str());
				functions.Set = (void*)setter;
				break;
			}
			case VariableType::UInt:
			{
				std::string fnName = "Script_GetValueOf" + var.Name + m_ClassName;
				GetUIntFn getter = (GetUIntFn)GetProcAddress(m_Loader->GetHandle(), fnName.c_str());
				functions.Get = (void*)getter;

				fnName = "Script_SetValueOf" + var.Name + m_ClassName;
				SetUIntFn setter = (SetUIntFn)GetProcAddress(m_Loader->GetHandle(), fnName.c_str());
				functions.Set = (void*)setter;
				break;
			}
			case VariableType::Long:
			{
				std::string fnName = "Script_GetValueOf" + var.Name + m_ClassName;
				GetLongFn getter = (GetLongFn)GetProcAddress(m_Loader->GetHandle(), fnName.c_str());
				functions.Get = (void*)getter;

				fnName = "Script_SetValueOf" + var.Name + m_ClassName;
				SetLongFn setter = (SetLongFn)GetProcAddress(m_Loader->GetHandle(), fnName.c_str());
				functions.Set = (void*)setter;
				break;
			}
			case VariableType::ULong:
			{
				std::string fnName = "Script_GetValueOf" + var.Name + m_ClassName;
				GetULongFn getter = (GetULongFn)GetProcAddress(m_Loader->GetHandle(), fnName.c_str());
				functions.Get = (void*)getter;

				fnName = "Script_SetValueOf" + var.Name + m_ClassName;
				SetULongFn setter = (SetULongFn)GetProcAddress(m_Loader->GetHandle(), fnName.c_str());
				functions.Set = (void*)setter;
				break;
			}
			case VariableType::LongLong:
			{
				std::string fnName = "Script_GetValueOf" + var.Name + m_ClassName;
				GetLongLongFn getter = (GetLongLongFn)GetProcAddress(m_Loader->GetHandle(), fnName.c_str());
				functions.Get = (void*)getter;

				fnName = "Script_SetValueOf" + var.Name + m_ClassName;
				SetLongLongFn setter = (SetLongLongFn)GetProcAddress(m_Loader->GetHandle(), fnName.c_str());
				functions.Set = (void*)setter;
				break;
			}
			case VariableType::ULongLong:
			{
				std::string fnName = "Script_GetValueOf" + var.Name + m_ClassName;
				GetULongLongFn getter = (GetULongLongFn)GetProcAddress(m_Loader->GetHandle(), fnName.c_str());
				functions.Get = (void*)getter;

				fnName = "Script_SetValueOf" + var.Name + m_ClassName;
				SetULongLongFn setter = (SetULongLongFn)GetProcAddress(m_Loader->GetHandle(), fnName.c_str());
				functions.Set = (void*)setter;
				break;
			}
			case VariableType::Float:
			{
				std::string fnName = "Script_GetValueOf" + var.Name + m_ClassName;
				GetFloatFn getter = (GetFloatFn)GetProcAddress(m_Loader->GetHandle(), fnName.c_str());
				functions.Get = (void*)getter;

				fnName = "Script_SetValueOf" + var.Name + m_ClassName;
				SetFloatFn setter = (SetFloatFn)GetProcAddress(m_Loader->GetHandle(), fnName.c_str());
				functions.Set = (void*)setter;
				break;
			}
			case VariableType::Double:
			{
				std::string fnName = "Script_GetValueOf" + var.Name + m_ClassName;
				GetDoubleFn getter = (GetDoubleFn)GetProcAddress(m_Loader->GetHandle(), fnName.c_str());
				functions.Get = (void*)getter;

				fnName = "Script_SetValueOf" + var.Name + m_ClassName;
				SetDoubleFn setter = (SetDoubleFn)GetProcAddress(m_Loader->GetHandle(), fnName.c_str());
				functions.Set = (void*)setter;
				break;
			}
			case VariableType::LongDouble:
			{
				std::string fnName = "Script_GetValueOf" + var.Name + m_ClassName;
				GetLongDoubleFn getter = (GetLongDoubleFn)GetProcAddress(m_Loader->GetHandle(), fnName.c_str());
				functions.Get = (void*)getter;

				fnName = "Script_SetValueOf" + var.Name + m_ClassName;
				SetLongDoubleFn setter = (SetLongDoubleFn)GetProcAddress(m_Loader->GetHandle(), fnName.c_str());
				functions.Set = (void*)setter;
				break;
			}
			case VariableType::Bool:
			{
				std::string fnName = "Script_GetValueOf" + var.Name + m_ClassName;
				GetBoolFn getter = (GetBoolFn)GetProcAddress(m_Loader->GetHandle(), fnName.c_str());
				functions.Get = (void*)getter;

				fnName = "Script_SetValueOf" + var.Name + m_ClassName;
				SetBoolFn setter = (SetBoolFn)GetProcAddress(m_Loader->GetHandle(), fnName.c_str());
				functions.Set = (void*)setter;
				break;
			}

			default:
				LV_LOG_WARN("(WindowsEntityInterface::InitVariableFunctions) Variable type is not supported.");
				break;
			}

			m_Variables[var.Name] = functions;
		}
	}

}