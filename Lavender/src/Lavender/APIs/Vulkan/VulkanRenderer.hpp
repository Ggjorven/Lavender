#pragma once

#include <vector>

#include "Lavender/Renderer/RenderInstance.hpp"
#include "Lavender/Renderer/RenderCommandBuffer.hpp"

#include "Lavender/APIs/Vulkan/VulkanRenderCommandBuffer.hpp"

#include <vulkan/vulkan.h>

namespace Lavender
{

	class VulkanRenderer : public RenderInstance
	{
	public:
		VulkanRenderer();
		virtual ~VulkanRenderer();

		void BeginFrame() override;
		void EndFrame() override;

		void Submit(RenderFunction function);
		void WaitFor(Ref<RenderCommandBuffer> commandBuffer);

		void DrawIndexed(Ref<RenderCommandBuffer> commandBuffer, Ref<IndexBuffer> indexBuffer);

		void OnResize(uint32_t width, uint32_t height) override;

		std::vector<Ref<VulkanRenderCommandBuffer>> GetCommandBuffers() { return m_WaitForCommandBuffers; }

	private:
		Utils::Queue<RenderFunction> m_RenderQueue = { };
		std::vector<Ref<VulkanRenderCommandBuffer>> m_WaitForCommandBuffers = { };
	};

}