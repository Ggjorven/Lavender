#pragma once

#include <vector>

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Renderer/Viewport.hpp"
#include "Lavender/Renderer/RenderPass.hpp"

#include "Lavender/APIs/Vulkan/VulkanPipeline.hpp"
#include "Lavender/APIs/Vulkan/VulkanRenderCommandBuffer.hpp"

#include "Lavender/UI/Style.hpp"

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Lavender
{

	class VulkanViewportImage
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

		void Resize(uint32_t width, uint32_t height);

		inline uint32_t GetWidth() const { return m_Width; }
		inline uint32_t GetHeight() const { return m_Height; }

		inline ViewportImageGroup GetImage() { return m_Image; }
		inline VkSampler GetSampler() { return m_Sampler; }

	private:
		ViewportImageGroup m_Image = {};
		VkSampler m_Sampler = VK_NULL_HANDLE;

		uint32_t m_Width = 0, m_Height = 0, m_Miplevels = 0;
	};



	class VulkanViewportRenderPass
	{
	public:
		VulkanViewportRenderPass(Ref<VulkanViewportImage> image);
		virtual ~VulkanViewportRenderPass();

		void Begin();
		void End();
		void Submit();

		void Resize(uint32_t width, uint32_t height);

		Ref<RenderPass> GetRenderPass();
		inline Ref<RenderCommandBuffer> GetCommandBuffer() { return m_CommandBuffer; }
		inline Ref<VulkanViewportImage> GetImage() { return m_Image; }

	private:
		Ref<VulkanRenderCommandBuffer> m_CommandBuffer = VK_NULL_HANDLE;
		Ref<VulkanViewportImage> m_Image = VK_NULL_HANDLE;

		VkRenderPass m_RenderPass = VK_NULL_HANDLE;
		VkFramebuffer m_Framebuffer = VK_NULL_HANDLE;
	};



	class VulkanViewport : public Viewport
	{
	public:
		VulkanViewport(uint32_t width, uint32_t height);
		virtual ~VulkanViewport();

		void BeginFrame() override;
		void EndFrame() override;

		void BeginRender() override;
		void EndRender() override;

		inline uint32_t GetWidth() const override { return m_Width; }
		inline uint32_t GetHeight() const override { return m_Height; }

		void Resize(uint32_t width, uint32_t height) override;

		inline Ref<RenderPass> GetRenderPass() override { return m_Renderpass->GetRenderPass(); }
		inline Ref<Pipeline> GetPipeline() override { return m_Pipeline; }
		ImTextureID GetCurrentImGuiTexture() override;

	private:
		Ref<VulkanPipeline> m_Pipeline = VK_NULL_HANDLE;
		Ref<VulkanViewportRenderPass> m_Renderpass = VK_NULL_HANDLE;

		UI::StyleList m_WindowStyle = {};

		ImTextureID m_ImGuiImage = {};
		uint32_t m_Width = 0, m_Height = 0;
	};

}