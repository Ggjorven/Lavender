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
		: m_BufferSize(size)
	{
		m_BufferAllocation = VulkanAllocator::AllocateBuffer(m_BufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY, m_Buffer);

		VkBuffer stagingBuffer = VK_NULL_HANDLE;
		VmaAllocation stagingBufferAllocation = VK_NULL_HANDLE;
		stagingBufferAllocation = VulkanAllocator::AllocateBuffer(m_BufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY, stagingBuffer);

		void* mappedData = nullptr;
		VulkanAllocator::MapMemory(stagingBufferAllocation, mappedData);
		memcpy(mappedData, data, m_BufferSize);
		VulkanAllocator::UnMapMemory(stagingBufferAllocation);

		VulkanAllocator::CopyBuffer(stagingBuffer, m_Buffer, m_BufferSize);

		VulkanAllocator::DestroyBuffer(stagingBuffer, stagingBufferAllocation);
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		auto buffer = m_Buffer;
		auto allocation = m_BufferAllocation;

		Renderer::SubmitFree([buffer, allocation]()
		{
			if (buffer != VK_NULL_HANDLE)
				VulkanAllocator::DestroyBuffer(buffer, allocation);
		});
	}

	void VulkanVertexBuffer::Bind(Ref<RenderCommandBuffer> commandBuffer)
	{
		auto cmdBuf = RefHelper::RefAs<VulkanRenderCommandBuffer>(commandBuffer);
		
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(cmdBuf->GetVulkanCommandBuffer(), 0, 1, &m_Buffer, offsets);
	}

	Ref<VertexBuffer> VulkanVertexBuffer::Copy()
	{
		Ref<VulkanVertexBuffer> newBuffer = RefHelper::Create<VulkanVertexBuffer>();

		newBuffer->m_BufferAllocation = VulkanAllocator::AllocateBuffer(m_BufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY, newBuffer->m_Buffer);
		VulkanAllocator::CopyBuffer(m_Buffer, newBuffer->m_Buffer, m_BufferSize);

		return newBuffer;
	}

}