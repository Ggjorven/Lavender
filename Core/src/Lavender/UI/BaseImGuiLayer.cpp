#include "lvpch.h"
#include "BaseImGuiLayer.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/Renderer.hpp"

#include "Lavender/Vulkan/VulkanImGuiLayer.hpp"

namespace Lavender
{

	BaseImGuiLayer* BaseImGuiLayer::Create()
	{
		switch (RendererSpecification::API)
		{
		case RendererSpecification::RenderingAPI::Vulkan:
			return new VulkanImGuiLayer();

		default:
			APP_LOG_ERROR("Invalid API selected.");
			break;
		}

		return nullptr;
	}

}