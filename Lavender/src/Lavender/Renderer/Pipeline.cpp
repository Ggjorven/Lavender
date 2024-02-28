#include "lvpch.h"
#include "Pipeline.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/Renderer.hpp"

#include "Lavender/APIs/Vulkan/VulkanPipeline.hpp"

namespace Lavender
{

	Ref<Pipeline> Pipeline::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RenderingAPI::Vulkan:
			return RefHelper::Create<VulkanPipeline>();

		default:
			LV_LOG_ERROR("Invalid API selected.");
			break;
		}

		return nullptr;
	}

	Ref<Pipeline> Pipeline::Create(PipelineLayout layout)
	{
		switch (Renderer::GetAPI())
		{
		case RenderingAPI::Vulkan:
			return RefHelper::Create<VulkanPipeline>(layout);

		default:
			LV_LOG_ERROR("Invalid API selected.");
			break;
		}

		return nullptr;
	}

	Ref<Pipeline> Pipeline::Create(PipelineLayout layout, Ref<Shader> shader)
	{
		switch (Renderer::GetAPI())
		{
		case RenderingAPI::Vulkan:
			return RefHelper::Create<VulkanPipeline>(layout, shader);

		default:
			LV_LOG_ERROR("Invalid API selected.");
			break;
		}

		return nullptr;
	}

	Ref<Pipeline> Pipeline::Create(PipelineLayout layout, Ref<RenderPass> renderpass)
	{
		switch (Renderer::GetAPI())
		{
		case RenderingAPI::Vulkan:
			return RefHelper::Create<VulkanPipeline>(layout,renderpass);

		default:
			LV_LOG_ERROR("Invalid API selected.");
			break;
		}

		return nullptr;
	}

	Ref<Pipeline> Pipeline::Create(PipelineLayout layout, Ref<Shader> shader, Ref<RenderPass> renderpass)
	{
		switch (Renderer::GetAPI())
		{
		case RenderingAPI::Vulkan:
			return RefHelper::Create<VulkanPipeline>(layout, shader, renderpass);

		default:
			LV_LOG_ERROR("Invalid API selected.");
			break;
		}

		return nullptr;
	}

}