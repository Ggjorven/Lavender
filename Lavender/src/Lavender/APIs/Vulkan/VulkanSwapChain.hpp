#pragma once

#include <stdint.h>
#include <memory>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include "Lavender/APIs/Vulkan/VulkanDevice.hpp"

namespace Lavender
{

	class VulkanSwapChain
	{
	public:
		VulkanSwapChain(VkInstance vkInstance, std::shared_ptr<VulkanDevice> vkDevice);

		void Init(uint32_t width, uint32_t height, const bool vsync);
		void Destroy();

		void BeginFrame();
		void EndFrame();

		void OnResize(uint32_t width, uint32_t height, const bool vsync);

		VkFramebuffer GetCurrentFrameBuffer() const { return m_Framebuffers[m_CurrentFrame]; }
		// TODO: Add a way of checking the index is valid
		VkFramebuffer GetFramebuffer(uint32_t index) { return m_Framebuffers[index]; }

		static std::shared_ptr<VulkanSwapChain> Create(VkInstance vkInstance, std::shared_ptr<VulkanDevice> vkDevice);

	private:
		uint32_t AcquireNextImage();
		void FindImageFormatAndColorSpace();

		void TempRecordDefaultCommandBuffer();

	private:
		VkInstance m_Instance = VK_NULL_HANDLE;
		std::shared_ptr<VulkanDevice> m_Device = VK_NULL_HANDLE;
		VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;

		struct SwapchainImage
		{
			VkImage Image = VK_NULL_HANDLE;
			VkImageView ImageView = VK_NULL_HANDLE;
		};
		std::vector<SwapchainImage> m_Images;

		struct
		{
			VkImage Image = VK_NULL_HANDLE;
			VmaAllocation MemoryAlloc = VK_NULL_HANDLE;
			VkImageView ImageView = VK_NULL_HANDLE;
		} m_DepthStencil;

		std::vector<VkFramebuffer> m_Framebuffers;
		VkRenderPass m_RenderPass = nullptr;

		VkCommandPool m_CommandPool = VK_NULL_HANDLE;
		std::vector<VkCommandBuffer> m_CommandBuffers = { };

		std::vector<VkSemaphore> m_ImageAvailableSemaphores = { };
		std::vector<VkSemaphore> m_RenderFinishedSemaphores = { };
		std::vector<VkFence> m_InFlightFences = { };

		VkFormat m_ColorFormat = VK_FORMAT_UNDEFINED;
		VkColorSpaceKHR m_ColorSpace = VK_COLOR_SPACE_MAX_ENUM_KHR;

		uint32_t m_CurrentFrame = 0;
		uint32_t m_AquiredImage = 0;
	};

}