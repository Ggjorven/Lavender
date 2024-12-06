#pragma once

#include "Lavender/Core/Core.hpp"
#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Utils/UUID.hpp"

#include <filesystem>

namespace Lavender::WorkSpace
{

	enum class State : uint8_t			{ None = 0, Editor, Runtime };
	enum class Configuration : uint8_t	{ None = 0, Debug, Release, Dist };
	enum class Platform : uint8_t		{ None = 0, Windows, MacOS, Linux, Android };
	enum class ScriptType : uint8_t		{ None = 0, Cpp, Cs, Python };

	std::string ConfigurationToString(Configuration config);
	std::string PlatformToString(Platform platform, bool firstUpperCase = false);
	std::string ScriptTypeToString(ScriptType type);

	// Engine Initialization
	void Init();
	void Destroy();

}