#pragma once

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/ECS/Entity.hpp"
#include "Lavender/Scripting/EntityInterface.hpp"
#include "Lavender/Platforms/Windows/WindowsScriptLoader.hpp"

#include <Windows.h>

namespace Lavender
{

	class WindowsEntityInterface : public EntityInterface
	{
	public:
		WindowsEntityInterface(const UUID& uuid, Ref<ScriptLoader> loader, const std::string& classname);
		WindowsEntityInterface(Entity& entity, Ref<ScriptLoader> loader, const std::string& classname);
		virtual ~WindowsEntityInterface();

		void Reload() override;

		void InvokeOnCreate() override;
		void InvokeOnUpdate(float deltaTime) override;

		inline Dict<std::string, EntityVariableFunctions>& GetVariables() { return m_Variables; }
		bool HasVariable(const std::string& name) override;

		template<typename T>
		void Set(const std::string& name, T value);
		template<>
		void Set<float>(const std::string& name, float value);

		template<typename T>
		T Get(const std::string& name);
		template<>
		float Get<float>(const std::string& name);

		inline Entity& GetEntity() { return m_Entity; }

	private:
		void InitVariableFunctions();

	private:
		Ref<WindowsScriptLoader> m_Loader = nullptr;
		std::string m_ClassName = "Unset Class";

		Entity m_Entity = {};

		ScriptableEntity* m_Instance = nullptr;
		ScriptableEntityFunctions m_Functions = {};

		Dict<std::string, EntityVariableFunctions> m_Variables = { };
	};

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Setters // TODO: Add all types
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template<typename T>
	inline void WindowsEntityInterface::Set(const std::string& name, T value)
	{
		LV_LOG_WARN("(WindowsEntityInterface::Set) Variable type not supported.");
	}

	template<>
	inline void WindowsEntityInterface::Set(const std::string& name, float value)
	{
		SetFloatFn SetFP = (SetFloatFn)m_Variables[name].Set;
		SetFP(m_Instance, value);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Getters // TODO: Add all types
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template<typename T>
	inline T WindowsEntityInterface::Get(const std::string& name)
	{
		LV_LOG_WARN("(WindowsEntityInterface::Get) Variable type not supported.");
		return T();
	}

	template<>
	inline float WindowsEntityInterface::Get(const std::string& name)
	{
		GetFloatFn GetFP = (GetFloatFn)m_Variables[name].Get;
		return GetFP(m_Instance);
	}

}