#include "lvpch.h"
#include "RenderingContext.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/Renderer.hpp"

#include "Lavender/APIs/Vulkan/VulkanContext.hpp"

namespace Lavender
{

	std::shared_ptr<RenderingContext> RenderingContext::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RenderingAPI::Vulkan:
			return std::make_shared<VulkanContext>();

		default:
			LV_LOG_ERROR("Invalid API selected.");
			break;
		}

		return nullptr;
	}

}