#pragma once

#include <vector>

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Renderer/Viewport.hpp"
#include "Lavender/Renderer/RenderPass.hpp"

#include "Lavender/APIs/Vulkan/VulkanRenderCommandBuffer.hpp"

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Lavender
{

	class VulkanViewportImage : public ViewportImage
	{
	public:
		struct ViewportImageGroup
		{
			VkImage Image = VK_NULL_HANDLE;
			VmaAllocation Allocation = VK_NULL_HANDLE;
			VkImageView ImageView = VK_NULL_HANDLE;
		};

	public:
		VulkanViewportImage(uint32_t width, uint32_t height);
		virtual ~VulkanViewportImage();

		void Resize(uint32_t width, uint32_t height) override;

		inline std::vector<ViewportImageGroup> GetImages() { return m_Images; }
		inline VkSampler GetSampler() { return m_Sampler; }

	private:
		std::vector<ViewportImageGroup> m_Images = { };

		VkSampler m_Sampler = VK_NULL_HANDLE;

		uint32_t m_Width = 0, m_Height = 0, m_Miplevels = 0;
	};



	class VulkanViewportRenderPass : public ViewportRenderPass
	{
	public:
		VulkanViewportRenderPass(Ref<ViewportImage> image);
		virtual ~VulkanViewportRenderPass();

		void Begin() override;
		void End() override;
		void Submit() override;

		void Resize(uint32_t width, uint32_t height) override;

		Ref<RenderPass> GetRenderPass() override;
		inline Ref<RenderCommandBuffer> GetCommandBuffer() override { return m_CommandBuffer; }
		inline Ref<VulkanViewportImage> GetImage() { return m_Image; }

	private:
		Ref<VulkanRenderCommandBuffer> m_CommandBuffer = VK_NULL_HANDLE;
		Ref<VulkanViewportImage> m_Image = VK_NULL_HANDLE;

		VkRenderPass m_RenderPass = VK_NULL_HANDLE;
		std::vector<VkFramebuffer> m_Framebuffers = { };
	};



	class VulkanViewport : public Viewport
	{
	public:
		VulkanViewport(uint32_t width, uint32_t height);
		virtual ~VulkanViewport();

		void BeginFrame() override;
		void EndFrame() override;

		void Resize(uint32_t width, uint32_t height) override;

		inline Ref<ViewportRenderPass> GetRenderPass() override { return m_Renderpass; }
		ImTextureID GetCurrentImGuiTexture() override;

	private:
		Ref<VulkanViewportRenderPass> m_Renderpass = VK_NULL_HANDLE;

		std::vector<ImTextureID> m_ImGuiImages = { };
	};

}