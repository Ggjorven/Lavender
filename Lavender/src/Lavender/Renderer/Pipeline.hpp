#pragma once

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Renderer/PipelineLayout.hpp"

namespace Lavender
{
	
	class Shader;
	class RenderPass;
	class DescriptorSetGroup;
	class RenderCommandBuffer;

	class Pipeline
	{
	public:
		Pipeline() = default;
		virtual ~Pipeline() = default;

		virtual void Initialize() = 0;
		virtual void Use(Ref<RenderCommandBuffer> commandBuffer) = 0;

		virtual void SetShader(Ref<Shader> shader) = 0;

		virtual PipelineSpecification& GetSpecification() = 0;
		virtual Ref<DescriptorSetGroup> GetDescriptorSets() = 0;

		static Ref<Pipeline> Create();
		static Ref<Pipeline> Create(PipelineSpecification specs, Ref<DescriptorSetGroup> sets);
		static Ref<Pipeline> Create(PipelineSpecification specs, Ref<DescriptorSetGroup> sets, Ref<Shader> shader);
		static Ref<Pipeline> Create(PipelineSpecification specs, Ref<DescriptorSetGroup> sets, Ref<RenderPass> renderpass);
		static Ref<Pipeline> Create(PipelineSpecification specs, Ref<DescriptorSetGroup> sets, Ref<Shader> shader, Ref<RenderPass> renderpass);  
	};

}