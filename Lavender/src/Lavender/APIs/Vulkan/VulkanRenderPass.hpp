#pragma once

#include <memory>

#include <vulkan/vulkan.h>

#include <glm/glm.hpp>

#include "Lavender/Renderer/RenderingAPI.hpp"

#include "Lavender/Utils/Utils.hpp"

namespace Lavender
{

	class Renderer;
	class BaseImGuiLayer;

	enum Attachments
	{
		AttachmentNone = 0,
		ColourAttachment = BIT(0),
		DepthAttachment = BIT(1)
	};

	class VulkanRenderPass
	{
	public:
		enum class ColourSpace
		{
			None = 0,
			Unorm = 44,
			sRGB = 50
		};

	public:
		VulkanRenderPass(const glm::vec2& extent, ColourSpace colourSpace, Attachments attachments);
		VulkanRenderPass(const glm::vec2& extent, ColourSpace colourSpace, int attachments);

		VulkanRenderPass(const glm::vec2& extent, ColourSpace colourSpace, Attachments attachments, std::vector<VkFramebuffer>& framebuffers);
		VulkanRenderPass(const glm::vec2& extent, ColourSpace colourSpace, int attachments, std::vector<VkFramebuffer>& framebuffers);
		virtual ~VulkanRenderPass();

		void RecreateFrameBuffers();

		bool HasDepth() const { return m_Attachments & DepthAttachment; }
		VkRenderPass& GetVkRenderPass() { return m_RenderPass; }

	private:
		void CreateRenderPass(const glm::vec2& extent, ColourSpace colourSpace, Attachments attachments);
		void CreateFrameBuffers(const glm::vec2& extent, ColourSpace colourSpace, Attachments attachments);

	private:
		ColourSpace m_ColourSpace = ColourSpace::None;
		Attachments m_Attachments = AttachmentNone;

		VkRenderPass m_RenderPass = VK_NULL_HANDLE;
		std::vector<VkFramebuffer> m_SwapChainFramebuffers = { };

		friend class Renderer;
		friend class BaseImGuiLayer;
	};

}