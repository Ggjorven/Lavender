#include "lvpch.h"
#include "Viewport.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/Renderer.hpp"

#include "Lavender/APIs/Vulkan/VulkanViewport.hpp"

namespace Lavender
{

	Ref<Viewport> Viewport::Create(uint32_t width, uint32_t height)
	{
		switch (Renderer::GetAPI())
		{
		case RenderingAPI::Vulkan:
			return RefHelper::Create<VulkanViewport>(width, height);

		default:
			LV_LOG_ERROR("Invalid API selected.");
			break;
		}

		return nullptr;
	}

}