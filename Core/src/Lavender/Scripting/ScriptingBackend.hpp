#pragma once

#include "Lavender/Core/Core.hpp"
#include "Lavender/Utils/Utils.hpp"

#include "Lavender/WorkSpace/WorkSpace.hpp"

namespace Lavender
{

	struct ScriptingSpecification
	{
	public:
		WorkSpace::ScriptingBackendType Type = WorkSpace::ScriptingBackendType::None;

		// Path is a dll for C++ and C# and a directory for Python
		std::filesystem::path Path = {};

	public:
		ScriptingSpecification() = default;
		ScriptingSpecification(WorkSpace::ScriptingBackendType type, const std::filesystem::path& path);
		virtual ~ScriptingSpecification() = default;
	};

	// Kind of a virtual class to be able to hold all backend types in a single handle
	class ScriptingBackend
	{
	public:
		ScriptingBackend() = default;
		virtual ~ScriptingBackend() = default;

		virtual void Reload() = 0;
		
		virtual void OnCreateAll() = 0;
		virtual void OnUpdateAll(float deltaTime) = 0;

		virtual void AddInstance(const std::string& classname, const UUID& entity) = 0;
		virtual void RemoveInstance(const std::string& classname, const UUID& entity) = 0;

		virtual std::vector<std::string> GetClasses() = 0;
		virtual ScriptingSpecification& GetSpecification() = 0;
		virtual WorkSpace::ScriptingBackendType GetBackendType() const = 0;
	
		static Ref<ScriptingBackend> Create(const ScriptingSpecification& specs);
		static Ref<ScriptingBackend> Get();
	};

}