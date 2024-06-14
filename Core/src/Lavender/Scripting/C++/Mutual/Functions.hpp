#pragma once

#include <Lavender/Utils/UUID.hpp>

#include <Lavender/ECS/Components.hpp>

#include <Lavender/Core/Input/KeyCodes.hpp>
#include <Lavender/Core/Input/MouseCodes.hpp>

namespace Lavender
{

	// Functions on the Engine side
	///////////////////////////////////////////////////////////////////////////////
	// Components
	///////////////////////////////////////////////////////////////////////////////
	typedef void*		(*AddComponentFn)			(ComponentType, UUID, void*);
	typedef void*		(*AddOrReplaceComponentFn)	(ComponentType, UUID, void*);
	typedef bool		(*HasComponentFn)			(ComponentType, UUID);
	typedef void*		(*GetComponentFn)			(ComponentType, UUID);
	typedef void		(*RemoveComponentFn)		(ComponentType, UUID);

	///////////////////////////////////////////////////////////////////////////////
	// Logging
	///////////////////////////////////////////////////////////////////////////////
	typedef void		(*ScriptLogMessageFn)		(uint8_t, const char*);

	///////////////////////////////////////////////////////////////////////////////
	// Input
	///////////////////////////////////////////////////////////////////////////////
	typedef bool		(*IsKeyPressedFn)			(Key);
	typedef bool		(*IsMousePressedFn)			(MouseButton);
	typedef glm::vec2*	(*GetCursorPositionFn)		();
	typedef void		(*SetCursorPositionFn)		(glm::vec2*);
	typedef void		(*SetCursorModeFn)			(CursorMode);

	// Gets set when a dll is loaded, these functions are used on the User side
	///////////////////////////////////////////////////////////////////////////////
	// Components
	///////////////////////////////////////////////////////////////////////////////
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

	///////////////////////////////////////////////////////////////////////////////
	// Logging
	///////////////////////////////////////////////////////////////////////////////
	struct LoggingFunctions
	{
	public:
		inline static ScriptLogMessageFn		LogMessage = nullptr;

	public:
		inline static bool Initialized()
		{
			return (LogMessage);
		}
	};

	///////////////////////////////////////////////////////////////////////////////
	// Input
	///////////////////////////////////////////////////////////////////////////////
	struct InputFunctions
	{
	public:
		inline static IsKeyPressedFn			IsKeyPressed = nullptr;
		inline static IsMousePressedFn			IsMousePressed = nullptr;
		inline static GetCursorPositionFn		GetCursorPosition = nullptr;
		inline static SetCursorPositionFn		SetCursorPosition = nullptr;
		inline static SetCursorModeFn			SetCursorMode = nullptr;

	public:
		inline static bool Initialized()
		{
			return (IsKeyPressed && IsMousePressed && GetCursorPosition && SetCursorPosition && SetCursorMode);
		}
	};

	// This gets exported from the Dll from the User side
	typedef void (*Internal_SetComponentFunctionsFn)	(AddComponentFn, AddOrReplaceComponentFn, HasComponentFn, GetComponentFn, RemoveComponentFn);
	typedef void (*Internal_SetLoggingFunctionsFn)		(ScriptLogMessageFn);
	typedef void (*Internal_SetInputFunctionsFn)		(IsKeyPressedFn, IsMousePressedFn, GetCursorPositionFn, SetCursorPositionFn, SetCursorModeFn);

#if defined(LAVENDER_DLL)
	extern "C" inline __declspec(dllexport) void Lavender_SetComponentFunctions(AddComponentFn add, AddOrReplaceComponentFn addOrReplace, HasComponentFn has, GetComponentFn get, RemoveComponentFn remove)
	{
		ComponentFunctions::Add = add;
		ComponentFunctions::AddOrReplace = addOrReplace;
		ComponentFunctions::Has = has;
		ComponentFunctions::Get = get;
		ComponentFunctions::Remove = remove;
	}

	extern "C" inline __declspec(dllexport) void Lavender_SetLoggingFunctions(ScriptLogMessageFn logMessage)
	{
		LoggingFunctions::LogMessage = logMessage;
	}

	extern "C" inline __declspec(dllexport) void Lavender_SetInputFunctions(IsKeyPressedFn isKeyPressed, IsMousePressedFn isMousePressed, GetCursorPositionFn getCursorPosition, SetCursorPositionFn setCursorPosition, SetCursorModeFn setCursorMode)
	{
		InputFunctions::IsKeyPressed = isKeyPressed;
		InputFunctions::IsMousePressed = isMousePressed;
		InputFunctions::GetCursorPosition = getCursorPosition;
		InputFunctions::SetCursorPosition = setCursorPosition;
		InputFunctions::SetCursorMode = setCursorMode;
	}
#endif

}