#include "lvpch.h"
#include "VulkanResources.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/APIs/Vulkan/VulkanManager.hpp"
#include "Lavender/APIs/Vulkan/Setup/VulkanHelper.hpp"
#include "Lavender/APIs/Vulkan/Setup/VulkanImageHelper.hpp"
#include "Lavender/APIs/Vulkan/Setup/VulkanBufferHelper.hpp"
#include "Lavender/Renderer/GraphicsContext.hpp"

namespace Lavender
{

    VulkanResources::VulkanResources()
    {
        CreateCommandPool();
        CreateCommandBuffers();
        CreateSyncObjects();

        VulkanManager::PopulateResourceInfo(this);
        CreateDepthResources();
    }

    VulkanResources::~VulkanResources()
    {
        VulkanDeviceInfo& info = VulkanManager::GetDeviceInfo();

        vkDeviceWaitIdle(info.Device);

        // Cleanup depth resources
        vkDestroyImageView(info.Device, m_DepthImageView, nullptr);
        if (m_DepthImage != VK_NULL_HANDLE)
            vmaDestroyImage(VulkanBufferHelper::GetAllocator(), m_DepthImage, m_DepthImageMemory);

        for (size_t i = 0; i < LV_MAX_FRAMES_IN_FLIGHT; i++)
        {
            vkDestroySemaphore(info.Device, m_RenderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(info.Device, m_ImageAvailableSemaphores[i], nullptr);
            vkDestroyFence(info.Device, m_InFlightFences[i], nullptr);
        }

        // Note(Jorben): This is not necessary since the commandbuffers would be automatically free'd when free'ing the commandpool
        // I prefer it this way since it's clearer to see when they are created and destroyed.
        for (size_t i = 0; i < LV_MAX_FRAMES_IN_FLIGHT; i++)
            vkFreeCommandBuffers(info.Device, m_CommandPool, 1, &m_CommandBuffers[i]);

        vkDestroyCommandPool(info.Device, m_CommandPool, nullptr);
    }

    void VulkanResources::AddRenderPass(VulkanRenderPass* renderpass)
    {
        m_RenderPasses.push_back(renderpass);
    }

    void VulkanResources::RecreateDepthResources()
    {
        VulkanDeviceInfo& info = VulkanManager::GetDeviceInfo();

        // Cleanup depth resources
        vkDestroyImageView(info.Device, m_DepthImageView, nullptr);
        if (m_DepthImage != VK_NULL_HANDLE)
            vmaDestroyImage(VulkanBufferHelper::GetAllocator(), m_DepthImage, m_DepthImageMemory);

        CreateDepthResources();
    }

    void VulkanResources::RecreateFramebuffers()
    {
        for (auto renderpass : m_RenderPasses)
            renderpass->RecreateFrameBuffers();
    }

    void VulkanResources::CreateCommandPool()
    {
        VulkanDeviceInfo& info = VulkanManager::GetDeviceInfo();

        VulkanHelper::QueueFamilyIndices queueFamilyIndices = VulkanHelper::QueueFamilyIndices::Find(GraphicsContext::Get()->GetInstance(), info.PhysicalDevice);

        VkCommandPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndices.GraphicsFamily.value();

        if (vkCreateCommandPool(info.Device, &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS)
            LV_LOG_ERROR("Failed to create command pool!");
    }

    void VulkanResources::CreateCommandBuffers()
    {
        VulkanDeviceInfo& info = VulkanManager::GetDeviceInfo();

        m_CommandBuffers.resize(LV_MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = m_CommandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t)m_CommandBuffers.size();

        if (vkAllocateCommandBuffers(info.Device, &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS)
            LV_LOG_ERROR("Failed to allocate command buffers!");
    }

    void VulkanResources::CreateSyncObjects()
    {
        VulkanDeviceInfo& info = VulkanManager::GetDeviceInfo();

        m_ImageAvailableSemaphores.resize(LV_MAX_FRAMES_IN_FLIGHT);
        m_RenderFinishedSemaphores.resize(LV_MAX_FRAMES_IN_FLIGHT);
        m_InFlightFences.resize(LV_MAX_FRAMES_IN_FLIGHT);

        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < LV_MAX_FRAMES_IN_FLIGHT; i++)
        {
            if (vkCreateSemaphore(info.Device, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(info.Device, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(info.Device, &fenceInfo, nullptr, &m_InFlightFences[i]) != VK_SUCCESS)
            {
                LV_LOG_ERROR("Failed to create synchronization objects for a frame!");
            }
        }
    }

    void VulkanResources::CreateDepthResources()
    {
        VulkanSwapChainInfo& info = VulkanManager::GetSwapChainInfo();

        VkFormat depthFormat = VulkanHelper::FindDepthFormat();

        uint32_t mips = 1;
        VulkanImageHelper::CreateImage(info.SwapChainExtent.width, info.SwapChainExtent.height, mips, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VMA_MEMORY_USAGE_GPU_ONLY, m_DepthImage, m_DepthImageMemory);

        m_DepthImageView = VulkanImageHelper::CreateImageView(m_DepthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);

        VulkanImageHelper::TransitionImageLayout(m_DepthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);
    }

}