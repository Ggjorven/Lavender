#include "lvpch.h"
#include "VulkanAllocator.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/Renderer.hpp"
#include "Lavender/APIs/Vulkan/VulkanContext.hpp"
#include "Lavender/APIs/Vulkan/VulkanCommands.hpp"

namespace Lavender
{

	static VmaAllocator s_Allocator = VK_NULL_HANDLE;

	VmaAllocation VulkanAllocator::AllocateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage, VkBuffer& dstBuffer)
	{
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // Change if necessary

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = memoryUsage; // VMA_MEMORY_USAGE_GPU_ONLY, VMA_MEMORY_USAGE_CPU_ONLY, etc.

		VmaAllocation allocation = VK_NULL_HANDLE;
		if (vmaCreateBuffer(s_Allocator, &bufferInfo, &allocInfo, &dstBuffer, &allocation, nullptr) != VK_SUCCESS)
			LV_LOG_ERROR("Failed to allocate buffer.");

		return allocation;
	}

	void VulkanAllocator::CopyBuffer(VkBuffer& srcBuffer, VkBuffer& dstBuffer, VkDeviceSize size)
	{
		VkCommandBuffer buffer = VulkanCommands::Begin();

		VkBufferCopy copyRegion = {};
		copyRegion.size = size;
		vkCmdCopyBuffer(buffer, srcBuffer, dstBuffer, 1, &copyRegion);

		VulkanCommands::EndAndSubmit(buffer);
	}

	void VulkanAllocator::DestroyBuffer(VkBuffer& buffer, VmaAllocation& allocation)
	{
		vmaDestroyBuffer(s_Allocator, buffer, allocation);
	}

	void VulkanAllocator::MapMemory(VmaAllocation& allocation, void*& mapData)
	{
		vmaMapMemory(s_Allocator, allocation, &mapData);
	}

	void VulkanAllocator::UnMapMemory(VmaAllocation& allocation)
	{
		vmaUnmapMemory(s_Allocator, allocation);
	}

	void VulkanAllocator::Init()
	{
		auto context = RefHelper::RefAs<VulkanContext>(Renderer::GetContext());

		VmaAllocatorCreateInfo allocatorInfo = {};
		allocatorInfo.instance = context->GetVulkanInstance();
		allocatorInfo.physicalDevice = context->GetPhysicalDevice()->GetVulkanPhysicalDevice();
		allocatorInfo.device = context->GetLogicalDevice()->GetVulkanDevice();

		if (vmaCreateAllocator(&allocatorInfo, &s_Allocator) != VK_SUCCESS)
			LV_LOG_ERROR("Failed to create VMA allocator!");
	}

	void VulkanAllocator::Destroy()
	{
		if (s_Allocator != VK_NULL_HANDLE)
		{
			vmaDestroyAllocator(s_Allocator);
			s_Allocator = VK_NULL_HANDLE;
		}
	}

}