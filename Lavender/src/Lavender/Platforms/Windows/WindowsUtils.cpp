#include "lvpch.h"
#include "WindowsUtils.hpp"

#include "Lavender/Core/Logging.hpp"

#include <GLFW/glfw3.h>

namespace Lavender::Utils
{

	std::unique_ptr<ToolKit> ToolKit::s_Instance = std::make_unique<WindowsToolKit>();

	float WindowsToolKit::GetTimeImpl() const
	{
		return (float)glfwGetTime();
	}

}