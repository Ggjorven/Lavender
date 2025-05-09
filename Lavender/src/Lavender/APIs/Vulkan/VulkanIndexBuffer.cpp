#include "lvpch.h"
#include "VulkanIndexBuffer.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/Renderer.hpp"
#include "Lavender/APIs/Vulkan/VulkanContext.hpp"

#include "Lavender/APIs/Vulkan/VulkanAllocator.hpp"
#include "Lavender/APIs/Vulkan/VulkanRenderCommandBuffer.hpp"

namespace Lavender
{

	VulkanIndexBuffer::VulkanIndexBuffer(uint32_t* indices, uint32_t count)
		: m_Count(count)
	{
		VkDeviceSize bufferSize = sizeof(uint32_t) * count;

		m_BufferAllocation = VulkanAllocator::AllocateBuffer(bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY, m_Buffer);

		VkBuffer stagingBuffer = VK_NULL_HANDLE;
		VmaAllocation stagingBufferAllocation = VK_NULL_HANDLE;
		stagingBufferAllocation = VulkanAllocator::AllocateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY, stagingBuffer);

		// Map the staging buffer and copy the data to it
		void* mappedData = nullptr;
		VulkanAllocator::MapMemory(stagingBufferAllocation, mappedData);
		memcpy(mappedData, indices, bufferSize);
		VulkanAllocator::UnMapMemory(stagingBufferAllocation);

		// Copy data from the staging buffer to the index buffer
		VulkanAllocator::CopyBuffer(stagingBuffer, m_Buffer, bufferSize);

		// Clean up the staging buffer
		VulkanAllocator::DestroyBuffer(stagingBuffer, stagingBufferAllocation);
	}

	VulkanIndexBuffer::~VulkanIndexBuffer()
	{
		auto buffer = m_Buffer;
		auto allocation = m_BufferAllocation;

		Renderer::SubmitFree([buffer, allocation]() 
		{
			if (buffer != VK_NULL_HANDLE)
				VulkanAllocator::DestroyBuffer(buffer, allocation);
		});

	}

	void VulkanIndexBuffer::Bind(Ref<RenderCommandBuffer> commandBuffer) const
	{
		auto cmdBuf = RefHelper::RefAs<VulkanRenderCommandBuffer>(commandBuffer);
		vkCmdBindIndexBuffer(cmdBuf->GetVulkanCommandBuffer(), m_Buffer, 0, VK_INDEX_TYPE_UINT32);
	}

	Ref<IndexBuffer> VulkanIndexBuffer::Copy()
	{
		Ref<VulkanIndexBuffer> newBuffer = RefHelper::Create<VulkanIndexBuffer>();

		newBuffer->m_BufferAllocation = VulkanAllocator::AllocateBuffer(m_Count * sizeof(uint32_t), VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY, newBuffer->m_Buffer);
		VulkanAllocator::CopyBuffer(m_Buffer, newBuffer->m_Buffer, m_Count * sizeof(uint32_t));
		newBuffer->m_Count = m_Count;

		return newBuffer;
	}

}