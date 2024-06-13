#include "lvpch.h"
#include "WorkSpace.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/Material.hpp"

#include "Lavender/WorkSpace/Project.hpp"

namespace Lavender::WorkSpace
{

	// Overall
	std::string ConfigurationToString(Configuration config)
	{
		switch (config)
		{
		case Configuration::Debug: return "Debug";
		case Configuration::Release: return "Release";
		case Configuration::Dist: return "Dist";

		default:
			APP_LOG_ERROR("Invalid Configuration passed in.");
			break;
		}

		return {};
	}

	std::string PlatformToString(Platform platform, bool firstUpperCase)
	{
		if (firstUpperCase)
		{
			switch (platform)
			{
			case Platform::Windows: return "Windows";
			case Platform::MacOS: return "MacOS";
			case Platform::Linux: return "Linux";
			case Platform::Android: return "Android";

			default:
				APP_LOG_ERROR("Invalid Platform passed in.");
				break;
			}
		}
		else
		{
			switch (platform)
			{
			case Platform::Windows: return "windows";
			case Platform::MacOS: return "macos";
			case Platform::Linux: return "linux";
			case Platform::Android: return "android";

			default:
				APP_LOG_ERROR("Invalid Platform passed in.");
				break;
			}
		}

		return {};
	}

	std::string ScriptingBackendTypeToString(ScriptingBackendType type)
	{
		switch (type)
		{
		case ScriptingBackendType::Cpp: return "C++";
		case ScriptingBackendType::Cs: return "C#";
		case ScriptingBackendType::Python: return "Python";

		default:
			APP_LOG_ERROR("Invalid Configuration passed in.");
			break;
		}

		return {};
	}



	// Engine Initialization
	void Init()
	{
		Material::Init();
	}

	void Destroy()
	{
		Material::Destroy();

		// Release the static project handle
		Project::Get().reset();
		Project::Get() = nullptr;
	}

}