#pragma once

#include "Lavender/Utils/Utils.hpp"

namespace Lavender
{

	class RenderCommandBuffer;

	class RenderPass
	{
	public:
		RenderPass() = default;
		virtual ~RenderPass() = default;

		virtual void Begin() = 0;
		virtual void End() = 0;
		virtual void Submit() = 0;

		virtual Ref<RenderCommandBuffer> GetCommandBuffer() = 0;

		static Ref<RenderPass> Create();
		static Ref<RenderPass> CreateFromCommandBuffer(Ref<RenderCommandBuffer> commandBuffer);
	};

}