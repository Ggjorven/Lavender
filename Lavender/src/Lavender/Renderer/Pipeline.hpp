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

		// Note (Jorben): Make sure you have this UniformBuffer specified in the layout
		//virtual void AddUniformBuffer(/*TODO*/) = 0;
		// Note (Jorben): Make sure you have this Image specified in the layout
		//virtual void AddImage(/*TODO*/) = 0;

		// TODO: Make some arguments = nullptr to remove the clutter of constructors
		static Ref<Pipeline> Create();
		static Ref<Pipeline> Create(PipelineLayout layout);
		static Ref<Pipeline> Create(PipelineLayout layout, Ref<Shader> shader);
		static Ref<Pipeline> Create(PipelineLayout layout, Ref<RenderPass> renderpass);
		static Ref<Pipeline> Create(PipelineLayout layout, Ref<Shader> shader, Ref<RenderPass> renderpass);
	};

}