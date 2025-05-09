#include "lvpch.h"
#include "ScriptLoader.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Platforms/Windows/WindowsScriptLoader.hpp"

namespace Lavender
{

	Ref<ScriptLoader> ScriptLoader::Create(const std::filesystem::path& filepath)
	{
		#if defined(LV_PLATFORM_WINDOWS)
		return RefHelper::Create<WindowsScriptLoader>(filepath);
		#endif
	}

}