#pragma once

#include "Lavender/Core/Core.hpp"
#include "Lavender/Utils/Utils.hpp"

namespace Lavender
{

	enum class ScriptingBackendType
	{
		None = 0, Cpp, Cs, Python // Cs and Python are not a thing yet. // TODO
	};

	struct ScriptingSpecification
	{
	public:
		ScriptingBackendType Type = ScriptingBackendType::Cpp;

		// Path is a dll for C++ and C# and a directory for Python
		std::filesystem::path Path = {};

	public:
		ScriptingSpecification() = default;
		ScriptingSpecification(ScriptingBackendType type, const std::filesystem::path& path);
		virtual ~ScriptingSpecification() = default;
	};

	// Kind of a virtual class to be able to hold all backend types in a single handle
	class ScriptingBackend
	{
	public:
		ScriptingBackend() = default;
		virtual ~ScriptingBackend() = default;

		virtual void Reload() = 0;
		
		virtual ScriptingSpecification& GetSpecification() = 0;
		virtual ScriptingBackendType GetBackendType() const = 0;
	
		static Ref<ScriptingBackend> Create(const ScriptingSpecification& specs);
		static Ref<ScriptingBackend> Get();
	};

}