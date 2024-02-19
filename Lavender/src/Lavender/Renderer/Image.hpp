#pragma once

#include <memory>
#include <filesystem>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include "Lavender/Renderer/UniformBuffer.hpp"
#include "Lavender/Renderer/RenderController.hpp"

namespace Lavender
{

	// TODO(Jorben): Make usable with ImGui?
	class Image
	{
	public:
		Image(std::shared_ptr<RenderController>& controller, UniformElement& element, uint32_t width, uint32_t height);
		Image(std::shared_ptr<RenderController>& controller, UniformElement& element, const std::filesystem::path& path);
		virtual ~Image();

		void SetData(void* data, size_t size);

		// Note(Jorben): This function only needs to be called once, when initializing.
		void UploadToController();

	private:
		void GenerateMipmaps(VkImage& image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

		VkSampler CreateSampler(uint32_t mipLevels);

	private:
		std::shared_ptr<RenderController> m_RenderController = nullptr;
		UniformElement m_Element = {};

		VkImage m_Image = VK_NULL_HANDLE;
		VmaAllocation m_Allocation = VK_NULL_HANDLE;

		VkImageView m_ImageView = VK_NULL_HANDLE;
		VkSampler m_Sampler = VK_NULL_HANDLE;

		uint32_t m_Width = 0, m_Height = 0, m_Miplevels = 0;
	};

}