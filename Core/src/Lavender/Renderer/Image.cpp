#include "lvpch.h"
#include "Image.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/Renderer.hpp"

#include "Lavender/Vulkan/VulkanImage.hpp"

namespace Lavender
{

	ImageSpecification::ImageSpecification(uint32_t width, uint32_t height, ImageUsageFlags flags)
		: Usage(ImageUsage::Size), Width(width), Height(height), Flags(flags)
	{
	}

	ImageSpecification::ImageSpecification(const std::filesystem::path& path, ImageUsageFlags flags)
		: Usage(ImageUsage::File), Path(path), Flags(flags)
	{
	}

	Ref<Image2D> Image2D::Create(const ImageSpecification& specs)
	{
		switch (RendererSpecification::API)
		{
		case RenderingAPI::Vulkan:
			return RefHelper::Create<VulkanImage2D>(specs);

		default:
			APP_LOG_ERROR("Invalid API selected.");
			break;
		}

		return nullptr;
	}

}