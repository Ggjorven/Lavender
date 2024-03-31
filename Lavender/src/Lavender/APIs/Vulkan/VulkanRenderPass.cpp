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

    VulkanRenderPass::VulkanRenderPass(VkRenderPass renderPass, Ref<VulkanRenderCommandBuffer> commandBuffer) // Note(Jorben): Since this constructor is only used for the Viewport we need to not destroy
        : m_RenderPass(renderPass), m_CommandBuffer(commandBuffer), m_Destroy(false)
    {
    }

    VulkanRenderPass::VulkanRenderPass(RenderPassSpecification specs)
        : VulkanRenderPass(specs, RefHelper::RefAs<VulkanRenderCommandBuffer>(RenderCommandBuffer::Create(RenderCommandBuffer::Usage::Sequential))) // TODO: Make this selectable
    {
	}

	VulkanRenderPass::VulkanRenderPass(RenderPassSpecification specs, Ref<RenderCommandBuffer> commandBuffer)
        : m_Specification(specs), m_CommandBuffer(RefHelper::RefAs<VulkanRenderCommandBuffer>(commandBuffer))
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
            attachments.push_back(context->GetSwapChain()->GetImageViews()[i]);

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
            framebufferInfo.width = width;
            framebufferInfo.height = height;
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
            colorAttachment.format = context->GetSwapChain()->GetColourFormat();
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

        std::array<VkSubpassDependency, 1> dependencies = { };
        dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[0].dstSubpass = 0;
        dependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependencies[0].srcAccessMask = 0;
        dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        if (m_Specification.UsedAttachments & RenderPassSpecification::Attachments::Depth)
            dependencies[0].dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        /*
        dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[0].dstSubpass = 0;
        dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        if (m_Specification.UsedAttachments & RenderPassSpecification::Attachments::Depth)
            dependencies[0].dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        dependencies[1].srcSubpass = 0;
        dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        if (m_Specification.UsedAttachments & RenderPassSpecification::Attachments::Depth)
            dependencies[0].srcAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        
        dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
        */

        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = (uint32_t)attachments.size();
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = (uint32_t)dependencies.size();
        renderPassInfo.pDependencies = dependencies.data();

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
            attachments.push_back(context->GetSwapChain()->GetImageViews()[i]);

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
        auto frameBuffers = m_Framebuffers;
        auto renderPass = m_RenderPass;

        if (m_Destroy)
        {
            Renderer::SubmitFree([frameBuffers, renderPass]()
            {
                auto device = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetLogicalDevice();
                vkDeviceWaitIdle(device->GetVulkanDevice());

                for (auto& framebuffer : frameBuffers)
                    vkDestroyFramebuffer(device->GetVulkanDevice(), framebuffer, nullptr);

                vkDestroyRenderPass(device->GetVulkanDevice(), renderPass, nullptr);
            });
        }
    }

}