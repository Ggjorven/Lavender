#include "lvpch.h"
#include "VulkanViewport.hpp"

#include "Lavender/Core/Application.hpp"
#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/Renderer.hpp"

#include "Lavender/APIs/Vulkan/VulkanAllocator.hpp"
#include "Lavender/APIs/Vulkan/VulkanContext.hpp"
#include "Lavender/APIs/Vulkan/VulkanRenderPass.hpp"
#include "Lavender/APIs/Vulkan/VulkanImGuiLayer.hpp"

#include <imgui_internal.h>
#include <backends/imgui_impl_vulkan.h>

namespace Lavender
{

	VulkanViewportImage::VulkanViewportImage(uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height), m_Miplevels(1)
	{
		auto context = RefHelper::RefAs<VulkanContext>(Renderer::GetContext());

		auto imageViews = context->GetSwapChain()->GetImageViews();
		m_Images.resize(imageViews.size());
		for (size_t i = 0; i < imageViews.size(); i++)
		{
			m_Images[i].Allocation = VulkanAllocator::CreateImage(width, height, m_Miplevels, context->GetSwapChain()->GetColourFormat(), VK_IMAGE_TILING_LINEAR, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VMA_MEMORY_USAGE_GPU_ONLY, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_Images[i].Image);

			m_Images[i].ImageView = VulkanAllocator::CreateImageView(m_Images[i].Image, context->GetSwapChain()->GetColourFormat(), VK_IMAGE_ASPECT_COLOR_BIT, m_Miplevels);

			VulkanAllocator::TransitionImageLayout(m_Images[i].Image, context->GetSwapChain()->GetColourFormat(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_Miplevels);
		}

		m_Sampler = VulkanAllocator::CreateSampler(m_Miplevels); // TODO: Change to 0?
	}

	VulkanViewportImage::~VulkanViewportImage()
	{
		auto device = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetLogicalDevice()->GetVulkanDevice();
		vkDeviceWaitIdle(device);

		for (auto& image : m_Images)
		{
			VulkanAllocator::DestroyImage(image.Image, image.Allocation);
			vkDestroyImageView(device, image.ImageView, nullptr);
		}

		vkDestroySampler(device, m_Sampler, nullptr);
	}

	void VulkanViewportImage::Resize(uint32_t width, uint32_t height)
	{
		auto context = RefHelper::RefAs<VulkanContext>(Renderer::GetContext());
		auto device = context->GetLogicalDevice()->GetVulkanDevice();
		
		vkDeviceWaitIdle(device);

		m_Width = width;
		m_Height = height;

		for (auto& image : m_Images)
		{
			VulkanAllocator::DestroyImage(image.Image, image.Allocation);
			vkDestroyImageView(device, image.ImageView, nullptr);
		}

		auto imageViews = context->GetSwapChain()->GetImageViews();
		m_Images.clear();
		m_Images.resize(imageViews.size());
		for (size_t i = 0; i < imageViews.size(); i++)
		{
			m_Images[i].Allocation = VulkanAllocator::CreateImage(width, height, m_Miplevels, context->GetSwapChain()->GetColourFormat(), VK_IMAGE_TILING_LINEAR, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VMA_MEMORY_USAGE_GPU_ONLY, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_Images[i].Image);

			m_Images[i].ImageView = VulkanAllocator::CreateImageView(m_Images[i].Image, context->GetSwapChain()->GetColourFormat(), VK_IMAGE_ASPECT_COLOR_BIT, m_Miplevels);

			VulkanAllocator::TransitionImageLayout(m_Images[i].Image, context->GetSwapChain()->GetColourFormat(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_Miplevels);
		}
	}



	VulkanViewportRenderPass::VulkanViewportRenderPass(Ref<ViewportImage> image)
		: m_Image(RefHelper::RefAs<VulkanViewportImage>(image)), m_CommandBuffer(RefHelper::RefAs<VulkanRenderCommandBuffer>(RenderCommandBuffer::Create(RenderCommandBuffer::Usage::Sequential)))
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
		auto images = m_Image->GetImages();
		m_Framebuffers.resize(images.size());
		for (size_t i = 0; i < images.size(); i++)
		{
			std::vector<VkImageView> attachments = { };
			attachments.push_back(images[i].ImageView);
			attachments.push_back(context->GetSwapChain()->GetDepthImageView());

			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = m_RenderPass;
			framebufferInfo.attachmentCount = (uint32_t)attachments.size();
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = m_Image->GetWidth();
			framebufferInfo.height = m_Image->GetHeight();
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(context->GetLogicalDevice()->GetVulkanDevice(), &framebufferInfo, nullptr, &m_Framebuffers[i]) != VK_SUCCESS)
				LV_LOG_ERROR("Failed to create framebuffer!");
		}
	}

	VulkanViewportRenderPass::~VulkanViewportRenderPass()
	{
		auto device = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetLogicalDevice();
		vkDeviceWaitIdle(device->GetVulkanDevice());

		for (auto& framebuffer : m_Framebuffers)
			vkDestroyFramebuffer(device->GetVulkanDevice(), framebuffer, nullptr);


		vkDestroyRenderPass(device->GetVulkanDevice(), m_RenderPass, nullptr);
	}

	void VulkanViewportRenderPass::Begin()
	{
		m_CommandBuffer->Begin();

		VkExtent2D swapChainExtent = { m_Image->GetWidth(), m_Image->GetHeight() };

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_RenderPass;
		renderPassInfo.framebuffer = m_Framebuffers[RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetSwapChain()->GetAquiredImage()];
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

		vkDeviceWaitIdle(device->GetVulkanDevice());

		// Destroy
		for (auto& framebuffer : m_Framebuffers)
			vkDestroyFramebuffer(device->GetVulkanDevice(), framebuffer, nullptr);

		vkDeviceWaitIdle(device->GetVulkanDevice());

		auto imageViews = context->GetSwapChain()->GetImageViews();
		m_Framebuffers.resize(imageViews.size());
		for (size_t i = 0; i < imageViews.size(); i++)
		{
			std::vector<VkImageView> attachments = { };
			attachments.push_back(m_Image->GetImages()[RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetSwapChain()->GetAquiredImage()].ImageView);

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

			if (vkCreateFramebuffer(context->GetLogicalDevice()->GetVulkanDevice(), &framebufferInfo, nullptr, &m_Framebuffers[i]) != VK_SUCCESS)
				LV_LOG_ERROR("Failed to create framebuffer!");
		}
	}

	Ref<RenderPass> VulkanViewportRenderPass::GetRenderPass()
	{
		return RefHelper::Create<VulkanRenderPass>(m_RenderPass);
	}



	VulkanViewport::VulkanViewport(uint32_t width, uint32_t height)
	{
		auto image = RefHelper::Create<VulkanViewportImage>(width, height);
		m_Renderpass = RefHelper::Create<VulkanViewportRenderPass>(image);

		for (auto& i : image->GetImages())
		{
			auto img = ImGui_ImplVulkan_AddTexture(image->GetSampler(), i.ImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			m_ImGuiImages.push_back((ImTextureID)img);
		}
	}

	VulkanViewport::~VulkanViewport()
	{
		auto device = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetLogicalDevice()->GetVulkanDevice();

		vkDeviceWaitIdle(device);
		auto pool = ((VulkanImGuiLayer*)Application::Get().GetImGuiLayer())->GetVulkanDescriptorPool();
		vkFreeDescriptorSets(device, pool, (uint32_t)m_ImGuiImages.size(), (VkDescriptorSet*)m_ImGuiImages.data());
	}

	void VulkanViewport::BeginFrame()
	{
		// TODO: Add a better way to prevent tearing
		Resize(m_Width, m_Height);

		m_Renderpass->Begin();
	}

	void VulkanViewport::EndFrame()
	{
		m_Renderpass->End();
		m_Renderpass->Submit();

		Renderer::WaitFor(m_Renderpass->GetCommandBuffer());
	}

	void VulkanViewport::BeginRender()
	{
		ImGui::Begin("Viewport");

		auto size = ImGui::GetWindowSize();
		m_Width = (uint32_t)size.x;
		m_Height = (uint32_t)size.y;
		
		auto region = ImGui::GetContentRegionAvail();
		ImGui::Image(GetCurrentImGuiTexture(), ImVec2(region.x, region.y));
	}

	void VulkanViewport::EndRender()
	{
		ImGui::End();
	}

	void VulkanViewport::Resize(uint32_t width, uint32_t height)
	{
		if (width != 0 && height != 0)
		{
			auto device = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetLogicalDevice()->GetVulkanDevice();

			auto pool = ((VulkanImGuiLayer*)Application::Get().GetImGuiLayer())->GetVulkanDescriptorPool();
			vkFreeDescriptorSets(device, pool, (uint32_t)m_ImGuiImages.size(), (VkDescriptorSet*)m_ImGuiImages.data());

			m_Renderpass->Resize(width, height);

			m_ImGuiImages.clear();
			auto image = m_Renderpass->GetImage();
			for (auto& i : image->GetImages())
			{
				auto img = ImGui_ImplVulkan_AddTexture(image->GetSampler(), i.ImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
				m_ImGuiImages.push_back((ImTextureID)img);
			}
		}
	}

	ImTextureID VulkanViewport::GetCurrentImGuiTexture()
	{
		auto acquiredImage = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetSwapChain()->GetAquiredImage();
		return m_ImGuiImages[acquiredImage];
	}

}