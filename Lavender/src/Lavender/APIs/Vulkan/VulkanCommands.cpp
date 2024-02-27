#include "lvpch.h"
#include "VulkanCommands.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/Renderer.hpp"
#include "Lavender/APIs/Vulkan/VulkanContext.hpp"

namespace Lavender
{

	VkCommandBuffer VulkanCommands::Begin()
	{
		auto context = RefHelper::RefAs<VulkanContext>(Renderer::GetContext());

		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = context->GetSwapChain()->GetCommandPool();
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		if (vkAllocateCommandBuffers(context->GetLogicalDevice()->GetVulkanDevice(), &allocInfo, &commandBuffer))
			LV_LOG_WARN("Failed to create single time command buffer!");

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		return commandBuffer;
	}

	void VulkanCommands::End(VkCommandBuffer commandBuffer)
	{
		vkEndCommandBuffer(commandBuffer);
	}

	void VulkanCommands::Submit(VkCommandBuffer commandBuffer)
	{
		auto context = RefHelper::RefAs<VulkanContext>(Renderer::GetContext());

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(context->GetLogicalDevice()->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(context->GetLogicalDevice()->GetGraphicsQueue());

		vkFreeCommandBuffers(context->GetLogicalDevice()->GetVulkanDevice(), context->GetSwapChain()->GetCommandPool(), 1, &commandBuffer);
	}

	void VulkanCommands::EndAndSubmit(VkCommandBuffer commandBuffer)
	{
		End(commandBuffer);
		Submit(commandBuffer);
	}

}