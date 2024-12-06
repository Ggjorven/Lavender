#include "lvpch.h"
#include "Pipeline.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/Shader.hpp"
#include "Lavender/Renderer/Renderer.hpp"
#include "Lavender/Renderer/RenderPass.hpp"
#include "Lavender/Renderer/Descriptors.hpp"

#include "Lavender/Vulkan/VulkanPipeline.hpp"

namespace Lavender
{

	Ref<Pipeline> Pipeline::Create(PipelineSpecification specs, Ref<DescriptorSets> sets, Ref<Shader> shader, Ref<RenderPass> renderpass)
	{
		switch (RendererSpecification::API)
		{
		case RenderingAPI::Vulkan:
			return RefHelper::Create<VulkanPipeline>(specs, sets, shader, renderpass);

		default:
			APP_ASSERT(false, "Invalid API selected.");
			break;
		}

		return nullptr;
	}

	Ref<Pipeline> Pipeline::Create(PipelineSpecification specs, Ref<DescriptorSets> sets, Ref<ComputeShader> shader)
	{
		switch (RendererSpecification::API)
		{
		case RenderingAPI::Vulkan:
			return RefHelper::Create<VulkanPipeline>(specs, sets, shader);

		default:
			APP_ASSERT(false, "Invalid API selected.");
			break;
		}

		return nullptr;
	}

}