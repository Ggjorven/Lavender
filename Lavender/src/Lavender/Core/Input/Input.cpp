#include "lvpch.h"
#include "Input.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Platforms/Windows/WindowsInput.hpp"

namespace Lavender
{

	Input* Input::s_Instance = nullptr;

	void Input::Init()
	{
		#ifdef LV_PLATFORM_WINDOWS
		s_Instance = new WindowsInput();
		#endif
	}

	void Input::Destroy()
	{
		delete s_Instance;
		s_Instance = nullptr;
	}

}