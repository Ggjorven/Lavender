#pragma once

#include "Lavender/Core/Core.hpp"
#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Scripting/ScriptingBackend.hpp"
#include "Lavender/Scripting/C++/Mutual/ScriptableEntity.hpp"

#include <Insight/Insight.hpp>

namespace Lavender
{

	struct ScriptEntityInfo
	{
	public:
		std::string Name = {};
		ScriptableEntity* Instance = nullptr;

	public:
		ScriptEntityInfo() = default;
		ScriptEntityInfo(const std::string& name, ScriptableEntity* instance);
		virtual ~ScriptEntityInfo() = default;
	};

	/*
	struct ScriptCache
	{
	public:
		Dict<std::string, CreateClassFn> ClassFuncs = { };
		GetUUIDFn UUIDFunc = nullptr;

	public:
		ScriptCache() = default;
		virtual ~ScriptCache() = default;

		inline bool Exists(const std::string& cls) { return (ClassFuncs.find(cls) != ClassFuncs.end()); }
	};
	*/
	
	
	
	class CppBackend : public ScriptingBackend
	{
	public:
		CppBackend(const ScriptingSpecification& specs);
		virtual ~CppBackend();

		void Reload() override;

		void AddInstance(const std::string& classname, const UUID& entity);
		void RemoveInstance(const std::string& classname, const UUID& entity);

		inline ScriptingSpecification& GetSpecification() { return m_Specification; }
		inline Ref<Insight::Dll> GetDll() { return m_Dll; }
		inline Dict<UUID, ScriptEntityInfo>& GetInstances() { return m_Instances; }

		inline ScriptingBackendType GetBackendType() const override { return ScriptingBackendType::Cpp; }

	private:
		ScriptingSpecification m_Specification = {};
		//ScriptCache m_Cache = {};

		Ref<Insight::Dll> m_Dll = {};
		Dict<UUID, ScriptEntityInfo> m_Instances = { };
	};

}