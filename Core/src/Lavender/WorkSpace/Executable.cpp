#include "lvpch.h"
#include "Executable.hpp"

#include "Lavender/Core/Logging.hpp"

namespace Lavender
{

	Executable::Executable(const std::filesystem::path& pathToLaunch, const std::string& args)
		: m_Path(pathToLaunch), m_Args(args)
	{
        Start(pathToLaunch, args);
	}

	Executable::~Executable()
    {
        Close();
	}

	bool Executable::IsRunning()
	{
#if defined(APP_PLATFORM_WINDOWS)
        if (m_ProcessInfo.hProcess)
        {
            DWORD status = {};
            if (GetExitCodeProcess(m_ProcessInfo.hProcess, &status) && status == STILL_ACTIVE)
                return true;
        }
#else
        if (m_ProcessInfo) // TODO: Check if this works on linux
        {
            if (waitpid(m_ProcessInfo, NULL, WNOHANG) == 0)
                return true;
        }
#endif

        m_IsRunning = false;
        return m_IsRunning;
	}

    void Executable::Start(const std::filesystem::path& pathToLaunch, const std::string& args)
    {
        m_Path = pathToLaunch;
        m_Args = args;

#if defined(APP_PLATFORM_WINDOWS)
        STARTUPINFO si = {};
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&m_ProcessInfo, sizeof(m_ProcessInfo));

        std::wstring wideCommand = Utils::ToolKit::WideString(fmt::format("{0} {1}", pathToLaunch.string(), args));

        if (!CreateProcess(NULL, wideCommand.data(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &m_ProcessInfo))
        {
            APP_LOG_ERROR("CreateProcess failed ({0})", GetLastError());
            return;
        }
#elif defined(APP_PLATFORM_LINUX)
        m_ProcessInfo = fork();
        if (m_ProcessInfo == -1)
        {
            APP_LOG_ERROR("Fork failed.");
            return;
        }
        if (m_ProcessInfo == 0)
        {
            execl("/bin/sh", "sh", "-c", (pathToLaunch.string() + " " + args).c_str(), (char*)NULL);
            _exit(EXIT_FAILURE);
        }
#endif

        m_IsRunning = true;
    }

    void Executable::Close()
    {
        if (IsRunning())
        {
#if defined(APP_PLATFORM_WINDOWS)
            TerminateProcess(m_ProcessInfo.hProcess, 0);
            CloseHandle(m_ProcessInfo.hProcess);
            CloseHandle(m_ProcessInfo.hThread);
#elif defined(APP_PLATFORM_LINUX)
            kill(m_ProcessInfo, SIGKILL);
#endif
        }
    }

}