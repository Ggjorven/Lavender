#pragma once

#include <vector>

#include "Lavender/Renderer/RenderCommandBuffer.hpp"
#include "Lavender/Utils/Utils.hpp"

#include <vulkan/vulkan.h>

namespace Lavender
{

	class VulkanRenderCommandBuffer : public RenderCommandBuffer
	{
	public:
		VulkanRenderCommandBuffer();
		virtual ~VulkanRenderCommandBuffer();

		void Begin() override;
		void End() override;
		void Submit() override;

		inline VkSemaphore GetRenderFinishedSemaphore(uint32_t index) const { return m_RenderFinishedSemaphores[index]; }
		inline VkFence GetInFlightFence(uint32_t index) const { return m_InFlightFences[index]; }

		VkCommandBuffer GetVulkanCommandBuffer();

		static void ResetSemaphore();
		static VkSemaphore GetSemaphore();

	private:
		std::vector<VkCommandBuffer> m_CommandBuffers = { };

		// Sync objects
		std::vector<VkSemaphore> m_RenderFinishedSemaphores = { };
		std::vector<VkFence> m_InFlightFences = { };
	};

}