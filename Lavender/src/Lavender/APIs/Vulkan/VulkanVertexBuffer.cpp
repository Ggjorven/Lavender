#include "lvpch.h"
#include "VulkanIndexBuffer.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/Renderer.hpp"
#include "Lavender/APIs/Vulkan/VulkanContext.hpp"

#include "Lavender/APIs/Vulkan/VulkanAllocator.hpp"
#include "Lavender/APIs/Vulkan/VulkanRenderCommandBuffer.hpp"
#include "VulkanVertexBuffer.hpp"

namespace Lavender
{

	VulkanVertexBuffer::VulkanVertexBuffer(void* data, size_t size)
	{
		m_BufferAllocation = VulkanAllocator::AllocateBuffer(size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY, m_Buffer);

		VkBuffer stagingBuffer = VK_NULL_HANDLE;
		VmaAllocation stagingBufferAllocation = VK_NULL_HANDLE;
		stagingBufferAllocation = VulkanAllocator::AllocateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY, stagingBuffer);

		void* mappedData = nullptr;
		VulkanAllocator::MapMemory(stagingBufferAllocation, mappedData);
		memcpy(mappedData, data, size);
		VulkanAllocator::UnMapMemory(stagingBufferAllocation);

		VulkanAllocator::CopyBuffer(stagingBuffer, m_Buffer, size);

		VulkanAllocator::DestroyBuffer(stagingBuffer, stagingBufferAllocation);
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		if (m_Buffer != VK_NULL_HANDLE)
			VulkanAllocator::DestroyBuffer(m_Buffer, m_BufferAllocation);
	}

	void VulkanVertexBuffer::Bind(Ref<RenderCommandBuffer> commandBuffer)
	{
		auto cmdBuf = RefHelper::RefAs<VulkanRenderCommandBuffer>(commandBuffer);
		
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(cmdBuf->GetVulkanCommandBuffer(), 0, 1, &m_Buffer, offsets);
	}

}