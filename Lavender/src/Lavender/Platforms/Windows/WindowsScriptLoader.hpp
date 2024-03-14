#pragma once

#include <filesystem>

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Scripting/ScriptLoader.hpp"

namespace Lavender
{

	class WindowsScriptLoader : public ScriptLoader
	{
	public:
		WindowsScriptLoader(const std::filesystem::path& filepath);
		virtual ~WindowsScriptLoader();

		void Reload() override;

		void Detach();

		inline bool IsDetached() override { return m_Detached; }
		inline bool IsValid() const { return m_Handle != nullptr; }

		#ifdef LV_PLATFORM_WINDOWS
		inline HMODULE GetHandle() { return m_Handle; }
		#endif

	private:
		void Load();
		void CopyOver();

	private:
		#ifdef LV_PLATFORM_WINDOWS
		HMODULE m_Handle = {};
		#endif
		std::filesystem::path m_Path = {};
		std::filesystem::path m_NewPath = {};

		bool m_Detached = false;
	};

}