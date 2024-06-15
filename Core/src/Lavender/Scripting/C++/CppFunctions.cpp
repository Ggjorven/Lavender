#include "lvpch.h"
#include "CppFunctions.hpp"

#include "Lavender/Core/Logging.hpp"

namespace Lavender
{

	void CppFunctions::OutSource(Ref<Insight::Dll> dll)
	{
		Script::Internal_SetComponentFunctionsFn compFunc = dll->GetCustomFunction<Script::Internal_SetComponentFunctionsFn>("Lavender_SetComponentFunctions");
		compFunc(&CppFunctions::Internal_AddComponent, &CppFunctions::Internal_AddOrReplaceComponent, &CppFunctions::Internal_HasComponent, &CppFunctions::Internal_GetComponent, &CppFunctions::Internal_RemoveComponent);

		Script::Internal_SetLoggingFunctionsFn logFunc = dll->GetCustomFunction<Script::Internal_SetLoggingFunctionsFn>("Lavender_SetLoggingFunctions");
		logFunc(&CppFunctions::Internal_LogMessage);

		Script::Internal_SetInputFunctionsFn inputFunc = dll->GetCustomFunction<Script::Internal_SetInputFunctionsFn>("Lavender_SetInputFunctions");
		inputFunc(&CppFunctions::Internal_IsKeyPressed, &CppFunctions::Internal_IsMousePressed, &CppFunctions::Internal_GetCursorPosition, &CppFunctions::Internal_SetCursorPosition, &CppFunctions::Internal_SetCursorMode);
	}

}