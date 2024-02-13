#include "lvpch.h"
#include "VulkanInstance.hpp"

#include <GLFW/glfw3.h>

#include "Lavender/Core/Application.hpp"
#include "Lavender/Core/Logging.hpp"

#include "Lavender/APIs/Vulkan/Setup/VulkanHelper.hpp"

#include "Lavender/Renderer/GraphicsContext.hpp"

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

static void SetDebugInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = VulkanDebugCallback;
}

namespace Lavender
{

	VulkanInstance::VulkanInstance()
	{
		CreateInstance();
		CreateDebugger();
		CreateSurface();
		PickPhysicalDevice();
		CreateDevice();
	}

	VulkanInstance::~VulkanInstance()
	{
		// Wait for the logical device to finish it's tasks
		vkDeviceWaitIdle(m_Device);

		vkDestroyDevice(m_Device, nullptr);

		#if BL_VALIDATION_LAYERS
		DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
		#endif

		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
		vkDestroyInstance(m_Instance, nullptr);
	}

	void VulkanInstance::CreateInstance()
	{
		//First run a check for validation layer support
		if (BL_VALIDATION_LAYERS && !VulkanHelper::ValidationLayersSupported())
			LV_LOG_ERROR("Validation layers requested, but not supported.");

		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Lavender Application";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		auto extensions = VulkanHelper::GetRequiredExtensions();

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		//createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;			// Note(Jorben): This line is needed for MacOS

		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		#if BL_VALIDATION_LAYERS
		createInfo.enabledLayerCount = static_cast<uint32_t>(s_RequestedValidationLayers.size());
		createInfo.ppEnabledLayerNames = s_RequestedValidationLayers.data();
		#else
		createInfo.enabledLayerCount = 0;
		#endif

		// Note(Jorben): Setup the debug messenger also for the create instance 
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
		#if BL_VALIDATION_LAYERS
		createInfo.enabledLayerCount = static_cast<uint32_t>(s_RequestedValidationLayers.size());
		createInfo.ppEnabledLayerNames = s_RequestedValidationLayers.data();

		SetDebugInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		#else
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
		#endif

		// Create our vulkan instance
		if (vkCreateInstance(&createInfo, nullptr, &m_Instance) != VK_SUCCESS)
			LV_LOG_FATAL("Failed to create Vulkan Instance.");
	}

	void VulkanInstance::CreateDebugger()
	{
		#if !(BL_VALIDATION_LAYERS)
		return;
		#else
		VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
		SetDebugInfo(createInfo);

		if (CreateDebugUtilsMessengerEXT(m_Instance, &createInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS)
			LV_LOG_ERROR("Failed to set up debug messenger!");
		#endif
	}

	void VulkanInstance::CreateSurface()
	{
		GLFWwindow* handle = GraphicsContext::Get()->GetWindowHandle();

		if (glfwCreateWindowSurface(m_Instance, handle, nullptr, &m_Surface) != VK_SUCCESS)
			LV_LOG_ERROR("Failed to create window surface!");
	}

	void VulkanInstance::PickPhysicalDevice()
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

		if (deviceCount == 0)
			LV_LOG_ERROR("Failed to find GPUs with Vulkan support!");

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

		for (const auto& device : devices)
		{
			if (VulkanHelper::PhysicalDeviceSuitable(this, device))
			{
				m_PhysicalDevice = device;
				break;
			}
		}

		// Note(Jorben): Check if no device was selected
		if (m_PhysicalDevice == VK_NULL_HANDLE)
			LV_LOG_ERROR("Failed to find a suitable GPU!");
	}

	void VulkanInstance::CreateDevice()
	{
		VulkanHelper::QueueFamilyIndices indices = VulkanHelper::QueueFamilyIndices::Find(this, m_PhysicalDevice);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indices.GraphicsFamily.value(), indices.PresentFamily.value() };

		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures = {};
		deviceFeatures.samplerAnisotropy = VK_TRUE;

		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(s_RequestedDeviceExtensions.size());
		createInfo.ppEnabledExtensionNames = s_RequestedDeviceExtensions.data();

		#if BL_VALIDATION_LAYERS
		createInfo.enabledLayerCount = static_cast<uint32_t>(s_RequestedValidationLayers.size());
		createInfo.ppEnabledLayerNames = s_RequestedValidationLayers.data();
		#else
		createInfo.enabledLayerCount = 0;
		#endif

		if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device) != VK_SUCCESS)
			LV_LOG_ERROR("Failed to create logical device!");

		// Retrieve the graphics & present queue handle
		vkGetDeviceQueue(m_Device, indices.GraphicsFamily.value(), 0, &m_GraphicsQueue);
		vkGetDeviceQueue(m_Device, indices.PresentFamily.value(), 0, &m_PresentQueue);
	}

}