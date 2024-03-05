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

	VmaAllocation VulkanAllocator::CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VmaMemoryUsage memUsage, VkImage& image)
	{
		VkImageCreateInfo imageInfo = {};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = mipLevels;
		imageInfo.arrayLayers = 1;
		imageInfo.format = format;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usage;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocCreateInfo = {};
		allocCreateInfo.usage = memUsage;

		VmaAllocation allocation;
		vmaCreateImage(s_Allocator, &imageInfo, &allocCreateInfo, &image, &allocation, nullptr);

		return allocation;
	}

	void VulkanAllocator::TransitionImageLayout(VkImage& image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels)
	{
		VkCommandBuffer commandBuffer = VulkanCommands::Begin();

		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		barrier.image = image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = mipLevels;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		VkPipelineStageFlags sourceStage = {};
		VkPipelineStageFlags destinationStage = {};

		// New layout checks
		if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

			if (HasStencilComponent(format))
				barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
		else if (newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
		{
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		}
		else
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

		// Old layout checks
		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
		{
			barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		}
		else
			LV_LOG_ERROR("Unsupported layout transition!");

		vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

		VulkanCommands::EndAndSubmit(commandBuffer);
	}

	void VulkanAllocator::CopyBufferToImage(VkBuffer& buffer, VkImage& image, uint32_t width, uint32_t height)
	{
		VkCommandBuffer commandBuffer = VulkanCommands::Begin();

		VkBufferImageCopy region = {};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = { width, height, 1 };

		vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		VulkanCommands::EndAndSubmit(commandBuffer);
	}

	VkImageView VulkanAllocator::CreateImageView(VkImage& image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels)
	{
		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = mipLevels;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;
		viewInfo.subresourceRange.aspectMask = aspectFlags;

		VkImageView imageView;
		if (vkCreateImageView(RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetLogicalDevice()->GetVulkanDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS)
			LV_LOG_ERROR("Failed to create texture image view!");

		return imageView;
	}

	void VulkanAllocator::DestroyImage(VkImage& image, VmaAllocation& allocation)
	{
		vmaDestroyImage(s_Allocator, image, allocation);
	}

	VkSampler VulkanAllocator::CreateSampler(uint32_t mipLevels)
	{
		VkSamplerCreateInfo samplerInfo = {};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

		VkPhysicalDeviceProperties properties = {};
		vkGetPhysicalDeviceProperties(RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetPhysicalDevice()->GetVulkanPhysicalDevice(), &properties);

		samplerInfo.anisotropyEnable = VK_TRUE;								// Can be disabled: just set VK_FALSE
		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy; // And 1.0f

		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.minLod = 0.0f; 
		samplerInfo.maxLod = (float)mipLevels;
		samplerInfo.mipLodBias = 0.0f; // Optional

		VkSampler sampler;
		if (vkCreateSampler(RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetLogicalDevice()->GetVulkanDevice(), &samplerInfo, nullptr, &sampler) != VK_SUCCESS)
			LV_LOG_ERROR("Failed to create texture sampler!");

		return sampler;
	}

	void VulkanAllocator::MapMemory(VmaAllocation& allocation, void*& mapData)
	{
		vmaMapMemory(s_Allocator, allocation, &mapData);
	}

	void VulkanAllocator::UnMapMemory(VmaAllocation& allocation)
	{
		vmaUnmapMemory(s_Allocator, allocation);
	}

	uint32_t VulkanAllocator::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties = {};
		vkGetPhysicalDeviceMemoryProperties(RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetPhysicalDevice()->GetVulkanPhysicalDevice(), &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
				return i;
		}

		LV_LOG_ERROR("Failed to find suitable memory type!");
		return -1;
	}

	bool VulkanAllocator::HasStencilComponent(VkFormat format)
	{
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}

	VkFormat VulkanAllocator::FindDepthFormat()
	{
		return FindSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	}

	VkFormat VulkanAllocator::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
	{
		for (VkFormat format : candidates)
		{
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetPhysicalDevice()->GetVulkanPhysicalDevice(), format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
				return format;
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
				return format;
		}

		LV_LOG_ERROR("Failed to find supported format!");
		return VK_FORMAT_UNDEFINED;
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