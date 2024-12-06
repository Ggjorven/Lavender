#pragma once

#include "Lavender/Scripting/C++/Mutual/Utils/UUID.hpp"

#include "Lavender/Scripting/C++/Mutual/Input/KeyCodes.hpp"
#include "Lavender/Scripting/C++/Mutual/Input/MouseCodes.hpp"

#include "Lavender/Scripting/C++/Mutual/ECS/Components.hpp"

#include <glm/glm.hpp>

namespace Lavender::Script
{

	// Functions on the Engine side
	///////////////////////////////////////////////////////////////////////////////
	// Window
	///////////////////////////////////////////////////////////////////////////////
	typedef uint32_t	(*GetWidthFn)				();
	typedef uint32_t	(*GetHeightFn)				();
	typedef uint32_t	(*GetXPositionFn)			();
	typedef uint32_t	(*GetYPositionFn)			();
	typedef uint32_t	(*GetMonitorWidthFn)		();
	typedef uint32_t	(*GetMonitorHeightFn)		();
	typedef void		(*SetVSyncFn)				(bool);
	typedef bool		(*IsVSyncFn)				();
	typedef void		(*SetTitleFn)				(const char*);

	///////////////////////////////////////////////////////////////////////////////
	// Logging
	///////////////////////////////////////////////////////////////////////////////
	typedef void		(*ScriptLogMessageFn)		(uint8_t, const char*);

	///////////////////////////////////////////////////////////////////////////////
	// Tools
	///////////////////////////////////////////////////////////////////////////////
	typedef void		(*ToolsInitFn)				();

	///////////////////////////////////////////////////////////////////////////////
	// Input
	///////////////////////////////////////////////////////////////////////////////
	typedef bool		(*IsKeyPressedFn)			(Key);
	typedef bool		(*IsMousePressedFn)			(MouseButton);
	typedef glm::vec2*	(*GetCursorPositionFn)		();
	typedef void		(*SetCursorPositionFn)		(glm::vec2*);
	typedef void		(*SetCursorModeFn)			(CursorMode);

	///////////////////////////////////////////////////////////////////////////////
	// Components
	///////////////////////////////////////////////////////////////////////////////
	typedef void*		(*AddComponentFn)			(ComponentType, UUID, void*);
	typedef void*		(*AddOrReplaceComponentFn)	(ComponentType, UUID, void*);
	typedef bool		(*HasComponentFn)			(ComponentType, UUID);
	typedef void*		(*GetComponentFn)			(ComponentType, UUID);
	typedef void		(*RemoveComponentFn)		(ComponentType, UUID);

	// Gets set when a dll is loaded, these functions are used on the User side
	///////////////////////////////////////////////////////////////////////////////
	// Window
	///////////////////////////////////////////////////////////////////////////////
	struct WindowFunctions
	{
	public:
		inline static GetWidthFn				GetWidth = nullptr;
		inline static GetHeightFn				GetHeight = nullptr;
		inline static GetXPositionFn			GetXPosition = nullptr;
		inline static GetYPositionFn			GetYPosition = nullptr;
		inline static GetMonitorWidthFn			GetMonitorWidth = nullptr;
		inline static GetMonitorWidthFn			GetMonitorHeight = nullptr;
		inline static SetVSyncFn				SetVSync = nullptr;
		inline static IsVSyncFn					IsVSync = nullptr;
		inline static SetTitleFn				SetTitle = nullptr;

	public:
		inline static bool Initialized()
		{
			return (GetWidth && GetHeight);
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

	// This gets exported from the Dll from the User side
	typedef void (*Internal_SetWindowFunctionsFn)		(GetWidthFn, GetHeightFn, GetXPositionFn, GetYPositionFn, GetMonitorWidthFn, GetMonitorHeightFn, SetVSyncFn, IsVSyncFn, SetTitleFn);
	typedef void (*Internal_SetLoggingFunctionsFn)		(ScriptLogMessageFn);
	typedef void (*Internal_SetInputFunctionsFn)		(IsKeyPressedFn, IsMousePressedFn, GetCursorPositionFn, SetCursorPositionFn, SetCursorModeFn);
	typedef void (*Internal_SetComponentFunctionsFn)	(AddComponentFn, AddOrReplaceComponentFn, HasComponentFn, GetComponentFn, RemoveComponentFn);

#if defined(LAVENDER_DLL)
	extern "C" inline __declspec(dllexport) void Lavender_SetWindowFunctions(GetWidthFn getWidth, GetHeightFn getHeight, GetXPositionFn getXPosition, GetYPositionFn getYPosition, GetMonitorWidthFn getMonitorWidth, GetMonitorHeightFn getMonitorHeight, SetVSyncFn setVSync, IsVSyncFn isVSync, SetTitleFn setTitle)
	{
		WindowFunctions::GetWidth = getWidth;
		WindowFunctions::GetHeight = getHeight;
		WindowFunctions::GetXPosition = getXPosition;
		WindowFunctions::GetYPosition = getYPosition;
		WindowFunctions::GetMonitorWidth = getMonitorWidth;
		WindowFunctions::GetMonitorHeight = getMonitorHeight;
		WindowFunctions::SetVSync = setVSync;
		WindowFunctions::IsVSync = isVSync;
		WindowFunctions::SetTitle = setTitle;
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