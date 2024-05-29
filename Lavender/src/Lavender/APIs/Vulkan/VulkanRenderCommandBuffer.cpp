#include "lvpch.h"
#include "VulkanRenderCommandBuffer.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Utils/Profiler.hpp"

#include "Lavender/Renderer/Renderer.hpp"

#include "Lavender/APIs//Vulkan/VulkanUtils.hpp"
#include "Lavender/APIs/Vulkan/VulkanContext.hpp"
#include "Lavender/APIs/Vulkan/VulkanRenderer.hpp"
#include "Lavender/APIs/Vulkan/VulkanSwapChain.hpp"

namespace Lavender
{

	static VkSemaphore s_LatestSemaphore = VK_NULL_HANDLE;

	VulkanRenderCommandBuffer::VulkanRenderCommandBuffer(CommandBufferSpecification specs)
		: m_Specification(specs)
	{
		// Check if specs are set properly
		if (!(m_Specification.Flags & CommandBufferSpecification::UsageFlags::WaitForLatest) && !(m_Specification.Flags & CommandBufferSpecification::UsageFlags::NoWaiting))
			LV_LOG_ERROR("No proper flags set.");

		auto device = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetLogicalDevice()->GetVulkanDevice();
		uint32_t framesInFlight = Renderer::GetSpecification().FramesInFlight;
		m_CommandBuffers.resize(framesInFlight);

		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetSwapChain()->GetCommandPool();
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t)m_CommandBuffers.size();

		if (vkAllocateCommandBuffers(device, &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS)
			LV_LOG_ERROR("Failed to allocate command buffers!");

		m_RenderFinishedSemaphores.resize(framesInFlight);
		m_InFlightFences.resize(framesInFlight);

		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo = {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < framesInFlight; i++)
		{
			if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]) != VK_SUCCESS ||
				vkCreateFence(device, &fenceInfo, nullptr, &m_InFlightFences[i]) != VK_SUCCESS)
			{
				LV_LOG_ERROR("Failed to create synchronization objects for a frame!");
			}
		}
	}

	VulkanRenderCommandBuffer::~VulkanRenderCommandBuffer()
	{
		auto commandBuffers = m_CommandBuffers;
		auto renderFinishedSemaphores = m_RenderFinishedSemaphores;
		auto inFlightFences = m_InFlightFences;

		Renderer::SubmitFree([commandBuffers, renderFinishedSemaphores, inFlightFences]() 
		{
			auto device = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetLogicalDevice()->GetVulkanDevice();

			vkDeviceWaitIdle(device);

			vkFreeCommandBuffers(device, RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetSwapChain()->GetCommandPool(), Renderer::GetSpecification().FramesInFlight, commandBuffers.data());

			for (size_t i = 0; i < Renderer::GetSpecification().FramesInFlight; i++)
			{
				vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
				vkDestroyFence(device, inFlightFences[i], nullptr);
			}
		});
	}

	void VulkanRenderCommandBuffer::Begin()
	{
		auto device = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetLogicalDevice()->GetVulkanDevice();
		uint32_t currentFrame = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetSwapChain()->GetCurrentFrame();
		VkCommandBuffer commandBuffer = m_CommandBuffers[currentFrame];

		vkResetFences(device, 1, &m_InFlightFences[currentFrame]);
		vkResetCommandBuffer(commandBuffer, 0);

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		{
			LV_PROFILE_SCOPE("VulkanRenderCommandBuffer::Begin::Begin");
			if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
				LV_LOG_ERROR("Failed to begin recording command buffer!");
		}
	}

	void VulkanRenderCommandBuffer::End()
	{
		uint32_t currentFrame = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetSwapChain()->GetCurrentFrame();
		VkCommandBuffer commandBuffer = m_CommandBuffers[currentFrame];

		{
			LV_PROFILE_SCOPE("VulkanRenderCommandBuffer::End::End");
			if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
				LV_LOG_ERROR("Failed to record command buffer!");
		}
	}

	void VulkanRenderCommandBuffer::Submit()
	{
		VkDevice device = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetLogicalDevice()->GetVulkanDevice();

		uint32_t currentFrame = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetSwapChain()->GetCurrentFrame();
		VkCommandBuffer commandBuffer = m_CommandBuffers[currentFrame];

		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		if (m_Specification.Flags & CommandBufferSpecification::UsageFlags::WaitForLatest)
		{
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = &s_LatestSemaphore;
		}
		if (m_Specification.Flags & CommandBufferSpecification::UsageFlags::NoWaiting)
		{
			submitInfo.waitSemaphoreCount = 0;
			submitInfo.pWaitSemaphores = nullptr;
		}

		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &m_RenderFinishedSemaphores[currentFrame];

		{
			LV_PROFILE_SCOPE("VulkanRenderCommandBuffer::Submit::Queue");
			auto logicalDevice = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetLogicalDevice();
			//vkQueueWaitIdle(logicalDevice->GetGraphicsQueue());
			VkResult result = vkQueueSubmit(logicalDevice->GetGraphicsQueue(), 1, &submitInfo, m_InFlightFences[currentFrame]);
			if (result != VK_SUCCESS)
				LV_LOG_ERROR("Failed to submit draw command buffer! Error: {0}", VKResultToString(result));
		}

		if (m_Specification.Flags & CommandBufferSpecification::UsageFlags::WaitForLatest)
			s_LatestSemaphore = m_RenderFinishedSemaphores[currentFrame]; // For the next commandBuffer
	}

	VkCommandBuffer VulkanRenderCommandBuffer::GetVulkanCommandBuffer()
	{
		uint32_t currentFrame = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetSwapChain()->GetCurrentFrame();

		return m_CommandBuffers[currentFrame];
	}

	void VulkanRenderCommandBuffer::ResetSemaphore()
	{
		s_LatestSemaphore = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetSwapChain()->GetCurrentImageAvailableSemaphore();
	}

	VkSemaphore VulkanRenderCommandBuffer::GetSemaphore()
	{
		return s_LatestSemaphore;
	}

}