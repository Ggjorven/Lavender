#pragma once

#include <memory>

#include "Lavender/APIs/Vulkan/Setup/VulkanInstance.hpp"
#include "Lavender/APIs/Vulkan/Setup/VulkanSwapChain.hpp"
#include "Lavender/APIs/Vulkan/Setup/VulkanResources.hpp"

#include <GLFW/glfw3.h>

namespace Lavender
{

	class VulkanHelper;

	class GraphicsContext
	{
	public:
		inline static GraphicsContext* Get() { return s_Instance; }
		
		GraphicsContext(GLFWwindow* windowHandle, bool vsync = false);
		virtual ~GraphicsContext();

		void Init();
		void Destroy();

		VulkanInstance*& GetInstance() { return m_Instance; }
		VulkanSwapChain*& GetSwapChain() { return m_SwapChain; }
		VulkanResources*& GetResources() { return m_Resources; }

		GLFWwindow* GetWindowHandle() { return m_WindowHandle; }

	private:
		static GraphicsContext* s_Instance;

		GLFWwindow* m_WindowHandle = nullptr;

		VulkanInstance* m_Instance = nullptr;
		VulkanSwapChain* m_SwapChain = nullptr;
		VulkanResources* m_Resources = nullptr;

		// Note(Jorben): this is needed for setting up the swapchain which happens when creating a window,
		// but since the window has not been created yet, we can't ask if the window is VSync.
		bool m_CreationVSync = false;

		friend class VulkanHelper;
	};

}