#pragma once

#include <stdint.h>
#include <memory>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include "Lavender/APIs/Vulkan/VulkanDevice.hpp"
#include "Lavender/Utils/Utils.hpp"

namespace Lavender
{

	class VulkanSwapChain
	{
	public:
		VulkanSwapChain(VkInstance vkInstance, Ref<VulkanDevice> vkDevice);

		void Init(uint32_t width, uint32_t height, const bool vsync);
		void Destroy();

		void BeginFrame();
		void EndFrame();

		void OnResize(uint32_t width, uint32_t height, const bool vsync);

		inline uint32_t GetCurrentFrame() const { return m_CurrentFrame; }
		inline uint32_t GetAquiredImage() const { return m_AquiredImage; }

		// Note(Jorben): This function is used by VulkanRenderCommandBuffers to wait for the image to be available
		inline VkSemaphore& GetCurrentImageAvailableSemaphore() { return m_ImageAvailableSemaphores[m_CurrentFrame]; }
		inline VkSemaphore& GetImageAvailableSemaphore(uint32_t index) { return m_ImageAvailableSemaphores[index]; }

		inline VkFramebuffer GetCurrentFrameBuffer() const { return m_Framebuffers[m_CurrentFrame]; }
		// TODO: Add a way of checking the index is valid
		inline VkFramebuffer GetFramebuffer(uint32_t index) const { return m_Framebuffers[index]; }

		inline VkCommandPool& GetCommandPool() { return m_CommandPool; }

		static Ref<VulkanSwapChain> Create(VkInstance vkInstance, Ref<VulkanDevice> vkDevice);

		void TempRecordDefaultCommandBuffer(); // TODO: Remove...
	
	private:
		uint32_t AcquireNextImage();
		void FindImageFormatAndColorSpace();


	private:
		VkInstance m_Instance = VK_NULL_HANDLE;
		Ref<VulkanDevice> m_Device = VK_NULL_HANDLE;
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

		std::vector<VkSemaphore> m_ImageAvailableSemaphores = { };

		VkFormat m_ColorFormat = VK_FORMAT_UNDEFINED;
		VkColorSpaceKHR m_ColorSpace = VK_COLOR_SPACE_MAX_ENUM_KHR;

		uint32_t m_CurrentFrame = 0;
		uint32_t m_AquiredImage = 0;
	};

}