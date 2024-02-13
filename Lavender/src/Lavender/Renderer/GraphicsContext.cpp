#include "lvpch.h"
#include "GraphicsContext.hpp"

#include "Lavender/Core/Application.hpp"
#include "Lavender/Core/Logging.hpp"

namespace Lavender
{

	GraphicsContext::GraphicsContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
	}

	GraphicsContext::~GraphicsContext()
	{
	}

	void GraphicsContext::Init()
	{
	}

	void GraphicsContext::Destroy()
	{
	}

}
