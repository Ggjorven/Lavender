#include "lvpch.h"
#include "WindowsScriptLoader.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Core/Input/Input.hpp"

namespace Lavender
{

	WindowsScriptLoader::WindowsScriptLoader(const std::filesystem::path& filepath)
		: m_Path(filepath)
	{
		if (!std::filesystem::exists(filepath))
		{
			LV_LOG_ERROR("Script path '{0}' doesn't exist.", filepath.string());
			m_Detached = true;
			return;
		}

		CopyOver();
		Load();
	}

	WindowsScriptLoader::~WindowsScriptLoader()
	{
		Detach();
	}

	void WindowsScriptLoader::Reload()
	{
		Detach();
		CopyOver();
		Load();
	}

	void WindowsScriptLoader::Detach()
	{
		if (m_Detached)
			return;

		// Destroy pointers
		std::string fnName = std::string("Script_RemoveInputInstance");
		RemoveInputInstanceFn removeInputInstanceFP = (RemoveInputInstanceFn)GetProcAddress(m_Handle, fnName.c_str());
		removeInputInstanceFP();

		FreeLibrary(m_Handle);
		m_Handle = nullptr;
		m_Detached = true;
	}

	void WindowsScriptLoader::Load()
	{
		m_Detached = false;

		m_Handle = LoadLibraryA((m_NewPath / m_Path.filename()).string().c_str());
		if (!IsValid())
		{
			LV_LOG_ERROR("Failed to load DLL: '{0}'", (m_NewPath / m_Path.filename()).string());
			return;
		}

		// Load functions and set pointers
		std::string fnName = std::string("Script_SetInputInstance");
		SetInputInstanceFn setInputInstanceFP = (SetInputInstanceFn)GetProcAddress(m_Handle, fnName.c_str());
		setInputInstanceFP(Input::GetInstance());

		fnName = std::string("Script_InitScriptLogger");
		InitLoggerFn initLoggerFP = (InitLoggerFn)GetProcAddress(m_Handle, fnName.c_str());
		initLoggerFP(&Log::GetLogger());
	}

	void WindowsScriptLoader::CopyOver()
	{
		// Note(Jorben): We have so many copies of paths since most std::filesystem functions not only return the outcome but apply it to the parameter/variable
		std::filesystem::path oldPath = m_Path;
		oldPath = oldPath.remove_filename();

		std::filesystem::path newPath = m_Path;
		m_NewPath = newPath.remove_filename();
		m_NewPath = m_NewPath / "Engine-Script";
		
		std::filesystem::create_directory(m_NewPath);

		std::filesystem::path dllOld = m_Path;
		std::filesystem::path dllNew = m_NewPath / m_Path.filename();

		// Note(Jorben): PDB is used for debugging.
		std::filesystem::path pdbOld = oldPath / m_Path.filename();
		pdbOld = pdbOld.replace_extension(".pdb");
		std::filesystem::path pdbNew = m_NewPath / m_Path.filename();
		pdbNew = pdbNew.replace_extension(".pdb");

		std::filesystem::copy_file(m_Path, (m_NewPath / m_Path.filename()), std::filesystem::copy_options::overwrite_existing);
		
		if (std::filesystem::exists(pdbOld))
			std::filesystem::copy_file(pdbOld, pdbNew, std::filesystem::copy_options::overwrite_existing);

		// Check if PDB file is present
		if (!std::filesystem::exists(pdbNew) && !std::filesystem::exists(pdbOld))
			LV_LOG_WARN("Failed to find PDB file for script ({0}), this is not critical. It only means debugging the script is not possible.", m_Path.string());

		// Note(Jorben): We delete the old one, since the debugger likes to use that one instead of the copied one and then we can't change the .pdb file while the app is running a.k.a we have to detach before we can compile our script.
		std::filesystem::remove(pdbOld);
	}

}