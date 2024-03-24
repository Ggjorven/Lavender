#include "lvpch.h"
#include "Pipeline.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/Renderer.hpp"
#include "Lavender/Renderer/DescriptorSet.hpp"

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

	Ref<Pipeline> Pipeline::Create(PipelineSpecification specs, Ref<DescriptorSetGroup> sets)
	{
		switch (Renderer::GetAPI())
		{
		case RenderingAPI::Vulkan:
			return RefHelper::Create<VulkanPipeline>(specs, sets);

		default:
			LV_LOG_ERROR("Invalid API selected.");
			break;
		}

		return nullptr;
	}

	Ref<Pipeline> Pipeline::Create(PipelineSpecification specs, Ref<DescriptorSetGroup> sets, Ref<Shader> shader)
	{
		switch (Renderer::GetAPI())
		{
		case RenderingAPI::Vulkan:
			return RefHelper::Create<VulkanPipeline>(specs, sets, shader);

		default:
			LV_LOG_ERROR("Invalid API selected.");
			break;
		}

		return nullptr;
	}

	Ref<Pipeline> Pipeline::Create(PipelineSpecification specs, Ref<DescriptorSetGroup> sets, Ref<RenderPass> renderpass)
	{
		switch (Renderer::GetAPI())
		{
		case RenderingAPI::Vulkan:
			return RefHelper::Create<VulkanPipeline>(specs, sets, renderpass);

		default:
			LV_LOG_ERROR("Invalid API selected.");
			break;
		}

		return nullptr;
	}

	Ref<Pipeline> Pipeline::Create(PipelineSpecification specs, Ref<DescriptorSetGroup> sets, Ref<Shader> shader, Ref<RenderPass> renderpass)
	{
		switch (Renderer::GetAPI())
		{
		case RenderingAPI::Vulkan:
			return RefHelper::Create<VulkanPipeline>(specs, sets, shader, renderpass);

		default:
			LV_LOG_ERROR("Invalid API selected.");
			break;
		}

		return nullptr;
	}

}