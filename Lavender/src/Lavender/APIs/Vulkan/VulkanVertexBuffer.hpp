#pragma once

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Renderer/VertexBuffer.hpp"

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Lavender
{

	class VulkanVertexBuffer : public VertexBuffer
	{
	public:
		VulkanVertexBuffer() = default;
		VulkanVertexBuffer(void* data, size_t size);
		virtual ~VulkanVertexBuffer();
		
		void Bind(Ref<RenderCommandBuffer> commandBuffer) override;

		Ref<VertexBuffer> Copy() override;

	private:
		VkBuffer m_Buffer = VK_NULL_HANDLE;
		VmaAllocation m_BufferAllocation = VK_NULL_HANDLE;

		size_t m_BufferSize = 0;
	};

}