#pragma once

#include "Lavender/Utils/Utils.hpp"

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Lavender
{

	class VulkanAllocator
	{
	public:
		VulkanAllocator() = default;
		virtual ~VulkanAllocator() = default;

		static VmaAllocation AllocateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage, VkBuffer& dstBuffer);
		static void CopyBuffer(VkBuffer& srcBuffer, VkBuffer& dstBuffer, VkDeviceSize size);
		static void DestroyBuffer(VkBuffer& buffer, VmaAllocation& allocation);

		static void MapMemory(VmaAllocation& allocation, void*& mapData);
		static void UnMapMemory(VmaAllocation& allocation);

		static void Init();
		static void Destroy();
	};

}