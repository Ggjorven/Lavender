#include "lvpch.h"
#include "ScriptingBackend.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Scripting/C++/CppBackend.hpp"

namespace Lavender
{

	ScriptingSpecification::ScriptingSpecification(WorkSpace::ScriptingBackendType type, const std::filesystem::path& path)
		: Type(type), Path(path)
	{
	}

	Ref<ScriptingBackend> ScriptingBackend::Create(const ScriptingSpecification& specs)
	{
		switch (specs.Type)
		{
		case WorkSpace::ScriptingBackendType::Cpp:
			return RefHelper::Create<CppBackend>(specs);

		case WorkSpace::ScriptingBackendType::Cs:		// TODO
		case WorkSpace::ScriptingBackendType::Python:	// TODO
		default:
			APP_LOG_ERROR("Invalid ScriptingBackendType passed in.");
			break;
		}

		return nullptr;
	}

}