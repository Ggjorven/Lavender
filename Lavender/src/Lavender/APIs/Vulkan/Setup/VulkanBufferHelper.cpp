#include "lvpch.h"
#include "VulkanImageHelper.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/APIs/Vulkan/VulkanManager.hpp"
#include "Lavender/APIs/Vulkan/VulkanActions.hpp"
#include "VulkanBufferHelper.hpp"

namespace Lavender
{

	VmaAllocator VulkanBufferHelper::s_Allocator = VK_NULL_HANDLE;

	void VulkanBufferHelper::Init()
	{
		VulkanDeviceInfo& info = VulkanManager::GetDeviceInfo();

		VmaAllocatorCreateInfo allocatorInfo = {};
		allocatorInfo.instance = info.Instance;
		allocatorInfo.physicalDevice = info.PhysicalDevice;
		allocatorInfo.device = info.Device;

		if (vmaCreateAllocator(&allocatorInfo, &s_Allocator) != VK_SUCCESS)
			LV_LOG_ERROR("Failed to create VMA allocator!");
	}

	void VulkanBufferHelper::Destroy()
	{
		if (s_Allocator != VK_NULL_HANDLE) 
		{
			vmaDestroyAllocator(s_Allocator);
			s_Allocator = VK_NULL_HANDLE;
		}
	}

	void VulkanBufferHelper::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage, VkBuffer& dstBuffer, VmaAllocation& dstBufferAllocation)
	{
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // Change if necessary

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = memoryUsage; // VMA_MEMORY_USAGE_GPU_ONLY, VMA_MEMORY_USAGE_CPU_ONLY, etc.

		if (vmaCreateBuffer(s_Allocator, &bufferInfo, &allocInfo, &dstBuffer, &dstBufferAllocation, nullptr) != VK_SUCCESS)
			LV_LOG_ERROR("Failed to allocate buffer.");
	}


	void VulkanBufferHelper::CopyBuffer(VkBuffer& srcBuffer, VkBuffer& dstBuffer, VkDeviceSize size)
	{
		VkCommandBuffer buffer = VulkanActions::BeginSingleTimeCommands();

		VkBufferCopy copyRegion = {};
		copyRegion.size = size;
		vkCmdCopyBuffer(buffer, srcBuffer, dstBuffer, 1, &copyRegion);

		VulkanActions::EndSingleTimeCommands(buffer);
	}

	uint32_t VulkanBufferHelper::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties = {};
		vkGetPhysicalDeviceMemoryProperties(VulkanManager::GetDeviceInfo().PhysicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
				return i;
		}

		LV_LOG_ERROR("Failed to find suitable memory type!");
		return -1;
	}

	bool VulkanBufferHelper::HasStencilComponent(VkFormat format)
	{
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}

}