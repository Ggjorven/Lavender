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
		static void DestroyBuffer(VkBuffer buffer, VmaAllocation allocation);

		static VmaAllocation CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VmaMemoryUsage memUsage, VkImage& image);
		static VmaAllocation CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VmaMemoryUsage memUsage, VkMemoryPropertyFlags requiredFlags, VkImage& image);


		static void TransitionImageLayout(VkImage& image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
		static void CopyBufferToImage(VkBuffer& buffer, VkImage& image, uint32_t width, uint32_t height);
		static VkImageView CreateImageView(VkImage& image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
		static void DestroyImage(VkImage image, VmaAllocation allocation);
		static VkSampler CreateSampler(uint32_t mipLevels); // TODO: Make it usable with multiple formats and stuff.

		static void MapMemory(VmaAllocation& allocation, void*& mapData);
		static void UnMapMemory(VmaAllocation& allocation);

		static uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		static bool HasStencilComponent(VkFormat format);

		static VkFormat FindDepthFormat();
		static VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

		static void Init();
		static void Destroy();
	};

}