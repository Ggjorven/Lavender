#include "lvpch.h"
#include "VulkanActions.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/APIs/Vulkan/VulkanManager.hpp"

namespace Lavender
{

	VkCommandBuffer VulkanActions::BeginSingleTimeCommands()
	{
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = VulkanManager::GetResourceInfo().CommandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		if (vkAllocateCommandBuffers(VulkanManager::GetDeviceInfo().Device, &allocInfo, &commandBuffer))
			LV_LOG_WARN("Failed to create single time command buffer!");

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		return commandBuffer;
	}

	void VulkanActions::EndSingleTimeCommands(VkCommandBuffer& buffer)
	{
		VulkanDeviceInfo& info = VulkanManager::GetDeviceInfo();

		vkEndCommandBuffer(buffer);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &buffer;

		vkQueueSubmit(info.GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(info.GraphicsQueue);

		vkFreeCommandBuffers(info.Device, VulkanManager::GetResourceInfo().CommandPool, 1, &buffer);
	}

}