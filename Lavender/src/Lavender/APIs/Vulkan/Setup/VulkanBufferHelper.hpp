#pragma once

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Lavender
{

	class VulkanBufferHelper
	{
	public:
		static void Init();
		static void Destroy();

		static void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage, VkBuffer& dstBuffer, VmaAllocation& dstBufferAllocation);
		static void CopyBuffer(VkBuffer& srcBuffer, VkBuffer& dstBuffer, VkDeviceSize size);

		static uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

		static bool HasStencilComponent(VkFormat format);

		static VmaAllocator& GetAllocator() { return s_Allocator; }

	private:
		static VmaAllocator s_Allocator;
	};

}