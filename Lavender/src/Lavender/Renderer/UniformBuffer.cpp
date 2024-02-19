#include "lvpch.h"
#include "UniformBuffer.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/APIs/Vulkan/Setup/VulkanBufferHelper.hpp"

namespace Lavender
{

	UniformBuffer::UniformBuffer(std::shared_ptr<RenderController>& renderController, UniformElement& element, size_t dataSize)
		: m_RenderController(renderController), m_Element(element), m_Size(dataSize)
	{
		for (size_t i = 0; i < LV_MAX_FRAMES_IN_FLIGHT; i++)
		{
			VulkanBufferHelper::CreateBuffer((VkDeviceSize)dataSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, m_Buffers[i], m_Allocations[i]);
		}
	}

	UniformBuffer::~UniformBuffer()
	{
		for (size_t i = 0; i < LV_MAX_FRAMES_IN_FLIGHT; i++)
		{
			if (m_Buffers[i] != VK_NULL_HANDLE)
				vmaDestroyBuffer(VulkanBufferHelper::GetAllocator(), m_Buffers[i], m_Allocations[i]);
		}
	}

	void UniformBuffer::SetData(void* data, size_t size)
	{
		// TODO(Jorben): Make assert?
		if (size != m_Size)
			LV_LOG_ERROR("Invalid size passed to SetData()");

		for (size_t i = 0; i < LV_MAX_FRAMES_IN_FLIGHT; i++)
		{
			// TODO(Jorben): Make this mapping function a VulkanBufferHelper function?
			void* mappedMemory = nullptr;
			if (vmaMapMemory(VulkanBufferHelper::GetAllocator(), m_Allocations[i], &mappedMemory) != VK_SUCCESS)
				LV_LOG_ERROR("Failed to map memory");

			memcpy(mappedMemory, data, size);
			vmaUnmapMemory(VulkanBufferHelper::GetAllocator(), m_Allocations[i]);
		}
	}

	void UniformBuffer::UploadToController()
	{
		m_RenderController->AddUBO(m_Buffers, m_Size, m_Element.Binding, m_Element.Count);
	}

}