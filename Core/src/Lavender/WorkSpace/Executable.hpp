#pragma once

#include "Lavender/Core/Core.hpp"
#include "Lavender/Utils/Utils.hpp"

#if defined(APP_PLATFORM_WINDOWS)
#include <windows.h>
#include <tlhelp32.h>
#elif defined(APP_PLATFORM_LINUX)
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#endif

namespace Lavender
{

	// Used by editor for launching the game.
	class Executable
	{
	public:
		Executable() = default;
		Executable(const std::filesystem::path& pathToLaunch, const std::string& args = "");
		~Executable();

		bool IsRunning();

		void Start(const std::filesystem::path& pathToLaunch, const std::string& args = "");
		void Close();

	private:
        bool m_IsRunning = false;

        std::filesystem::path m_Path = {};
		std::string m_Args = {};

#if defined(APP_PLATFORM_WINDOWS)
		PROCESS_INFORMATION m_ProcessInfo = {};
#elif defined(APP_PLATFORM_LINUX)
		pid_t m_ProcessInfo = {};
#endif

    };

}