#include "lvpch.h"
#include "CommandBuffer.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/Renderer.hpp"

#include "Lavender/Vulkan/VulkanCommandBuffer.hpp"

namespace Lavender
{

	Ref<CommandBuffer> CommandBuffer::Create(CommandBufferSpecification specs)
	{
		switch (RendererSpecification::API)
		{
		case RendererSpecification::RenderingAPI::Vulkan:
			return RefHelper::Create<VulkanCommandBuffer>(specs);

		default:
			APP_ASSERT(false, "Invalid API selected.");
			break;
		}

		return nullptr;
	}

}