#pragma once

#include <vector>

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Renderer/RenderPass.hpp"
#include "Lavender/Renderer/RenderCommandBuffer.hpp"

#include "Lavender/APIs/Vulkan/VulkanRenderCommandBuffer.hpp"
#include "Lavender/APIs/Vulkan/VulkanImage.hpp"

#include <vulkan/vulkan.h>

namespace Lavender
{

	class VulkanRenderPass : public RenderPass
	{
	public:
		VulkanRenderPass(RenderPassSpecification specs);
		VulkanRenderPass(RenderPassSpecification specs, Ref<RenderCommandBuffer> commandBuffer);
		virtual ~VulkanRenderPass();

		void Begin() override;
		void End() override;
		void Submit() override;

		void AddAttachment(Ref<Image2D> attachment) override;

		void Resize(uint32_t width, uint32_t height) override;

		Ref<RenderCommandBuffer> GetCommandBuffer() override { return m_CommandBuffer; }
		VkRenderPass& GetVulkanRenderPass() { return m_RenderPass; }

	private:
		void Create();

		void Destroy();

	private:
		RenderPassSpecification m_Specification = {};

		Ref<VulkanRenderCommandBuffer> m_CommandBuffer = VK_NULL_HANDLE;

		VkRenderPass m_RenderPass = VK_NULL_HANDLE;
		std::vector<VkFramebuffer> m_Framebuffers = { };

		Ref<VulkanImage2D> m_Attachment = nullptr;
	};
}