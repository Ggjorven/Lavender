#pragma once

#include <vector>

#include "Lavender/Core/Core.hpp"
#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Renderer/CommandBuffer.hpp"

#include <vulkan/vulkan.h>

namespace Lavender
{

	class VulkanCommandBuffer : public CommandBuffer
	{
	public:
		VulkanCommandBuffer(CommandBufferSpecification specs);
		virtual ~VulkanCommandBuffer();

		void Begin() override;
		void End() override;
		void Submit(Queue queue, const std::vector<Ref<CommandBuffer>>& waitOn) override;

		void WaitOnFinish() override;

		inline VkSemaphore GetRenderFinishedSemaphore(uint32_t index) { return m_RenderFinishedSemaphores[index]; }
		inline VkFence GetInFlightFence(uint32_t index) { return m_InFlightFences[index]; }
		inline VkCommandBuffer GetVulkanCommandBuffer(uint32_t index) { return m_CommandBuffers[index]; }

	private:
		CommandBufferSpecification m_Specification = {};

		std::vector<VkCommandBuffer> m_CommandBuffers = { };

		// Sync objects
		std::vector<VkSemaphore> m_RenderFinishedSemaphores = { };
		std::vector<VkFence> m_InFlightFences = { };
	};

}