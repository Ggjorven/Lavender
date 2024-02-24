#include "lvpch.h"
#include "Window.hpp"

#include "Lavender/Platforms/Windows/WindowsWindow.hpp"

namespace Lavender
{

	std::unique_ptr<Window> Window::Create()
	{
		#ifdef LV_PLATFORM_WINDOWS
		return std::make_unique<WindowsWindow>();
		#endif

		// TODO(Jorben): Add all the platforms
	}

}