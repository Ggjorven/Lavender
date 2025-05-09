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

		inline VariableList GetVariables() override { return *m_Functions.GetVariableList(); }
		inline Dict<std::string, EntityVariableFunctions>& GetVariablesWithFuncions() { return m_Variables; }
		bool HasVariable(const std::string& name) override;

		template<typename T>
		void Set(const std::string& name, T value);
		template<>
		void Set<char>(const std::string& name, char value);
		template<>
		void Set<signed char>(const std::string& name, signed char value);
		template<>
		void Set<unsigned char>(const std::string& name, unsigned char value);
		template<>
		void Set<short>(const std::string& name, short value);
		template<>
		void Set<unsigned short>(const std::string& name, unsigned short value);
		template<>
		void Set<int>(const std::string& name, int value);
		template<>
		void Set<unsigned int>(const std::string& name, unsigned int value);
		template<>
		void Set<long>(const std::string& name, long value);
		template<>
		void Set<unsigned long>(const std::string& name, unsigned long value);
		template<>
		void Set<long long>(const std::string& name, long long value);
		template<>
		void Set<unsigned long long>(const std::string& name, unsigned long long value);
		template<>
		void Set<float>(const std::string& name, float value);
		template<>
		void Set<double>(const std::string& name, double value);
		template<>
		void Set<long double>(const std::string& name, long double value);
		template<>
		void Set<bool>(const std::string& name, bool value);

		template<typename T>
		T Get(const std::string& name);
		template<>
		char Get<char>(const std::string& name);
		template<>
		signed char Get<signed char>(const std::string& name);
		template<>
		unsigned char Get<unsigned char>(const std::string& name);
		template<>
		short Get<short>(const std::string& name);
		template<>
		unsigned short Get<unsigned short>(const std::string& name);
		template<>
		int Get<int>(const std::string& name);
		template<>
		unsigned int Get<unsigned int>(const std::string& name);
		template<>
		long Get<long>(const std::string& name);
		template<>
		unsigned long Get<unsigned long>(const std::string& name);
		template<>
		long long Get<long long>(const std::string& name);
		template<>
		unsigned long long Get<unsigned long long>(const std::string& name);
		template<>
		float Get<float>(const std::string& name);
		template<>
		double Get<double>(const std::string& name);
		template<>
		long double Get<long double>(const std::string& name);
		template<>
		bool Get<bool>(const std::string& name);

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
	// Setters
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template<typename T>
	inline void WindowsEntityInterface::Set(const std::string& name, T value)
	{
		LV_LOG_WARN("(WindowsEntityInterface::Set) Variable type not supported.");
	}

	template<>
	inline void WindowsEntityInterface::Set(const std::string& name, char value)
	{
		SetCharFn SetFP = (SetCharFn)m_Variables[name].Set;
		SetFP(m_Instance, value);
	}

	template<>
	inline void WindowsEntityInterface::Set(const std::string& name, signed char value)
	{
		SetSCharFn SetFP = (SetSCharFn)m_Variables[name].Set;
		SetFP(m_Instance, value);
	}

	template<>
	inline void WindowsEntityInterface::Set(const std::string& name, unsigned char value)
	{
		SetUCharFn SetFP = (SetUCharFn)m_Variables[name].Set;
		SetFP(m_Instance, value);
	}

	template<>
	inline void WindowsEntityInterface::Set(const std::string& name, short value)
	{
		SetShortFn SetFP = (SetShortFn)m_Variables[name].Set;
		SetFP(m_Instance, value);
	}

	template<>
	inline void WindowsEntityInterface::Set(const std::string& name, unsigned short value)
	{
		SetUShortFn SetFP = (SetUShortFn)m_Variables[name].Set;
		SetFP(m_Instance, value);
	}

	template<>
	inline void WindowsEntityInterface::Set(const std::string& name, int value)
	{
		SetIntFn SetFP = (SetIntFn)m_Variables[name].Set;
		SetFP(m_Instance, value);
	}

	template<>
	inline void WindowsEntityInterface::Set(const std::string& name, unsigned int value)
	{
		SetUIntFn SetFP = (SetUIntFn)m_Variables[name].Set;
		SetFP(m_Instance, value);
	}

	template<>
	inline void WindowsEntityInterface::Set(const std::string& name, long value)
	{
		SetLongFn SetFP = (SetLongFn)m_Variables[name].Set;
		SetFP(m_Instance, value);
	}

	template<>
	inline void WindowsEntityInterface::Set(const std::string& name, unsigned long value)
	{
		SetULongFn SetFP = (SetULongFn)m_Variables[name].Set;
		SetFP(m_Instance, value);
	}

	template<>
	inline void WindowsEntityInterface::Set(const std::string& name, long long value)
	{
		SetLongLongFn SetFP = (SetLongLongFn)m_Variables[name].Set;
		SetFP(m_Instance, value);
	}

	template<>
	inline void WindowsEntityInterface::Set(const std::string& name, unsigned long long value)
	{
		SetULongLongFn SetFP = (SetULongLongFn)m_Variables[name].Set;
		SetFP(m_Instance, value);
	}

	template<>
	inline void WindowsEntityInterface::Set(const std::string& name, float value)
	{
		SetFloatFn SetFP = (SetFloatFn)m_Variables[name].Set;
		SetFP(m_Instance, value);
	}

	template<>
	inline void WindowsEntityInterface::Set(const std::string& name, double value)
	{
		SetDoubleFn SetFP = (SetDoubleFn)m_Variables[name].Set;
		SetFP(m_Instance, value);
	}

	template<>
	inline void WindowsEntityInterface::Set(const std::string& name, long double value)
	{
		SetLongDoubleFn SetFP = (SetLongDoubleFn)m_Variables[name].Set;
		SetFP(m_Instance, value);
	}

	template<>
	inline void WindowsEntityInterface::Set(const std::string& name, bool value)
	{
		SetBoolFn SetFP = (SetBoolFn)m_Variables[name].Set;
		SetFP(m_Instance, value);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Getters
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template<typename T>
	inline T WindowsEntityInterface::Get(const std::string& name)
	{
		LV_LOG_WARN("(WindowsEntityInterface::Get) Variable type not supported.");
		return T();
	}

	template<>
	inline char WindowsEntityInterface::Get(const std::string& name)
	{
		GetCharFn GetFP = (GetCharFn)m_Variables[name].Get;
		return GetFP(m_Instance);
	}

	template<>
	inline signed char WindowsEntityInterface::Get(const std::string& name)
	{
		GetSCharFn GetFP = (GetSCharFn)m_Variables[name].Get;
		return GetFP(m_Instance);
	}

	template<>
	inline unsigned char WindowsEntityInterface::Get(const std::string& name)
	{
		GetUCharFn GetFP = (GetUCharFn)m_Variables[name].Get;
		return GetFP(m_Instance);
	}

	template<>
	inline short WindowsEntityInterface::Get(const std::string& name)
	{
		GetShortFn GetFP = (GetShortFn)m_Variables[name].Get;
		return GetFP(m_Instance);
	}

	template<>
	inline unsigned short WindowsEntityInterface::Get(const std::string& name)
	{
		GetUShortFn GetFP = (GetUShortFn)m_Variables[name].Get;
		return GetFP(m_Instance);
	}

	template<>
	inline int WindowsEntityInterface::Get(const std::string& name)
	{
		GetIntFn GetFP = (GetIntFn)m_Variables[name].Get;
		return GetFP(m_Instance);
	}

	template<>
	inline unsigned int WindowsEntityInterface::Get(const std::string& name)
	{
		GetUIntFn GetFP = (GetUIntFn)m_Variables[name].Get;
		return GetFP(m_Instance);
	}

	template<>
	inline long WindowsEntityInterface::Get(const std::string& name)
	{
		GetLongFn GetFP = (GetLongFn)m_Variables[name].Get;
		return GetFP(m_Instance);
	}

	template<>
	inline unsigned long WindowsEntityInterface::Get(const std::string& name)
	{
		GetULongFn GetFP = (GetULongFn)m_Variables[name].Get;
		return GetFP(m_Instance);
	}

	template<>
	inline long long WindowsEntityInterface::Get(const std::string& name)
	{
		GetLongLongFn GetFP = (GetLongLongFn)m_Variables[name].Get;
		return GetFP(m_Instance);
	}

	template<>
	inline unsigned long long WindowsEntityInterface::Get(const std::string& name)
	{
		GetULongLongFn GetFP = (GetULongLongFn)m_Variables[name].Get;
		return GetFP(m_Instance);
	}

	template<>
	inline float WindowsEntityInterface::Get(const std::string& name)
	{
		GetFloatFn GetFP = (GetFloatFn)m_Variables[name].Get;
		return GetFP(m_Instance);
	}

	template<>
	inline double WindowsEntityInterface::Get(const std::string& name)
	{
		GetDoubleFn GetFP = (GetDoubleFn)m_Variables[name].Get;
		return GetFP(m_Instance);
	}

	template<>
	inline long double WindowsEntityInterface::Get(const std::string& name)
	{
		GetLongDoubleFn GetFP = (GetLongDoubleFn)m_Variables[name].Get;
		return GetFP(m_Instance);
	}

	template<>
	inline bool WindowsEntityInterface::Get(const std::string& name)
	{
		GetBoolFn GetFP = (GetBoolFn)m_Variables[name].Get;
		return GetFP(m_Instance);
	}

}