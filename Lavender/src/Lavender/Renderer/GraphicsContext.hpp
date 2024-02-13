#pragma once

#include <memory>

#include <GLFW/glfw3.h>

namespace Lavender
{

	class GraphicsContext
	{
	public:
		GraphicsContext(GLFWwindow* windowHandle);
		virtual ~GraphicsContext();

		void Init();
		void Destroy();

	private:
		GLFWwindow* m_WindowHandle = nullptr;
	};

}