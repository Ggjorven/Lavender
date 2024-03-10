#pragma once

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Renderer/IndexBuffer.hpp"

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Lavender
{

	class VulkanIndexBuffer : public IndexBuffer
	{
	public:
		VulkanIndexBuffer(uint32_t* indices, uint32_t count);
		virtual ~VulkanIndexBuffer();

		void Bind(Ref<RenderCommandBuffer> commandBuffer) const;

		inline uint32_t GetCount() const { return m_Count; }

	private:
		VkBuffer m_Buffer = VK_NULL_HANDLE;
		VmaAllocation m_BufferAllocation = VK_NULL_HANDLE;

		uint32_t m_Count = 0;
	};

}