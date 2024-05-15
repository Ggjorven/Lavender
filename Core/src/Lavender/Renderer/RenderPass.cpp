#include "lvpch.h"
#include "RenderPass.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/Renderer.hpp"

#include "Lavender/Vulkan/VulkanRenderPass.hpp"

namespace Lavender
{

	Ref<RenderPass> RenderPass::Create(RenderPassSpecification specs, Ref<CommandBuffer> commandBuffer)
	{
		switch (RendererSpecification::API)
		{
		case RendererSpecification::RenderingAPI::Vulkan:
			return RefHelper::Create<VulkanRenderPass>(specs, commandBuffer);

		default:
			APP_ASSERT(false, "Invalid API selected.");
			break;
		}

		return nullptr;
	}

}