#include "lvpch.h"
#include "ScriptingBackend.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/WorkSpace/Project.hpp"
#include "Lavender/WorkSpace/EngineTracker.hpp"

#include "Lavender/Scripting/C++/CppBackend.hpp"

namespace Lavender
{

	ScriptingSpecification::ScriptingSpecification(WorkSpace::ScriptType type, const std::filesystem::path& path)
		: Type(type), Path(path)
	{
	}

	std::filesystem::path ScriptingBackend::GetPath(WorkSpace::ScriptType type)
	{
		auto& info = Project::Get()->GetInfo();

		switch (type)
		{
		case WorkSpace::ScriptType::Cpp:
			return info.Directory / info.Script / "bin" / fmt::format("{0}-{1}", WorkSpace::ConfigurationToString(Track::Lavender::Config), WorkSpace::PlatformToString(Track::Lavender::Platform, false)) / "Script/Script.dll";

		case WorkSpace::ScriptType::Cs:		// TODO
		case WorkSpace::ScriptType::Python:	// TODO
		default:
			APP_LOG_ERROR("Invalid ScriptType passed in.");
			break;
		}

		return {};
	}

	Ref<ScriptingBackend> ScriptingBackend::Create(const ScriptingSpecification& specs)
	{
		switch (specs.Type)
		{
		case WorkSpace::ScriptType::Cpp:
			return RefHelper::Create<CppBackend>(specs);

		case WorkSpace::ScriptType::Cs:		// TODO
		case WorkSpace::ScriptType::Python:	// TODO
		default:
			APP_LOG_ERROR("Invalid ScriptType passed in.");
			break;
		}

		return nullptr;
	}

}