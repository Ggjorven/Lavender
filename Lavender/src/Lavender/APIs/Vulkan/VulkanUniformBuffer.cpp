#include "lvpch.h"
#include "VulkanUniformBuffer.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Utils/Profiler.hpp"

#include "Lavender/Renderer/Renderer.hpp"

#include "Lavender/APIs/Vulkan/VulkanAllocator.hpp"
#include "Lavender/APIs/Vulkan/VulkanPipeline.hpp"
#include "Lavender/APIs/Vulkan/VulkanContext.hpp"
#include "Lavender/APIs/Vulkan/VulkanDescriptorSet.hpp"

namespace Lavender
{

	VulkanUniformBuffer::VulkanUniformBuffer(size_t dataSize)
		: m_Size(dataSize)
	{
		uint32_t framesInFlight = Renderer::GetSpecification().FramesInFlight;
		m_Buffers.resize((size_t)framesInFlight);
		m_Allocations.resize((size_t)framesInFlight);

		for (size_t i = 0; i < framesInFlight; i++)
			m_Allocations[i] = VulkanAllocator::AllocateBuffer((VkDeviceSize)dataSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, m_Buffers[i]);
	}

	VulkanUniformBuffer::VulkanUniformBuffer(Ref<DescriptorSet> set, UniformElement element, size_t dataSize)
		: m_Set(set), m_Element(element), m_Size(dataSize)
	{
		uint32_t framesInFlight = Renderer::GetSpecification().FramesInFlight;
		m_Buffers.resize((size_t)framesInFlight);
		m_Allocations.resize((size_t)framesInFlight);

		for (size_t i = 0; i < framesInFlight; i++)
			m_Allocations[i] = VulkanAllocator::AllocateBuffer((VkDeviceSize)dataSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, m_Buffers[i]);

		Upload();
	}

	VulkanUniformBuffer::~VulkanUniformBuffer()
	{
		for (size_t i = 0; i < Renderer::GetSpecification().FramesInFlight; i++)
		{
			if (m_Buffers[i] != VK_NULL_HANDLE)
				VulkanAllocator::DestroyBuffer(m_Buffers[i], m_Allocations[i]);
		}
	}

	void VulkanUniformBuffer::SetData(void* data, size_t size)
	{
		LV_PROFILE_SCOPE("VulkanUniformBuffer::SetData");

		// TODO(Jorben): Make assert?
		if (size != m_Size)
			LV_LOG_ERROR("Invalid size passed to SetData()");

		for (size_t i = 0; i < Renderer::GetSpecification().FramesInFlight; i++)
		{
			void* mappedMemory = nullptr;
			VulkanAllocator::MapMemory(m_Allocations[i], mappedMemory);
			memcpy(mappedMemory, data, size);
			VulkanAllocator::UnMapMemory(m_Allocations[i]);
		}
	}

	void VulkanUniformBuffer::Upload()
	{
		Upload(m_Set, m_Element);
	}

	void VulkanUniformBuffer::Upload(Ref<DescriptorSet> set, UniformElement element)
	{
		LV_PROFILE_SCOPE("VulkanUniformBuffer::Upload");

		auto vkSet = RefHelper::RefAs<VulkanDescriptorSet>(set);

		for (size_t i = 0; i < Renderer::GetSpecification().FramesInFlight; i++)
		{
			VkDescriptorBufferInfo bufferInfo = {};
			bufferInfo.buffer = m_Buffers[i];
			bufferInfo.offset = 0;
			bufferInfo.range = m_Size;

			VkWriteDescriptorSet descriptorWrite = {};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = vkSet->GetVulkanSet((uint32_t)i);
			descriptorWrite.dstBinding = element.Binding;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = element.Count;
			descriptorWrite.pBufferInfo = &bufferInfo;

			vkUpdateDescriptorSets(RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetLogicalDevice()->GetVulkanDevice(), 1, &descriptorWrite, 0, nullptr);
		}
	}

}