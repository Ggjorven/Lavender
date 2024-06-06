#pragma once

#include <Lavender/Utils/UUID.hpp>
#include <Lavender/ECS/Components.hpp>

namespace Lavender
{

	// Functions on the Engine side
	typedef void*	(*AddComponentFn)			(ComponentType, UUID, void*);
	typedef void*	(*AddOrReplaceComponentFn)	(ComponentType, UUID, void*);
	typedef bool	(*HasComponentFn)			(ComponentType, UUID);
	typedef void*	(*GetComponentFn)			(ComponentType, UUID);
	typedef void	(*RemoveComponentFn)		(ComponentType, UUID);

	// Gets set when a dll is loaded, these functions are used on the User side
	struct ComponentFunctions
	{
	public:
		inline static AddComponentFn			Add = nullptr;
		inline static AddOrReplaceComponentFn	AddOrReplace = nullptr;
		inline static HasComponentFn			Has = nullptr;
		inline static GetComponentFn			Get = nullptr;
		inline static RemoveComponentFn			Remove = nullptr;

	public:
		inline static bool Initialized()
		{
			return (Add && AddOrReplace && Has && Get && Remove);
		}
	};


	// This gets exported from the Dll from the User side
	typedef void (*Internal_SetComponentFunctionsFn)	(AddComponentFn, AddOrReplaceComponentFn, HasComponentFn, GetComponentFn, RemoveComponentFn);

#if defined(LAVENDER_DLL)
	extern "C" inline __declspec(dllexport) void Lavender_SetComponentFunctions(AddComponentFn add, AddOrReplaceComponentFn addOrReplace, HasComponentFn has, GetComponentFn get, RemoveComponentFn remove)
	{
		ComponentFunctions::Add = add;
		ComponentFunctions::AddOrReplace = addOrReplace;
		ComponentFunctions::Has = has;
		ComponentFunctions::Get = get;
		ComponentFunctions::Remove = remove;
	}
#endif

}