#pragma once

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Renderer/Image.hpp"
#include "Lavender/Renderer/Pipeline.hpp"
#include "Lavender/Renderer/DescriptorSet.hpp"

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include <imgui.h>

namespace Lavender
{

	class VulkanImage2D : public Image2D
	{
	public:
		VulkanImage2D(uint32_t width, uint32_t height);
		VulkanImage2D(const std::filesystem::path& path);
		VulkanImage2D(Ref<DescriptorSet> set, UniformElement element, uint32_t width, uint32_t height);
		VulkanImage2D(Ref<DescriptorSet> set, UniformElement element, const std::filesystem::path& path);
		virtual ~VulkanImage2D();

		void SetData(void* data, size_t size) override;

		void Upload() override;
		void Upload(Ref<DescriptorSet> set, UniformElement element) override;

		VkFormat GetFormat() const { return VK_FORMAT_R8G8B8A8_UNORM; } // TODO: Use custom format

		VkImage GetVulkanImage() { return m_Image; }
		VkImageView GetImageView() { return m_ImageView; }
		VkSampler GetSampler() { return m_Sampler; }

	private:
		void GenerateMipmaps(VkImage& image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

	private:
		Ref<DescriptorSet> m_Set = nullptr;
		UniformElement m_Element = {};

		VkImage m_Image = VK_NULL_HANDLE;
		VmaAllocation m_Allocation = VK_NULL_HANDLE;

		VkImageView m_ImageView = VK_NULL_HANDLE;
		VkSampler m_Sampler = VK_NULL_HANDLE;

		uint32_t m_Width = 0, m_Height = 0, m_Miplevels = 0;
	};

}