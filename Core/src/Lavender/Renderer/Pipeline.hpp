#pragma once

#include "Lavender/Core/Core.hpp"
#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Renderer/Buffers.hpp"

namespace Lavender
{

	class Shader;
	class ComputeShader;
	class RenderPass;
	class DescriptorSets;
	class CommandBuffer;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Specification 
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	enum class CullingMode : uint8_t
	{
		None = 0, Front = 1, Back, FrontAndBack
	};

	enum class PolygonMode : uint32_t
	{
		None = 0x7FFFFFFF, Fill = 0, Line = 1
	};

	struct PipelineSpecification
	{
	public:
		BufferLayout Bufferlayout = {};

		PolygonMode Polygonmode = PolygonMode::Fill;
		CullingMode Cullingmode = CullingMode::Front;

		float LineWidth = 1.0f;
		bool Blending = false;
	};

	enum class PipelineBindPoint
	{
		Graphics = 0, Compute, RayTracingKHR, RayTracingNV
	};

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Pipeline 
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class Pipeline
	{
	public:
		Pipeline() = default;
		virtual ~Pipeline() = default;

		virtual void Use(Ref<CommandBuffer> commandBuffer, PipelineBindPoint bindPoint = PipelineBindPoint::Graphics) = 0;

		virtual PipelineSpecification& GetSpecification() = 0;
		virtual Ref<DescriptorSets> GetDescriptorSets() = 0;

		static Ref<Pipeline> Create(PipelineSpecification specs, Ref<DescriptorSets> sets, Ref<Shader> shader, Ref<RenderPass> renderpass);
		static Ref<Pipeline> Create(PipelineSpecification specs, Ref<DescriptorSets> sets, Ref<ComputeShader> shader);
	};

}