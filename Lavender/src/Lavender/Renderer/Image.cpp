#include "lvpch.h"
#include "Image.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/APIs/Vulkan/Setup/VulkanResources.hpp"
#include "Lavender/APIs/Vulkan/Setup/VulkanBufferHelper.hpp"
#include "Lavender/APIs/Vulkan/Setup/VulkanImageHelper.hpp"
#include "Lavender/APIs/Vulkan/VulkanManager.hpp"
#include "Lavender/APIs/Vulkan/VulkanActions.hpp"
#include "Lavender/Renderer/RenderController.hpp"

#include <stb_image.h>
#include "Image.hpp"

namespace Lavender
{

	Image::Image(std::shared_ptr<RenderController>& controller, UniformElement& element, uint32_t width, uint32_t height)
		: m_RenderController(controller), m_Element(element), m_Width(width), m_Height(height)
	{
		VulkanImageHelper::CreateImage(width, height, m_Miplevels, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VMA_MEMORY_USAGE_GPU_ONLY, m_Image, m_Allocation);

		m_ImageView = VulkanImageHelper::CreateImageView(m_Image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, m_Miplevels);
		m_Sampler = CreateSampler(m_Miplevels);
	}

	Image::Image(std::shared_ptr<RenderController>& controller, UniformElement& element, const std::filesystem::path& path)
		: m_RenderController(controller), m_Element(element)
	{
		int width, height, texChannels;
		stbi_uc* pixels = stbi_load(path.string().c_str(), &width, &height, &texChannels, STBI_rgb_alpha);
		m_Width = width;
		m_Height = height;

		size_t imageSize = m_Width * m_Height * 4;

		VulkanImageHelper::CreateImage(m_Width, m_Height, m_Miplevels, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VMA_MEMORY_USAGE_GPU_ONLY, m_Image, m_Allocation);

		m_ImageView = VulkanImageHelper::CreateImageView(m_Image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, m_Miplevels);
		m_Sampler = CreateSampler(m_Miplevels);

		SetData((void*)pixels, imageSize);
	}

	Image::~Image()
	{
		vkDestroySampler(VulkanManager::GetDeviceInfo().Device, m_Sampler, nullptr);
		vkDestroyImageView(VulkanManager::GetDeviceInfo().Device, m_ImageView, nullptr);

		if (m_Image != VK_NULL_HANDLE)
			vmaDestroyImage(VulkanBufferHelper::GetAllocator(), m_Image, m_Allocation);
	}

	void Image::SetData(void* data, size_t size)
	{
		VkBuffer stagingBuffer = VK_NULL_HANDLE;
		VmaAllocation stagingBufferAllocation = VK_NULL_HANDLE;

		VulkanBufferHelper::CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY, stagingBuffer, stagingBufferAllocation);

		void* mappedData;
		vmaMapMemory(VulkanBufferHelper::GetAllocator(), stagingBufferAllocation, &mappedData);
		memcpy(mappedData, data, size); // `data` is your image data
		vmaUnmapMemory(VulkanBufferHelper::GetAllocator(), stagingBufferAllocation);

		VulkanImageHelper::TransitionImageLayout(m_Image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_Miplevels);
		VulkanImageHelper::CopyBufferToImage(stagingBuffer, m_Image, m_Width, m_Height);
		GenerateMipmaps(m_Image, VK_FORMAT_R8G8B8A8_UNORM, m_Width, m_Height, m_Miplevels);

		vmaDestroyBuffer(VulkanBufferHelper::GetAllocator(), stagingBuffer, stagingBufferAllocation);
	}

	void Image::UploadToController()
	{
		m_RenderController->AddImage(m_ImageView, m_Sampler, m_Element.Binding, m_Element.Count);
	}

	void Image::GenerateMipmaps(VkImage& image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels)
	{
		// Check if image format supports linear blitting
		VkFormatProperties formatProperties;
		vkGetPhysicalDeviceFormatProperties(VulkanManager::GetDeviceInfo().PhysicalDevice, imageFormat, &formatProperties);

		if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
			LV_LOG_ERROR("Texture image format does not support linear blitting!");

		VkCommandBuffer commandBuffer = VulkanActions::BeginSingleTimeCommands();

		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.image = image;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.levelCount = 1;

		int32_t mipWidth = texWidth;
		int32_t mipHeight = texHeight;

		for (uint32_t i = 1; i < mipLevels; i++)
		{
			barrier.subresourceRange.baseMipLevel = i - 1;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			vkCmdPipelineBarrier(commandBuffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);

			VkImageBlit blit = {};
			blit.srcOffsets[0] = { 0, 0, 0 };
			blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
			blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.srcSubresource.mipLevel = i - 1;
			blit.srcSubresource.baseArrayLayer = 0;
			blit.srcSubresource.layerCount = 1;
			blit.dstOffsets[0] = { 0, 0, 0 };
			blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
			blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.dstSubresource.mipLevel = i;
			blit.dstSubresource.baseArrayLayer = 0;
			blit.dstSubresource.layerCount = 1;

			vkCmdBlitImage(commandBuffer,
				image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1, &blit,
				VK_FILTER_LINEAR);

			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(commandBuffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);

			if (mipWidth > 1) mipWidth /= 2;
			if (mipHeight > 1) mipHeight /= 2;
		}

		barrier.subresourceRange.baseMipLevel = mipLevels - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		VulkanActions::EndSingleTimeCommands(commandBuffer);
	}

	VkSampler Image::CreateSampler(uint32_t mipLevels)
	{
		VkSamplerCreateInfo samplerInfo = {};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

		VkPhysicalDeviceProperties properties = {};
		vkGetPhysicalDeviceProperties(VulkanManager::GetDeviceInfo().PhysicalDevice, &properties);

		samplerInfo.anisotropyEnable = VK_TRUE;								// Can be disabled: just set VK_FALSE
		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy; // And 1.0f

		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.minLod = 0.0f; // Optional: (samplerInfo.minLod = static_cast<float>(mipLevels / 2))
		samplerInfo.maxLod = static_cast<float>(mipLevels);
		samplerInfo.mipLodBias = 0.0f; // Optional

		VkSampler sampler;
		if (vkCreateSampler(VulkanManager::GetDeviceInfo().Device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS)
			LV_LOG_ERROR("Failed to create texture sampler!");

		return sampler;
	}

}