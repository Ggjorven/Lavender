#pragma once

#include <memory>

#include "Lavender/Renderer/RenderConfig.hpp"
#include "Lavender/Utils/Utils.hpp"

namespace Lavender
{

	class RenderCommandBuffer;
	class IndexBuffer;

	class RenderInstance
	{
	public:
		RenderInstance() = default;
		virtual ~RenderInstance() = default;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		virtual void Submit(RenderFunction function) = 0;
		virtual void WaitFor(Ref<RenderCommandBuffer> commandBuffer) = 0;
		virtual void Wait() = 0;

		virtual void DrawIndexed(Ref<RenderCommandBuffer> commandBuffer, Ref<IndexBuffer> indexBuffer) = 0;

		virtual void OnResize(uint32_t width, uint32_t height) = 0;

		virtual RenderData GetRenderData() = 0;

		static RenderInstance* Create();
	};

}