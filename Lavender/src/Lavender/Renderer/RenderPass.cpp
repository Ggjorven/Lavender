#include "lvpch.h"
#include "RenderPass.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/Renderer.hpp"

#include "Lavender/APIs/Vulkan/VulkanRenderPass.hpp"

namespace Lavender
{

	Ref<RenderPass> RenderPass::Create(RenderPassSpecification specs)
	{
		switch (Renderer::GetAPI())
		{
		case RenderingAPI::Vulkan:
			return RefHelper::Create<VulkanRenderPass>(specs);

		default:
			LV_LOG_ERROR("Invalid API selected.");
			break;
		}

		return nullptr;
	}

	Ref<RenderPass> RenderPass::CreateFromCommandBuffer(RenderPassSpecification specs, Ref<RenderCommandBuffer> commandBuffer)
	{
		switch (Renderer::GetAPI())
		{
		case RenderingAPI::Vulkan:
			return RefHelper::Create<VulkanRenderPass>(specs, commandBuffer);

		default:
			LV_LOG_ERROR("Invalid API selected.");
			break;
		}

		return nullptr;
	}

}