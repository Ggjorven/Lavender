#include "lvpch.h"
#include "WindowsScriptLoader.hpp"

#include "Lavender/Core/Logging.hpp"

namespace Lavender
{

	WindowsScriptLoader::WindowsScriptLoader(const std::filesystem::path& filepath)
	{
		m_Handle = LoadLibraryA(filepath.string().c_str());
		if (!IsValid())
		{
			LV_LOG_ERROR("Failed to load DLL");
		}
	}

	WindowsScriptLoader::~WindowsScriptLoader()
	{
		FreeLibrary(m_Handle);
	}

}