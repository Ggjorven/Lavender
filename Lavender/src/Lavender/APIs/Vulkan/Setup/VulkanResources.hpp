#pragma once

#include <memory>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include "Lavender/APIs/Vulkan/VulkanRenderPass.hpp"

namespace Lavender
{

	#define LV_MAX_FRAMES_IN_FLIGHT 2

	class VulkanManager;
	class VulkanHelper;

	class VulkanResources
	{
	public:
		VulkanResources();
		virtual ~VulkanResources();

		void AddRenderPass(VulkanRenderPass* renderpass);

		std::vector<VulkanRenderPass*>& GetRenderPasses() { return m_RenderPasses; }

		void RecreateDepthResources();
		void RecreateFramebuffers();

	private:
		void CreateCommandPool();
		void CreateCommandBuffers();
		void CreateSyncObjects();

		void CreateDepthResources();

	private:
		VkCommandPool m_CommandPool = VK_NULL_HANDLE;
		std::vector<VkCommandBuffer> m_CommandBuffers = { };

		// Used for synchronization
		std::vector<VkSemaphore> m_ImageAvailableSemaphores = { };
		std::vector<VkSemaphore> m_RenderFinishedSemaphores = { };
		std::vector<VkFence> m_InFlightFences = { };

		// All renderpasses
		std::vector<VulkanRenderPass*> m_RenderPasses = { };

		// Colour and render images.
		VkImage m_DepthImage = VK_NULL_HANDLE;
		VmaAllocation m_DepthImageMemory = VK_NULL_HANDLE;
		VkImageView m_DepthImageView = VK_NULL_HANDLE;

		friend class VulkanManager;
		friend class VulkanHelper;
	};

}