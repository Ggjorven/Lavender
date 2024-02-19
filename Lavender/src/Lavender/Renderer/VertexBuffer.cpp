#include "lvpch.h"
#include "VertexBuffer.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/Renderer.hpp"
#include "Lavender/APIs/Vulkan/VulkanManager.hpp"
#include "Lavender/APIs/Vulkan/Setup/VulkanBufferHelper.hpp"

namespace Lavender
{

	uint32_t BufferDataTypeSize(BufferDataType type)
	{
		switch (type)
		{
		case BufferDataType::Float:    return 4;
		case BufferDataType::Float2:   return 4 * 2;
		case BufferDataType::Float3:   return 4 * 3;
		case BufferDataType::Float4:   return 4 * 4;
		case BufferDataType::Mat3:     return 4 * 3 * 3;
		case BufferDataType::Mat4:     return 4 * 4 * 4;
		case BufferDataType::Int:      return 4;
		case BufferDataType::Int2:     return 4 * 2;
		case BufferDataType::Int3:     return 4 * 3;
		case BufferDataType::Int4:     return 4 * 4;
		case BufferDataType::Bool:     return 1;
		}

		LV_LOG_ERROR("Unknown ShaderDataType!");
		return 0;
	}

	BufferElement::BufferElement(BufferDataType type, uint32_t location, const std::string& name, bool normalized)
		: Name(name), Location(location), Type(type), Size(BufferDataTypeSize(type)), Offset(0), Normalized(normalized)
	{
	}

	uint32_t BufferElement::GetComponentCount() const
	{
		switch (Type)
		{
		case BufferDataType::Float:   return 1;
		case BufferDataType::Float2:  return 2;
		case BufferDataType::Float3:  return 3;
		case BufferDataType::Float4:  return 4;
		case BufferDataType::Mat3:    return 3 * 3;
		case BufferDataType::Mat4:    return 4 * 4;
		case BufferDataType::Int:     return 1;
		case BufferDataType::Int2:    return 2;
		case BufferDataType::Int3:    return 3;
		case BufferDataType::Int4:    return 4;
		case BufferDataType::Bool:    return 1;
		}

		LV_LOG_ERROR("Unknown ShaderDataType!");
		return 0;
	}

	BufferLayout::BufferLayout(const std::initializer_list<BufferElement>& elements)
		: m_Elements(elements)
	{
		CalculateOffsetsAndStride();
	}

	void BufferLayout::CalculateOffsetsAndStride()
	{
		size_t offset = 0;
		m_Stride = 0;
		for (auto& element : m_Elements)
		{
			element.Offset = offset;
			offset += element.Size;
			m_Stride += element.Size;
		}
	}

	VertexBuffer::VertexBuffer(void* data, size_t size)
	{
		VulkanBufferHelper::CreateBuffer(size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY, m_Buffer, m_BufferAllocation);

		VkBuffer stagingBuffer = VK_NULL_HANDLE;
		VmaAllocation stagingBufferAllocation = VK_NULL_HANDLE;
		VulkanBufferHelper::CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY, stagingBuffer, stagingBufferAllocation);

		void* mappedData;
		vmaMapMemory(VulkanBufferHelper::GetAllocator(), stagingBufferAllocation, &mappedData);
		memcpy(mappedData, data, size);
		vmaUnmapMemory(VulkanBufferHelper::GetAllocator(), stagingBufferAllocation);

		VulkanBufferHelper::CopyBuffer(stagingBuffer, m_Buffer, size);

		vmaDestroyBuffer(VulkanBufferHelper::GetAllocator(), stagingBuffer, stagingBufferAllocation);
	}

	VertexBuffer::~VertexBuffer()
	{
		if (m_Buffer != VK_NULL_HANDLE)
			vmaDestroyBuffer(VulkanBufferHelper::GetAllocator(), m_Buffer, m_BufferAllocation);
	}

	void VertexBuffer::Bind() const
	{
		auto& resourceInfo = VulkanManager::GetResourceInfo();

		VkBuffer buffers[] = { m_Buffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(resourceInfo.CommandBuffers[Renderer::GetCurrentFrame()], 0, 1, buffers, offsets);
	}

}
