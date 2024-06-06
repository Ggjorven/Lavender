#include "lvpch.h"
#include "CppFunctions.hpp"

#include "Lavender/Core/Logging.hpp"

namespace Lavender
{

	void CppFunctions::OutSource(Ref<Insight::Dll> dll)
	{
		Internal_SetComponentFunctionsFn func = dll->GetCustomFunction<Internal_SetComponentFunctionsFn>("Lavender_SetComponentFunctions");

		func(&CppFunctions::Internal_AddComponent, &CppFunctions::Internal_AddOrReplaceComponent, &CppFunctions::Internal_HasComponent, &CppFunctions::Internal_GetComponent, &CppFunctions::Internal_RemoveComponent);
	}

}