#include "lvpch.h"
#include "GraphicsContext.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/APIs/Vulkan/VulkanManager.hpp"
#include "Lavender/APIs/Vulkan/Setup/VulkanBufferHelper.hpp"

namespace Lavender
{

	GraphicsContext* GraphicsContext::s_Instance = nullptr;

	GraphicsContext::GraphicsContext(GLFWwindow* windowHandle, bool vsync)
		: m_WindowHandle(windowHandle), m_CreationVSync(vsync)
	{
	}

	GraphicsContext::~GraphicsContext()
	{
	}

	void GraphicsContext::Init()
	{
		s_Instance = this;

		VulkanManager::Init();

		m_Instance = new VulkanInstance();
		VulkanManager::PopulateDeviceInfo();

		VulkanBufferHelper::Init();

		m_SwapChain = new VulkanSwapChain();
		VulkanManager::PopulateSwapChainInfo();

		m_Resources = new VulkanResources();
		VulkanManager::PopulateResourceInfo();
	}

	void GraphicsContext::Destroy()
	{

		delete m_SwapChain;
		delete m_Resources;
		VulkanBufferHelper::Destroy();
		delete m_Instance;

		s_Instance = nullptr;
	}

}
