#pragma once

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Renderer/PipelineLayout.hpp"

namespace Lavender
{

	class Pipeline
	{
	public:
		Pipeline() = default;
		virtual ~Pipeline() = default;

		virtual void Use() = 0;

		// Note (Jorben): Make sure you have this UniformBuffer specified in the layout
		//virtual void AddUniformBuffer(/*TODO*/) = 0;
		// Note (Jorben): Make sure you have this Image specified in the layout
		//virtual void AddImage(/*TODO*/) = 0;

		static Ref<Pipeline> Create();
		static Ref<Pipeline> Create(PipelineLayout layout);
		// TODO: Add one with shader as well
	};

}