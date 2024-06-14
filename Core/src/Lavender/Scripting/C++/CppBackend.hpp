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

	struct ScriptCache
	{
	public:
		OnCreateFn OnCreate = nullptr;
		OnUpdateFn OnUpdate = nullptr;
		GetUUIDFn GetUUID = nullptr;

	public:
		ScriptCache() = default;
		virtual ~ScriptCache() = default;
	};
	
	
	
	class CppBackend : public ScriptingBackend
	{
	public:
		CppBackend(const ScriptingSpecification& specs);
		virtual ~CppBackend();

		void Reload() override;

		void OnCreateAll() override;
		void OnUpdateAll(float deltaTime) override;

		void AddInstance(const std::string& classname, const UUID& entity) override;
		void RemoveInstance(const std::string& classname, const UUID& entity) override;

		std::vector<std::string> GetClasses() override;
		inline ScriptingSpecification& GetSpecification() override { return m_Specification; }
		inline Ref<Insight::Dll> GetDll() { return m_Dll; }
		inline Dict<UUID, ScriptEntityInfo>& GetInstances() { return m_Instances; }

		inline WorkSpace::ScriptingBackendType GetBackendType() const override { return WorkSpace::ScriptingBackendType::Cpp; }

	private:
		void CopyOver();

	private:
		ScriptingSpecification m_Specification = {};
		std::filesystem::path m_CopyPath = {};

		Ref<Insight::Dll> m_Dll = {};
		ScriptCache m_Cache = {};

		Dict<UUID, ScriptEntityInfo> m_Instances = { };
	};

}