#include "Input.hpp"

#include "Lavender/Scripting/Logger.hpp"

namespace Lavender
{

	Input* Input::s_Instance = nullptr;

}

// Non-user exported functions
extern "C"
{

	EXPORT void Script_SetInputInstance(Lavender::Input* instance)
	{
		Lavender::Input::SetInstance(instance);
	}

	EXPORT void Script_RemoveInputInstance()
	{
		Lavender::Input::NullInstance();
	}

	// Note(Jorben): For some reason when adding this to the Logger.cpp file it doesn't export // TODO: Fix this issue
	EXPORT void Script_InitScriptLogger(std::shared_ptr<spdlog::logger>* logger)
	{
		Lavender::ScriptLogger::Init(logger);
	}

}