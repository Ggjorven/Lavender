#include "lvpch.h"
#include "Image.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/Renderer.hpp"

#include "Lavender/APIs/Vulkan/VulkanImage.hpp"

namespace Lavender
{

	Ref<Image2D> Image2D::Create(Ref<Pipeline> pipeline, UniformElement element, uint32_t width, uint32_t height)
	{
		switch (Renderer::GetAPI())
		{
		case RenderingAPI::Vulkan:
			return RefHelper::Create<VulkanImage2D>(pipeline, element, width, height);

		default:
			LV_LOG_ERROR("Invalid API selected.");
			break;
		}

		return nullptr;
	}

	Ref<Image2D> Image2D::Create(Ref<Pipeline> pipeline, UniformElement element, const std::filesystem::path& path)
	{
		switch (Renderer::GetAPI())
		{
		case RenderingAPI::Vulkan:
			return RefHelper::Create<VulkanImage2D>(pipeline, element, path);

		default:
			LV_LOG_ERROR("Invalid API selected.");
			break;
		}

		return nullptr;
	}

	Ref<Image2D> Image2D::Create(uint32_t width, uint32_t height)
	{
		switch (Renderer::GetAPI())
		{
		case RenderingAPI::Vulkan:
			return RefHelper::Create<VulkanImage2D>(width, height);

		default:
			LV_LOG_ERROR("Invalid API selected.");
			break;
		}

		return nullptr;
	}

}