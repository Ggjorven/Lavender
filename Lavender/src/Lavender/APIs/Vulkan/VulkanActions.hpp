#pragma once

#include <vulkan/vulkan.h>

namespace Lavender
{

	class VulkanActions
	{
	public:
		static VkCommandBuffer BeginSingleTimeCommands();
		static void EndSingleTimeCommands(VkCommandBuffer& buffer);
	};

}