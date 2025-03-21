#pragma once

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Renderer/PipelineLayout.hpp"

namespace Lavender
{
	
	class Shader;
	class RenderPass;
	class RenderCommandBuffer;

	class Pipeline
	{
	public:
		Pipeline() = default;
		virtual ~Pipeline() = default;

		virtual void Initialize() = 0;
		virtual void Use(Ref<RenderCommandBuffer> commandBuffer) = 0;

		virtual void SetShader(Ref<Shader> shader) = 0;

		// TODO: Make some arguments = nullptr to remove the clutter of constructors
		static Ref<Pipeline> Create();
		static Ref<Pipeline> Create(PipelineSpecification specs);
		static Ref<Pipeline> Create(PipelineSpecification specs, Ref<Shader> shader);
		static Ref<Pipeline> Create(PipelineSpecification specs, Ref<RenderPass> renderpass);
		static Ref<Pipeline> Create(PipelineSpecification specs, Ref<Shader> shader, Ref<RenderPass> renderpass);
	};

}