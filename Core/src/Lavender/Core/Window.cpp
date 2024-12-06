#include "lvpch.h"
#include "Window.hpp"

#include "Lavender/Platforms/Windows/WindowsWindow.hpp"

namespace Lavender
{

	Unique<Window> Window::Create(const WindowSpecification& properties)
	{
		#ifdef APP_PLATFORM_WINDOWS
		return UniqueHelper::Create<WindowsWindow>(properties);
		#endif

		// TODO: Add all the platforms
	}

}