#include "lvpch.h"
#include "Window.hpp"

#include "Lavender/Platforms/Windows/WindowsWindow.hpp"

namespace Lavender
{

	std::unique_ptr<Window> Window::Create(const WindowSpecification& properties)
	{
		#ifdef APP_PLATFORM_WINDOWS
		return std::make_unique<WindowsWindow>(properties);
		#endif

		// TODO: Add all the platforms
	}

}