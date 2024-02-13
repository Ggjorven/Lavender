#include "lvpch.h"
#include "Window.hpp"

#include "Lavender/Platforms/Windows/WindowsWindow.hpp"

namespace Lavender
{

	std::unique_ptr<Window> Window::Create(const WindowProperties properties)
	{
		#ifdef LV_PLATFORM_WINDOWS
		return std::make_unique<WindowsWindow>(properties);
		#endif

		// TODO(Jorben): Add all the platforms
	}

}