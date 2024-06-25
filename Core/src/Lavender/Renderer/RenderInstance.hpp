#pragma once

#include <memory>

#include "Lavender/Core/Core.hpp"
#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Renderer/RendererConfig.hpp"

namespace Lavender
{

	class CommandBuffer;
	class IndexBuffer;
	class Image2D;

	class RenderInstance
	{
	public:
		RenderInstance() = default;
		virtual ~RenderInstance() = default;

		virtual void Init() = 0;
		
		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		virtual void Submit(RenderFunction function) = 0;
		virtual void SubmitFree(FreeFunction function) = 0;
		virtual void SubmitFreeToBack(FreeFunction function) = 0;

		virtual void Wait() = 0;

		virtual void Draw(Ref<CommandBuffer> commandBuffer, uint32_t verticeCount) = 0;
		virtual void DrawIndexed(Ref<CommandBuffer> commandBuffer, Ref<IndexBuffer> indexBuffer) = 0;

		virtual void OnResize(uint32_t width, uint32_t height) = 0;

		virtual void RemoveFromQueues(Ref<CommandBuffer> commandBuffer) = 0;

		virtual Utils::Queue<RenderFunction>& GetRenderQueue() = 0;
		virtual Utils::Queue<FreeFunction>& GetFreeQueue() = 0;

		virtual uint32_t GetCurrentFrame() const = 0;
		virtual std::vector<Ref<Image2D>>& GetSwapChainImages() = 0;
		virtual Ref<Image2D> GetDepthImage() = 0;
		
		static RenderInstance* Create();
	};

}