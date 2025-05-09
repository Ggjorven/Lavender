#include "lvpch.h"
#include "RenderCommandBuffer.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/Renderer.hpp"

#include "Lavender/APIs/Vulkan/VulkanRenderCommandBuffer.hpp"

namespace Lavender
{

	Ref<RenderCommandBuffer> RenderCommandBuffer::Create(CommandBufferSpecification specs)
	{
		switch (Renderer::GetAPI())
		{
		case RenderingAPI::Vulkan:
			return RefHelper::Create<VulkanRenderCommandBuffer>(specs);

		default:
			LV_LOG_ERROR("Invalid API selected.");
			break;
		}

		return nullptr;
	}

}