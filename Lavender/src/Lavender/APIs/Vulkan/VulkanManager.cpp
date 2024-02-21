#include "lvpch.h"
#include "VulkanManager.hpp"

#include "Lavender/Renderer/GraphicsContext.hpp"
#include "Lavender/APIs/Vulkan/Setup/VulkanInstance.hpp"
#include "Lavender/APIs/Vulkan/Setup/VulkanSwapChain.hpp"
#include "Lavender/APIs/Vulkan/Setup/VulkanResources.hpp"

namespace Lavender
{

	std::unique_ptr<VulkanManager> VulkanManager::s_Instance = nullptr;

	void VulkanManager::Init()
	{
		s_Instance = std::make_unique<VulkanManager>();
	}

	void VulkanManager::RecreateSwapChain()
	{
		VulkanSwapChain& vc = *GraphicsContext::Get()->GetSwapChain();
		vc.RecreateSwapChain();

		VulkanManager::PopulateResourceInfo();

		auto& resources = *GraphicsContext::Get()->GetResources();

		// TODO(Jorben): Recreate colour resources (TODO)
		resources.RecreateDepthResources();

		VulkanManager::PopulateResourceInfo();
		resources.RecreateFramebuffers();
	}

	void VulkanManager::PopulateDeviceInfo()
	{
		PopulateDeviceInfo(GraphicsContext::Get()->GetInstance());
	}

	void VulkanManager::PopulateSwapChainInfo()
	{
		PopulateSwapChainInfo(GraphicsContext::Get()->GetSwapChain());
	}

	void VulkanManager::PopulateResourceInfo()
	{
		PopulateResourceInfo(GraphicsContext::Get()->GetResources());
	}

	void VulkanManager::PopulateDeviceInfo(VulkanInstance* obj)
	{
		VulkanInstance& vc = *obj;

		VulkanDeviceInfo deviceInfo = {};
		deviceInfo.Instance = vc.m_Instance;
		deviceInfo.DebugMessenger = vc.m_DebugMessenger;
		deviceInfo.Surface = vc.m_Surface;

		deviceInfo.PhysicalDevice = vc.m_PhysicalDevice;
		deviceInfo.Device = vc.m_Device;

		deviceInfo.GraphicsQueue = vc.m_GraphicsQueue;
		deviceInfo.PresentQueue = vc.m_PresentQueue;

		s_Instance->m_DeviceInfo = deviceInfo;
	}

	void VulkanManager::PopulateSwapChainInfo(VulkanSwapChain* obj)
	{
		VulkanSwapChain& vc = *obj;

		VulkanSwapChainInfo swapchainInfo = {};
		swapchainInfo.SwapChain = vc.m_SwapChain;
		swapchainInfo.SwapChainImageFormat = vc.m_SwapChainImageFormat;
		swapchainInfo.SwapChainExtent = vc.m_SwapChainExtent;

		swapchainInfo.SwapChainImages = vc.m_SwapChainImages;
		swapchainInfo.SwapChainImageViews = vc.m_SwapChainImageViews;

		s_Instance->m_SwapChainInfo = swapchainInfo;
	}

	void VulkanManager::PopulateResourceInfo(VulkanResources* obj)
	{
		VulkanResources& vc = *obj;

		VulkanResourceInfo resourceInfo = {};
		resourceInfo.CommandPool = vc.m_CommandPool;
		resourceInfo.CommandBuffers = vc.m_CommandBuffers;

		resourceInfo.ImageAvailableSemaphores = vc.m_ImageAvailableSemaphores;
		resourceInfo.RenderFinishedSemaphores = vc.m_RenderFinishedSemaphores;
		resourceInfo.InFlightFences = vc.m_InFlightFences;

		resourceInfo.DepthImage = vc.m_DepthImage;
		resourceInfo.DepthImageMemory = vc.m_DepthImageMemory;
		resourceInfo.DepthImageView = vc.m_DepthImageView;

		s_Instance->m_ResourceInfo = resourceInfo;
	}

}