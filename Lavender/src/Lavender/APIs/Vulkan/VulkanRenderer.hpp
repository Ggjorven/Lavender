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

		void Submit(RenderFunction function) override;
		void WaitFor(Ref<RenderCommandBuffer> commandBuffer) override;
		void Wait() override;

		void DrawIndexed(Ref<RenderCommandBuffer> commandBuffer, Ref<IndexBuffer> indexBuffer) override;

		void OnResize(uint32_t width, uint32_t height) override;

		RenderData GetRenderData() override;

		std::vector<Ref<VulkanRenderCommandBuffer>> GetCommandBuffers() { return m_WaitForCommandBuffers; }

	private:
		Utils::Queue<RenderFunction> m_RenderQueue = { };
		std::vector<Ref<VulkanRenderCommandBuffer>> m_WaitForCommandBuffers = { };
	};

}