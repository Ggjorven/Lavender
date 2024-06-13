#include "lvpch.h"
#include "ScriptingBackend.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Scripting/C++/CppBackend.hpp"

namespace Lavender
{

	static Ref<ScriptingBackend> s_Instance = nullptr;

	ScriptingSpecification::ScriptingSpecification(WorkSpace::ScriptingBackendType type, const std::filesystem::path& path)
		: Type(type), Path(path)
	{
	}

	Ref<ScriptingBackend> ScriptingBackend::Create(const ScriptingSpecification& specs)
	{
		switch (specs.Type)
		{
		case WorkSpace::ScriptingBackendType::Cpp:
			s_Instance = RefHelper::Create<CppBackend>(specs);
			return s_Instance;

		case WorkSpace::ScriptingBackendType::Cs:		// TODO
		case WorkSpace::ScriptingBackendType::Python:	// TODO
		default:
			APP_LOG_ERROR("Invalid ScriptingBackendType passed in.");
			break;
		}

		return nullptr;
	}

	Ref<ScriptingBackend> ScriptingBackend::Get()
	{
		return s_Instance;
	}

}