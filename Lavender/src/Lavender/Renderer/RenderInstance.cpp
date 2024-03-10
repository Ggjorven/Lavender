#include "lvpch.h"
#include "RenderInstance.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/Renderer.hpp"

#include "Lavender/APIs/Vulkan/VulkanRenderer.hpp"

namespace Lavender
{

	RenderInstance* RenderInstance::Create(const RendererSpecification& specs)
	{
		switch (Renderer::GetAPI())
		{
		case RenderingAPI::Vulkan:
			return new VulkanRenderer(specs);

		default:
			LV_LOG_ERROR("Invalid API selected.");
			break;
		}

		return nullptr;
	}

}