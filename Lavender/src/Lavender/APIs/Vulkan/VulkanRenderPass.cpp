#include "lvpch.h"
#include "VulkanRenderPass.hpp"

#include "Lavender/Core/Logging.hpp"
#include "Lavender/Core/Application.hpp"

#include "Lavender/Renderer/Renderer.hpp"
#include "Lavender/APIs/Vulkan/VulkanRenderer.hpp"
#include "Lavender/APIs/Vulkan/VulkanContext.hpp"
#include "Lavender/APIs/Vulkan/VulkanAllocator.hpp"
#include "Lavender/APIs/Vulkan/VulkanImGuiLayer.hpp"

namespace Lavender
{

	VulkanRenderPass::VulkanRenderPass(RenderPassSpecification specs)
        : VulkanRenderPass(specs, RefHelper::RefAs<VulkanRenderCommandBuffer>(RenderCommandBuffer::Create(RenderCommandBuffer::Usage::Sequential))) // TODO: Make this selectable
    {
	}

	VulkanRenderPass::VulkanRenderPass(RenderPassSpecification specs, Ref<RenderCommandBuffer> commandBuffer)
        : m_Specification(specs), m_CommandBuffer(RefHelper::RefAs<VulkanRenderCommandBuffer>(commandBuffer))
	{
        Create();
	}

    VulkanRenderPass::VulkanRenderPass(RenderPassSpecification specs, Ref<Image2D> image)
        : m_Specification(specs), m_Image(RefHelper::RefAs<VulkanImage2D>(image)), m_CommandBuffer(RefHelper::RefAs<VulkanRenderCommandBuffer>(RenderCommandBuffer::Create(RenderCommandBuffer::Usage::Sequential))) // TODO: Make this selectable
    {
        Create();
    }

	VulkanRenderPass::~VulkanRenderPass()
	{
        Destroy();
	}

	void VulkanRenderPass::Begin()
	{
        m_CommandBuffer->Begin();

        VkExtent2D swapChainExtent = { Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight() };

        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_RenderPass;
        renderPassInfo.framebuffer = m_Framebuffers[RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetSwapChain()->GetAquiredImage()];
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = swapChainExtent;

        std::vector<VkClearValue> clearValues = {};

        VkClearValue colourClear = { { {0.0f, 0.0f, 0.0f, 1.0f} } };
        clearValues.push_back(colourClear);

        if (m_Specification.UsedAttachments & RenderPassSpecification::Attachments::Depth)
        {
            VkClearValue depthClear = { { { 1.0f, 0 } } };
            clearValues.push_back(depthClear);
        }

        renderPassInfo.clearValueCount = (uint32_t)clearValues.size();
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(m_CommandBuffer->GetVulkanCommandBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        // TODO: Maybe move this?
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

	void VulkanRenderPass::End()
	{
        vkCmdEndRenderPass(m_CommandBuffer->GetVulkanCommandBuffer());

        m_CommandBuffer->End();
	}

	void VulkanRenderPass::Submit()
	{
        m_CommandBuffer->Submit();
	}

    void VulkanRenderPass::Resize(uint32_t width, uint32_t height)
    {
        auto context = RefHelper::RefAs<VulkanContext>(Renderer::GetContext());
        auto device = context->GetLogicalDevice();

        // Destroy
        for (auto& framebuffer : m_Framebuffers)
            vkDestroyFramebuffer(device->GetVulkanDevice(), framebuffer, nullptr);

        auto imageViews = context->GetSwapChain()->GetImageViews();

        m_Framebuffers.resize(imageViews.size());
        
        for (size_t i = 0; i < imageViews.size(); i++)
        {
            std::vector<VkImageView> attachments = { };
            if (!m_Image)
            {
                attachments.push_back(context->GetSwapChain()->GetImageViews()[i]);
            }
            else
            {
                attachments.push_back(m_Image->GetImageView());
            }

            if (m_Specification.UsedAttachments & RenderPassSpecification::Attachments::Depth)
            {
                auto depthImageView = context->GetSwapChain()->GetDepthImageView();
                attachments.push_back(depthImageView);
            }

            VkFramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = m_RenderPass;
            framebufferInfo.attachmentCount = (uint32_t)attachments.size();
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = Application::Get().GetWindow().GetWidth();
            framebufferInfo.height = Application::Get().GetWindow().GetHeight();
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(context->GetLogicalDevice()->GetVulkanDevice(), &framebufferInfo, nullptr, &m_Framebuffers[i]) != VK_SUCCESS)
                LV_LOG_ERROR("Failed to create framebuffer!");
        }
    }

    void VulkanRenderPass::Create()
    {
        auto context = RefHelper::RefAs<VulkanContext>(Renderer::GetContext());

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Renderpass
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector<VkAttachmentDescription> attachments = { };
        std::vector<VkAttachmentReference> attachmentRefs = { };

        {
            VkAttachmentDescription& colorAttachment = attachments.emplace_back();
            colorAttachment.format = m_Image ? m_Image->GetFormat() : context->GetSwapChain()->GetColourFormat();
            colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
            colorAttachment.loadOp = (VkAttachmentLoadOp)m_Specification.ColourLoadOp;
            colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colorAttachment.initialLayout = (VkImageLayout)m_Specification.PreviousImageLayout;
            colorAttachment.finalLayout = (VkImageLayout)m_Specification.FinalImageLayout;

            VkAttachmentReference& colorAttachmentRef = attachmentRefs.emplace_back();
            colorAttachmentRef.attachment = 0;
            colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        }

        if (m_Specification.UsedAttachments & RenderPassSpecification::Attachments::Depth)
        {
            VkAttachmentDescription& depthAttachment = attachments.emplace_back();
            depthAttachment.format = VulkanAllocator::FindDepthFormat();
            depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
            depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            VkAttachmentReference& depthAttachmentRef = attachmentRefs.emplace_back();
            depthAttachmentRef.attachment = 1;
            depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        }

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &attachmentRefs[0];

        if (m_Specification.UsedAttachments & RenderPassSpecification::Attachments::Depth)
            subpass.pDepthStencilAttachment = &attachmentRefs[1];

        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcAccessMask = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        if (m_Specification.UsedAttachments & RenderPassSpecification::Attachments::Depth)
            dependency.dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = (uint32_t)attachments.size();
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(context->GetLogicalDevice()->GetVulkanDevice(), &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS)
            LV_LOG_ERROR("Failed to create render pass!");

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Framebuffers // TODO: Framebuffer class
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        auto imageViews = context->GetSwapChain()->GetImageViews();

        m_Framebuffers.resize(imageViews.size());

        for (size_t i = 0; i < imageViews.size(); i++)
        {
            std::vector<VkImageView> attachments = { };
            if (!m_Image)
            {
                attachments.push_back(context->GetSwapChain()->GetImageViews()[i]);
            }
            else
            {
                attachments.push_back(m_Image->GetImageView());
            }

            if (m_Specification.UsedAttachments & RenderPassSpecification::Attachments::Depth)
            {
                auto depthImageView = context->GetSwapChain()->GetDepthImageView();
                attachments.push_back(depthImageView);
            }

            VkFramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = m_RenderPass;
            framebufferInfo.attachmentCount = (uint32_t)attachments.size();
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = Application::Get().GetWindow().GetWidth();
            framebufferInfo.height = Application::Get().GetWindow().GetHeight();
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(context->GetLogicalDevice()->GetVulkanDevice(), &framebufferInfo, nullptr, &m_Framebuffers[i]) != VK_SUCCESS)
                LV_LOG_ERROR("Failed to create framebuffer!");
        }
    }

    void VulkanRenderPass::Destroy()
    {
        auto device = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetLogicalDevice();

        vkDeviceWaitIdle(device->GetVulkanDevice());

        for (auto& framebuffer : m_Framebuffers)
            vkDestroyFramebuffer(device->GetVulkanDevice(), framebuffer, nullptr);

        vkDestroyRenderPass(device->GetVulkanDevice(), m_RenderPass, nullptr);
    }

}