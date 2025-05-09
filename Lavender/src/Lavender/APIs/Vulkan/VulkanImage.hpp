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

	VkFormat GetVulkanFormatFromImageFormat(ImageSpecification::ImageFormat format);

	class VulkanImage2D : public Image2D
	{
	public:
		VulkanImage2D() = default;
		VulkanImage2D(const ImageSpecification& specs);
		VulkanImage2D(const ImageSpecification& specs, Ref<DescriptorSet> set, UniformElement element);
		virtual ~VulkanImage2D();

		void SetData(void* data, size_t size) override;

		void Resize(uint32_t width, uint32_t height) override;

		void Upload() override;
		void Upload(Ref<DescriptorSet> set, UniformElement element) override;

		inline ImageSpecification& GetSpecification() override { return m_Specification; }
		inline uint32_t GetWidth() const override { return m_Specification.Width; }
		inline uint32_t GetHeight() const override { return m_Specification.Height; }

		#ifndef LV_DISABLE_IMGUI
		inline void* GetUIImage() override { return m_TextureID; }
		#endif

		Ref<Image2D> Copy() override;

		VkFormat GetFormat() const;
		
		inline VkImage GetVulkanImage() { return m_Image; }
		inline VkImageView GetImageView() { return m_ImageView; }
		inline VkSampler GetSampler() { return m_Sampler; }

	private:
		void CreateImage(uint32_t width, uint32_t height);
		void CreateImage(const std::filesystem::path& path);

		void GenerateMipmaps(VkImage& image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

		#ifndef LV_DISABLE_IMGUI
		void CreateUIImage();
		#endif

	private:
		ImageSpecification m_Specification = {};

		Ref<DescriptorSet> m_Set = nullptr;
		UniformElement m_Element = {};

		VkImage m_Image = VK_NULL_HANDLE;
		VmaAllocation m_Allocation = VK_NULL_HANDLE;

		VkImageView m_ImageView = VK_NULL_HANDLE;
		VkSampler m_Sampler = VK_NULL_HANDLE;

		uint32_t m_Miplevels = 1;
		#ifndef LV_DISABLE_IMGUI
		void* m_TextureID = nullptr;
		#endif
	};

}