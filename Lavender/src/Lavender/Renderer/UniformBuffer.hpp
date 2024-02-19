#pragma once

#include <string>
#include <memory>
#include <unordered_set>
#include <array>

#include "Lavender/Renderer/RenderController.hpp"
#include "Lavender/Renderer/UniformInfo.hpp"
#include "Lavender/APIs/Vulkan/Setup/VulkanResources.hpp"
#include "Lavender/Utils/Utils.hpp"

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Lavender
{

	class UniformBuffer
	{
	public:
		UniformBuffer(std::shared_ptr<RenderController>& controller, UniformElement& element, size_t dataSize);
		virtual ~UniformBuffer();

		void SetData(void* data, size_t size);

		// Note(Jorben): This function only needs to be called once, when initializing.
		void UploadToController();

	private:
		std::shared_ptr<RenderController> m_RenderController = nullptr;
		UniformElement m_Element = {};
		size_t m_Size = 0;

		std::array<VkBuffer, LV_MAX_FRAMES_IN_FLIGHT> m_Buffers = { };
		std::array<VmaAllocation, LV_MAX_FRAMES_IN_FLIGHT> m_Allocations = { };
	};

}