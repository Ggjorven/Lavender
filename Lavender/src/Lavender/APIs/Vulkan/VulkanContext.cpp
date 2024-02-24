#include "lvpch.h"
#include "VulkanContext.hpp"

#include "Lavender/Core/Logging.hpp"
#include "Lavender/Core/Application.hpp"

#include "Lavender/Utils/Utils.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

	if (func != nullptr)
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);

	return VK_ERROR_EXTENSION_NOT_PRESENT;
}

static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

	if (func != nullptr)
		func(instance, debugMessenger, pAllocator);
}

static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
	{
		LV_LOG_WARN("Validation layer: {0}", pCallbackData->pMessage);
		return VK_FALSE;
	}

	return VK_FALSE;
}

namespace Lavender
{

	VulkanContext::VulkanContext()
	{
	}

	VulkanContext::~VulkanContext()
	{
	}

	void VulkanContext::Init()
	{
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Instance Creation
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Lavender Application";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 2, 0);
		appInfo.pEngineName = "Lavender Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 2, 0);
		appInfo.apiVersion = VK_API_VERSION_1_2;

		// TODO(Jorben): GLFW can handle this
		#if defined(LV_PLATFORM_WINDOWS)
			#define VK_KHR_WIN32_SURFACE_EXTENSION_NAME "VK_KHR_win32_surface"
		#elif defined(LV_PLATFORM_LINUX)
			#define VK_KHR_WIN32_SURFACE_EXTENSION_NAME "VK_KHR_xcb_surface"
		#endif
			
		std::vector<const char*> instanceExtensions = { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME };
		if (s_Validation)
		{
			instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME); // Very little performance hit, can be used in Release.
			instanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
			instanceExtensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
		}

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		#if defined(LV_PLATFORM_MACOS)
		createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
		#endif
		createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
		createInfo.ppEnabledExtensionNames = instanceExtensions.data();

		if (s_Validation)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(s_RequestedValidationLayers.size());
			createInfo.ppEnabledLayerNames = s_RequestedValidationLayers.data();
		}
		else
		{
			createInfo.enabledLayerCount = 0;
		}

		// Note(Jorben): Setup the debug messenger also for the create instance 
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
		debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugCreateInfo.pfnUserCallback = VulkanDebugCallback;

		if (s_Validation)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(s_RequestedValidationLayers.size());
			createInfo.ppEnabledLayerNames = s_RequestedValidationLayers.data();

			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		}
		else
		{
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;
		}

		if (vkCreateInstance(&createInfo, nullptr, &m_VulkanInstance) != VK_SUCCESS)
			LV_LOG_FATAL("Failed to create Vulkan Instance.");

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Debugger Creation
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if (s_Validation)
		{
			if (CreateDebugUtilsMessengerEXT(m_VulkanInstance, &debugCreateInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS)
				LV_LOG_ERROR("Failed to set up debug messenger!");
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Surface Creation
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		GLFWwindow* handle = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		if (glfwCreateWindowSurface(m_VulkanInstance, handle, nullptr, &m_Surface) != VK_SUCCESS)
			LV_LOG_ERROR("Failed to create window surface!");

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Physical device selection
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		m_PhysicalDevice = VulkanPhysicalDevice::Select();

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Logical device creation
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		m_Device = VulkanDevice::Create(m_PhysicalDevice);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Swapchain creation
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		auto& window = Application::Get().GetWindow();

		m_SwapChain = VulkanSwapChain::Create(m_VulkanInstance, m_Device);
		m_SwapChain->Init(window.GetWidth(), window.GetHeight(), window.IsVSync());
	}

	void VulkanContext::Destroy()
	{
		vkDeviceWaitIdle(m_Device->GetVulkanDevice());

		// TODO: Destroy swapchain
		m_SwapChain->Destroy();

		m_Device->Destroy();

		if (s_Validation)
			DestroyDebugUtilsMessengerEXT(m_VulkanInstance, m_DebugMessenger, nullptr);
		
		vkDestroySurfaceKHR(m_VulkanInstance, m_Surface, nullptr);
		vkDestroyInstance(m_VulkanInstance, nullptr);
	}

}