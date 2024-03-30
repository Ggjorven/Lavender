#include "lvpch.h"
#include "VulkanViewport.hpp"

#include "Lavender/Core/Application.hpp"
#include "Lavender/Core/Logging.hpp"
#include "Lavender/Utils/Profiler.hpp"

#include "Lavender/Renderer/Mesh.hpp"
#include "Lavender/Renderer/Shader.hpp"
#include "Lavender/Renderer/Renderer.hpp"
#include "Lavender/Renderer/UniformBuffer.hpp"
#include "Lavender/Renderer/FrameResources.hpp"

#include "Lavender/APIs/Vulkan/VulkanAllocator.hpp"
#include "Lavender/APIs/Vulkan/VulkanContext.hpp"
#include "Lavender/APIs/Vulkan/VulkanRenderPass.hpp"
#include "Lavender/APIs/Vulkan/VulkanImGuiLayer.hpp"

#include "Lavender/UI/UI.hpp"

#include <glm/glm.hpp>

#include <imgui_internal.h>
#include <backends/imgui_impl_vulkan.h>

namespace Lavender
{

	VulkanViewportImage::VulkanViewportImage(uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height), m_Miplevels(1)
	{
		auto context = RefHelper::RefAs<VulkanContext>(Renderer::GetContext());
		VkFormat format = context->GetSwapChain()->GetColourFormat();

		m_Image.Allocation = VulkanAllocator::CreateImage(width, height, m_Miplevels, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VMA_MEMORY_USAGE_GPU_ONLY/*, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT*/, m_Image.Image);

		m_Image.ImageView = VulkanAllocator::CreateImageView(m_Image.Image, format, VK_IMAGE_ASPECT_COLOR_BIT, m_Miplevels);

		VulkanAllocator::TransitionImageLayout(m_Image.Image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_Miplevels);

		m_Sampler = VulkanAllocator::CreateSampler(m_Miplevels);
	}

	VulkanViewportImage::~VulkanViewportImage()
	{
		auto device = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetLogicalDevice()->GetVulkanDevice();
		vkDeviceWaitIdle(device);

		VulkanAllocator::DestroyImage(m_Image.Image, m_Image.Allocation);
		vkDestroyImageView(device, m_Image.ImageView, nullptr);

		vkDestroySampler(device, m_Sampler, nullptr);
	}

	void VulkanViewportImage::Resize(uint32_t width, uint32_t height)
	{
		auto context = RefHelper::RefAs<VulkanContext>(Renderer::GetContext());
		auto device = context->GetLogicalDevice()->GetVulkanDevice();

		VkFormat format = context->GetSwapChain()->GetColourFormat();

		vkDeviceWaitIdle(device);

		m_Width = width;
		m_Height = height;

		VulkanAllocator::DestroyImage(m_Image.Image, m_Image.Allocation);
		vkDestroyImageView(device, m_Image.ImageView, nullptr);

		m_Image.Allocation = VulkanAllocator::CreateImage(width, height, m_Miplevels, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VMA_MEMORY_USAGE_GPU_ONLY/*, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT*/, m_Image.Image);

		m_Image.ImageView = VulkanAllocator::CreateImageView(m_Image.Image, format, VK_IMAGE_ASPECT_COLOR_BIT, m_Miplevels);

		VulkanAllocator::TransitionImageLayout(m_Image.Image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_Miplevels);
	}



	VulkanViewportRenderPass::VulkanViewportRenderPass(Ref<VulkanViewportImage> image)
		: m_Image(image), m_CommandBuffer(RefHelper::RefAs<VulkanRenderCommandBuffer>(RenderCommandBuffer::Create(RenderCommandBuffer::Usage::Sequential)))
	{
		auto context = RefHelper::RefAs<VulkanContext>(Renderer::GetContext());

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Renderpass
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		std::array<VkAttachmentDescription, 2> attachments = {};
		// Color attachment
		attachments[0].format = context->GetSwapChain()->GetColourFormat();
		attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
		attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachments[0].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		// Depth attachment
		attachments[1].format = VulkanAllocator::FindDepthFormat();
		attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
		attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference colorReference = {};
		colorReference.attachment = 0;
		colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthReference = {};
		depthReference.attachment = 1;
		depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpassDescription = {};
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.colorAttachmentCount = 1;
		subpassDescription.pColorAttachments = &colorReference;
		subpassDescription.pDepthStencilAttachment = &depthReference;
		subpassDescription.inputAttachmentCount = 0;
		subpassDescription.pInputAttachments = nullptr;
		subpassDescription.preserveAttachmentCount = 0;
		subpassDescription.pPreserveAttachments = nullptr;
		subpassDescription.pResolveAttachments = nullptr;

		// Subpass dependencies for layout transitions
		std::array<VkSubpassDependency, 2> dependencies = { };

		dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[0].dstSubpass = 0;
		dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		dependencies[1].srcSubpass = 0;
		dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = (uint32_t)attachments.size();
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpassDescription;
		renderPassInfo.dependencyCount = (uint32_t)dependencies.size();
		renderPassInfo.pDependencies = dependencies.data();

		if (vkCreateRenderPass(context->GetLogicalDevice()->GetVulkanDevice(), &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS)
			LV_LOG_ERROR("Failed to create viewport render pass!");

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Framebuffers // TODO: Framebuffer class
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		std::vector<VkImageView> imageViews = { };
		imageViews.push_back(m_Image->GetImage().ImageView);
		imageViews.push_back(context->GetSwapChain()->GetDepthImageView());

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = m_RenderPass;
		framebufferInfo.attachmentCount = (uint32_t)imageViews.size();
		framebufferInfo.pAttachments = imageViews.data();
		framebufferInfo.width = m_Image->GetWidth();
		framebufferInfo.height = m_Image->GetHeight();
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(context->GetLogicalDevice()->GetVulkanDevice(), &framebufferInfo, nullptr, &m_Framebuffer) != VK_SUCCESS)
			LV_LOG_ERROR("Failed to create framebuffer!");
	}

	VulkanViewportRenderPass::~VulkanViewportRenderPass()
	{
		auto device = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetLogicalDevice();
		vkDeviceWaitIdle(device->GetVulkanDevice());

		vkDestroyFramebuffer(device->GetVulkanDevice(), m_Framebuffer, nullptr);

		vkDestroyRenderPass(device->GetVulkanDevice(), m_RenderPass, nullptr);
	}

	void VulkanViewportRenderPass::Begin()
	{
		m_CommandBuffer->Begin();

		VkExtent2D swapChainExtent = { m_Image->GetWidth(), m_Image->GetHeight() };

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_RenderPass;
		renderPassInfo.framebuffer = m_Framebuffer;
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapChainExtent;

		std::vector<VkClearValue> clearValues = {};

		VkClearValue colourClear = { { {0.0f, 0.0f, 0.0f, 1.0f} } };
		clearValues.push_back(colourClear);
		VkClearValue depthClear = { { { 1.0f, 0 } } };
		clearValues.push_back(depthClear);

		renderPassInfo.clearValueCount = (uint32_t)clearValues.size();
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(m_CommandBuffer->GetVulkanCommandBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)swapChainExtent.width;
		viewport.height = (float)swapChainExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(m_CommandBuffer->GetVulkanCommandBuffer(), 0, 1, &viewport);

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = swapChainExtent;
		vkCmdSetScissor(m_CommandBuffer->GetVulkanCommandBuffer(), 0, 1, &scissor);
	}

	void VulkanViewportRenderPass::End()
	{
		vkCmdEndRenderPass(m_CommandBuffer->GetVulkanCommandBuffer());

		m_CommandBuffer->End();
	}

	void VulkanViewportRenderPass::Submit()
	{
		m_CommandBuffer->Submit();
	}

	void VulkanViewportRenderPass::Resize(uint32_t width, uint32_t height)
	{
		m_Image->Resize(width, height);

		auto context = RefHelper::RefAs<VulkanContext>(Renderer::GetContext());
		auto device = context->GetLogicalDevice();

		vkDestroyFramebuffer(device->GetVulkanDevice(), m_Framebuffer, nullptr);

		std::vector<VkImageView> attachments = { };
		attachments.push_back(m_Image->GetImage().ImageView);
		auto depthImageView = context->GetSwapChain()->GetDepthImageView();
		attachments.push_back(depthImageView);

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = m_RenderPass;
		framebufferInfo.attachmentCount = (uint32_t)attachments.size();
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = width;	
		framebufferInfo.height = height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(context->GetLogicalDevice()->GetVulkanDevice(), &framebufferInfo, nullptr, &m_Framebuffer) != VK_SUCCESS)
			LV_LOG_ERROR("Failed to create framebuffer!");
	}

	Ref<RenderPass> VulkanViewportRenderPass::GetRenderPass()
	{
		return RefHelper::Create<VulkanRenderPass>(m_RenderPass, m_CommandBuffer);
	}



	VulkanViewport::VulkanViewport(uint32_t width, uint32_t height)
	{
		auto image = RefHelper::Create<VulkanViewportImage>(width, height);
		m_Renderpass = RefHelper::Create<VulkanViewportRenderPass>(image);

		m_WindowStyle = UI::StyleList({
			{ UI::StyleType::WindowPadding, { 0.0f, 0.0f} }
		});

		m_WindowColours = UI::StyleColourList({
			{ UI::StyleColourType::SeparatorActive, UI::Colours::LighterTint },
			{ UI::StyleColourType::SeparatorHovered, UI::Colours::LighterTint }
		});

		m_ImGuiImage = (ImTextureID)ImGui_ImplVulkan_AddTexture(image->GetSampler(), image->GetImage().ImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}

	VulkanViewport::~VulkanViewport()
	{
		auto device = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetLogicalDevice()->GetVulkanDevice();
		vkDeviceWaitIdle(device);
		
		auto pool = ((VulkanImGuiLayer*)Application::Get().GetImGuiLayer())->GetVulkanDescriptorPool();
		vkFreeDescriptorSets(device, pool, 1, (VkDescriptorSet*)&m_ImGuiImage);
	}

	void VulkanViewport::BeginFrame()
	{
		LV_PROFILE_SCOPE("VulkanViewport::BeginFrame");
		m_Renderpass->Begin();

		FrameResources::GetPipeline()->Use(m_Renderpass->GetCommandBuffer());
	}

	void VulkanViewport::EndFrame()
	{
		LV_PROFILE_SCOPE("VulkanViewport::EndFrame");
		m_Renderpass->End();
		m_Renderpass->Submit();

		Renderer::WaitFor(m_Renderpass->GetCommandBuffer());
	}

	void VulkanViewport::BeginRender()
	{
		LV_PROFILE_SCOPE("VulkanViewport::BeginRender");
		m_WindowStyle.Push();
		m_WindowColours.Push();

		// To remove the tab bar.
		ImGuiWindowClass window = {};
		window.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
		ImGui::SetNextWindowClass(&window);

		UI::BeginWindow("Viewport", UI::WindowFlags::NoCollapse | UI::WindowFlags::NoDecoration | UI::WindowFlags::NoBackground | UI::WindowFlags::NoTitleBar | UI::WindowFlags::NoMove);

		auto imWindow = ImGui::GetCurrentWindow();
		auto position = imWindow->Pos;
		auto size = imWindow->Size;
		if ((uint32_t)size.x != m_Width || (uint32_t)size.y != m_Height)
		{
			Resize((uint32_t)size.x, (uint32_t)size.y);
		}
		m_Width = (uint32_t)size.x;
		m_Height = (uint32_t)size.y;

		auto& mainWindow = Application::Get().GetWindow();
		m_XPos = (uint32_t)position.x - mainWindow.GetPositionX();
		m_YPos = (uint32_t)position.y - mainWindow.GetPositionY();

		auto region = ImGui::GetContentRegionAvail();
		ImGui::Image(GetCurrentImGuiTexture(), ImVec2(region.x, region.y), ImVec2(1.0f, 0.0f), ImVec2(0.0f, 1.0f)); // TODO: Replace with UI Image
	}

	void VulkanViewport::EndRender()
	{
		LV_PROFILE_SCOPE("VulkanViewport::EndRender");
		UI::EndWindow();

		m_WindowStyle.Pop();
		m_WindowColours.Pop();
	}

	bool VulkanViewport::InView(const glm::vec2& mainWindowPosition) const
	{
		if (mainWindowPosition.x >= m_XPos && mainWindowPosition.x <= m_XPos + m_Width &&
			mainWindowPosition.y >= m_YPos && mainWindowPosition.y <= m_YPos + m_Height)
		{
			return true;
		}

		return false;
	}

	void VulkanViewport::Resize(uint32_t width, uint32_t height)
	{
		if (width != 0 && height != 0)
		{
			auto device = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetLogicalDevice()->GetVulkanDevice();
			vkDeviceWaitIdle(device);

			auto pool = ((VulkanImGuiLayer*)Application::Get().GetImGuiLayer())->GetVulkanDescriptorPool();
			vkFreeDescriptorSets(device, pool, 1, (VkDescriptorSet*)&m_ImGuiImage);

			m_Renderpass->Resize(width, height);

			m_ImGuiImage = (ImTextureID)ImGui_ImplVulkan_AddTexture(m_Renderpass->GetImage()->GetSampler(), m_Renderpass->GetImage()->GetImage().ImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}
	}

	glm::vec2 VulkanViewport::ConvertMousePosition(const glm::vec2& mainWindowPosition) const
	{
		return glm::vec2(mainWindowPosition.x - m_XPos, mainWindowPosition.y - m_YPos);
	}

	ImTextureID VulkanViewport::GetCurrentImGuiTexture()
	{
		return m_ImGuiImage;
	}

}