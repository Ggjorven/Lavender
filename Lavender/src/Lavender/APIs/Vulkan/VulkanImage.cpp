#include "lvpch.h"
#include "VulkanImage.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Utils/Profiler.hpp"

#include "Lavender/Renderer/Renderer.hpp"

#include "Lavender/APIs/Vulkan/VulkanUtils.hpp"
#include "Lavender/APIs/Vulkan/VulkanAllocator.hpp"
#include "Lavender/APIs/Vulkan/VulkanPipeline.hpp"
#include "Lavender/APIs/Vulkan/VulkanContext.hpp"
#include "Lavender/APIs/Vulkan/VulkanCommands.hpp"
#include "Lavender/APIs/Vulkan/VulkanDescriptorSet.hpp"

#include <stb_image.h>

#include <backends/imgui_impl_vulkan.h>

namespace Lavender
{

	static VkImageUsageFlags GetVulkanImageUsageFromImageUsage(ImageSpecification::ImageUsageFlags usage);

	VulkanImage2D::VulkanImage2D(const ImageSpecification& specs)
		: m_Specification(specs)
	{
		switch (m_Specification.Usage)
		{
		case ImageSpecification::ImageUsage::Size:
			CreateImage(m_Specification.Width, m_Specification.Height);
			break;
		case ImageSpecification::ImageUsage::File:
			CreateImage(m_Specification.Path);
			break;

		default:
			LV_LOG_ERROR("Invalid image usage selected.");
			break;
		}
	}

	VulkanImage2D::VulkanImage2D(const ImageSpecification& specs, Ref<DescriptorSet> set, UniformElement element)
		: m_Specification(specs), m_Set(set), m_Element(element)
	{
		switch (m_Specification.Usage)
		{
		case ImageSpecification::ImageUsage::Size:
			CreateImage(m_Specification.Width, m_Specification.Height);
			break;
		case ImageSpecification::ImageUsage::File:
			CreateImage(m_Specification.Path);
			break;

		default:
			LV_LOG_ERROR("Invalid image usage selected.");
			break;
		}
	}

	VulkanImage2D::~VulkanImage2D()
	{
		auto textureID = m_TextureID;
		auto sampler = m_Sampler;
		auto imageView = m_ImageView;
		auto image = m_Image;
		auto allocation = m_Allocation;

		Renderer::SubmitFree([textureID, sampler, imageView, image, allocation]()
		{
			auto device = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetLogicalDevice()->GetVulkanDevice();
			Renderer::Wait();

			#ifndef LV_DISABLE_IMGUI
			ImGui_ImplVulkan_FreeTexture(textureID);
			#endif

			vkDestroySampler(device, sampler, nullptr);
			vkDestroyImageView(device, imageView, nullptr);

			if (image != VK_NULL_HANDLE)
				VulkanAllocator::DestroyImage(image, allocation);
		});
	}

	void VulkanImage2D::SetData(void* data, size_t size)
	{
		LV_PROFILE_SCOPE("VulkanImage2D::SetData");

		VkBuffer stagingBuffer = VK_NULL_HANDLE;
		VmaAllocation stagingBufferAllocation = VK_NULL_HANDLE;
		stagingBufferAllocation = VulkanAllocator::AllocateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY, stagingBuffer);

		void* mappedData;
		VulkanAllocator::MapMemory(stagingBufferAllocation, mappedData);
		memcpy(mappedData, data, size);
		VulkanAllocator::UnMapMemory(stagingBufferAllocation);

		VulkanAllocator::TransitionImageLayout(m_Image, GetVulkanFormatFromImageFormat(m_Specification.Format), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_Miplevels);
		VulkanAllocator::CopyBufferToImage(stagingBuffer, m_Image, m_Specification.Width, m_Specification.Height);
		GenerateMipmaps(m_Image, GetVulkanFormatFromImageFormat(m_Specification.Format), m_Specification.Width, m_Specification.Height, m_Miplevels);

		VulkanAllocator::DestroyBuffer(stagingBuffer, stagingBufferAllocation);
	}

	void VulkanImage2D::Resize(uint32_t width, uint32_t height)
	{
		//LV_LOG_TRACE("(VulkanImage2D::Resize)");

		auto textureID = m_TextureID;
		auto sampler = m_Sampler;
		auto imageView = m_ImageView;
		auto image = m_Image;
		auto allocation = m_Allocation;

		Renderer::SubmitFree([textureID, sampler, imageView, image, allocation]()
		{
			auto device = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetLogicalDevice()->GetVulkanDevice();
			Renderer::Wait();

			#ifndef LV_DISABLE_IMGUI
			ImGui_ImplVulkan_FreeTexture(textureID);
			#endif

			vkDestroySampler(device, sampler, nullptr);
			vkDestroyImageView(device, imageView, nullptr);

			if (image != VK_NULL_HANDLE)
				VulkanAllocator::DestroyImage(image, allocation);
		});

		//////////////////////////////////////////////////////////////////////
		CreateImage(width, height);
	}

	void VulkanImage2D::Upload()
	{
		Upload(m_Set, m_Element);
	}

	void VulkanImage2D::Upload(Ref<DescriptorSet> set, UniformElement element)
	{
		LV_PROFILE_SCOPE("VulkanImage2D::Upload");

		auto vkSet = RefHelper::RefAs<VulkanDescriptorSet>(set);

		for (size_t i = 0; i < Renderer::GetSpecification().FramesInFlight; i++)
		{
			VkDescriptorImageInfo imageInfo = {};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = m_ImageView;
			imageInfo.sampler = m_Sampler;

			VkWriteDescriptorSet descriptorWrite = {};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = vkSet->GetVulkanSet((uint32_t)i);
			descriptorWrite.dstBinding = element.Binding;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrite.descriptorCount = element.Count;
			descriptorWrite.pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetLogicalDevice()->GetVulkanDevice(), 1, &descriptorWrite, 0, nullptr);
		}
	}

	Ref<Image2D> VulkanImage2D::Copy()
	{
		Ref<VulkanImage2D> newImage = RefHelper::Create<VulkanImage2D>();
		newImage->m_Specification = m_Specification;
		newImage->m_Miplevels = m_Miplevels;
		newImage->m_Element = m_Element;

		newImage->m_Allocation = VulkanAllocator::CreateImage(m_Specification.Width, m_Specification.Height, m_Miplevels, GetVulkanFormatFromImageFormat(m_Specification.Format), VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | GetVulkanImageUsageFromImageUsage(m_Specification.Flags), VMA_MEMORY_USAGE_GPU_ONLY, newImage->m_Image);
		VulkanAllocator::TransitionImageLayout(newImage->m_Image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_Miplevels);

		VulkanAllocator::TransitionImageLayout(m_Image, GetFormat(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, m_Miplevels);
		VulkanAllocator::TransitionImageLayout(newImage->m_Image, newImage->GetFormat(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, newImage->m_Miplevels);

		VkCommandBuffer commandBuffer = VulkanCommands::Begin();

		for (uint32_t i = 0; i < m_Miplevels; i++) 
		{
			VkImageCopy copyRegion = {};
			copyRegion.srcSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, i, 0, 1 };
			copyRegion.dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, i, 0, 1 };
			copyRegion.extent.width = m_Specification.Width >> i;		// Halfs the size
			copyRegion.extent.height = m_Specification.Height >> i;	// Halfs the size
			copyRegion.extent.depth = 1;

			vkCmdCopyImage(commandBuffer, m_Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, newImage->m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);
		}

		VulkanCommands::EndAndSubmit(commandBuffer);

		VulkanAllocator::TransitionImageLayout(m_Image, GetFormat(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_Miplevels);
		VulkanAllocator::TransitionImageLayout(newImage->m_Image, newImage->GetFormat(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, newImage->m_Miplevels);

		newImage->m_ImageView = VulkanAllocator::CreateImageView(newImage->m_Image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, newImage->m_Miplevels);
		newImage->m_Sampler = VulkanAllocator::CreateSampler(newImage->m_Miplevels);

		newImage->CreateUIImage();

		return newImage;
	}

	VkFormat VulkanImage2D::GetFormat() const
	{
		return GetVulkanFormatFromImageFormat(m_Specification.Format);
	}

	void VulkanImage2D::CreateImage(uint32_t width, uint32_t height)
	{
		m_Specification.Width = width;
		m_Specification.Height = height;
		if (!(m_Specification.Flags & ImageSpecification::ImageUsageFlags::NoMipMaps))
			m_Miplevels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;

		m_Allocation = VulkanAllocator::CreateImage(width, height, m_Miplevels, GetVulkanFormatFromImageFormat(m_Specification.Format), VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | GetVulkanImageUsageFromImageUsage(m_Specification.Flags), VMA_MEMORY_USAGE_GPU_ONLY, m_Image);

		m_ImageView = VulkanAllocator::CreateImageView(m_Image, GetVulkanFormatFromImageFormat(m_Specification.Format), VK_IMAGE_ASPECT_COLOR_BIT, m_Miplevels);
		m_Sampler = VulkanAllocator::CreateSampler(m_Miplevels);

		VulkanAllocator::TransitionImageLayout(m_Image, GetVulkanFormatFromImageFormat(m_Specification.Format), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_Miplevels);

		#ifndef LV_DISABLE_IMGUI
		CreateUIImage();
		#endif
	}

	void VulkanImage2D::CreateImage(const std::filesystem::path& path)
	{
		int width, height, texChannels;
		stbi_uc* pixels = stbi_load(path.string().c_str(), &width, &height, &texChannels, STBI_rgb_alpha);
		if (pixels == nullptr)
		{
			LV_LOG_ERROR("Failed to load image from '{0}'", path.string());
			return;
		}

		m_Specification.Width = width;
		m_Specification.Height = height;
		if (!(m_Specification.Flags & ImageSpecification::ImageUsageFlags::NoMipMaps))
			m_Miplevels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;

		m_Specification.Format = ImageSpecification::ImageFormat::RGBA;
		size_t imageSize = m_Specification.Width * m_Specification.Height * 4;

		m_Allocation = VulkanAllocator::CreateImage(m_Specification.Width, m_Specification.Height, m_Miplevels, GetVulkanFormatFromImageFormat(m_Specification.Format), VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | GetVulkanImageUsageFromImageUsage(m_Specification.Flags), VMA_MEMORY_USAGE_GPU_ONLY, m_Image);

		m_ImageView = VulkanAllocator::CreateImageView(m_Image, GetVulkanFormatFromImageFormat(m_Specification.Format), VK_IMAGE_ASPECT_COLOR_BIT, m_Miplevels);
		m_Sampler = VulkanAllocator::CreateSampler(m_Miplevels);

		#ifndef LV_DISABLE_IMGUI
		CreateUIImage();
		#endif

		SetData((void*)pixels, imageSize);
		stbi_image_free((void*)pixels);
	}

	void VulkanImage2D::GenerateMipmaps(VkImage& image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels)
	{
		// Check if image format supports linear blitting
		VkFormatProperties formatProperties;
		vkGetPhysicalDeviceFormatProperties(RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetPhysicalDevice()->GetVulkanPhysicalDevice(), imageFormat, &formatProperties);

		if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
			LV_LOG_ERROR("Texture image format does not support linear blitting!");

		VkCommandBuffer commandBuffer = VulkanCommands::Begin();

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

		VulkanCommands::EndAndSubmit(commandBuffer);
	}

	#ifndef LV_DISABLE_IMGUI
	void VulkanImage2D::CreateUIImage()
	{
		m_TextureID = ImGui_ImplVulkan_AddTexture(m_Sampler, m_ImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}
	#endif

	VkFormat GetVulkanFormatFromImageFormat(ImageSpecification::ImageFormat format)
	{
		switch (format)
		{
		case ImageSpecification::ImageFormat::RGBA: 
			return VK_FORMAT_R8G8B8A8_UNORM;
		case ImageSpecification::ImageFormat::sRGB:
			return VK_FORMAT_R8G8B8A8_SRGB;
		case ImageSpecification::ImageFormat::BGRA:
			return VK_FORMAT_B8G8R8A8_UNORM;

		default:
			LV_LOG_ERROR("Invalid image format.");
			break;
		}

		return VK_FORMAT_UNDEFINED;
	}

	static VkImageUsageFlags GetVulkanImageUsageFromImageUsage(ImageSpecification::ImageUsageFlags usage)
	{
		VkImageUsageFlags flags = 0;

		if (usage & ImageSpecification::ImageUsageFlags::Colour)
			flags = flags | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		if (usage & ImageSpecification::ImageUsageFlags::Depth)
			flags = flags | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		if (usage & ImageSpecification::ImageUsageFlags::Sampled)
			flags = flags | VK_IMAGE_USAGE_SAMPLED_BIT;
		if (usage & ImageSpecification::ImageUsageFlags::Storage)
			flags = flags | VK_IMAGE_USAGE_STORAGE_BIT;
		if (usage & ImageSpecification::ImageUsageFlags::Transient)
			flags = flags | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
		if (usage & ImageSpecification::ImageUsageFlags::Input)
			flags = flags | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;

		return flags;
	}


}