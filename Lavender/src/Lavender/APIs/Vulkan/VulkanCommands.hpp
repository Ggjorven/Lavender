#pragma once

#include <vulkan/vulkan.h>

namespace Lavender
{

	class VulkanCommands
	{
	public:
		static VkCommandBuffer Begin();
		static void End(VkCommandBuffer commandBuffer);
		static void Submit(VkCommandBuffer commandBuffer);

		static void EndAndSubmit(VkCommandBuffer commandBuffer);
	};

}