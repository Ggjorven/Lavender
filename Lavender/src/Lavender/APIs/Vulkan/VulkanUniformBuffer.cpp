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

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// UniformBuffer
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
		auto buffers = m_Buffers;
		auto allocations = m_Allocations;

		Renderer::SubmitFree([buffers, allocations]()
		{
			for (size_t i = 0; i < Renderer::GetSpecification().FramesInFlight; i++)
			{
				if (buffers[i] != VK_NULL_HANDLE)
					VulkanAllocator::DestroyBuffer(buffers[i], allocations[i]);
			}
		});
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

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Dynamic UniformBuffer
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	VulkanDynamicUniformBuffer::VulkanDynamicUniformBuffer(uint32_t elements, size_t sizeOfOneElement)
		: m_ElementCount(elements), m_SizeOfOneElement(sizeOfOneElement)
	{
		size_t uboAlignment = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetPhysicalDevice()->GetProperties().limits.minUniformBufferOffsetAlignment;
		m_AlignmentOfOneElement = (sizeOfOneElement / uboAlignment) * uboAlignment + ((sizeOfOneElement % uboAlignment) > 0 ? uboAlignment : 0);
		
		uint32_t framesInFlight = Renderer::GetSpecification().FramesInFlight;
		m_Buffers.resize((size_t)framesInFlight);
		m_Allocations.resize((size_t)framesInFlight);

		for (size_t i = 0; i < framesInFlight; i++)
			m_Allocations[i] = VulkanAllocator::AllocateBuffer((VkDeviceSize)(m_ElementCount * m_AlignmentOfOneElement), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, m_Buffers[i]);

		m_IndexedData.resize((size_t)elements);
	}

	VulkanDynamicUniformBuffer::VulkanDynamicUniformBuffer(Ref<DescriptorSet> set, UniformElement element, uint32_t elements, size_t sizeOfOneElement)
		: m_Set(set), m_Element(element), m_ElementCount(elements), m_SizeOfOneElement(sizeOfOneElement)
	{
		size_t uboAlignment = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetPhysicalDevice()->GetProperties().limits.minUniformBufferOffsetAlignment;
		m_AlignmentOfOneElement = (sizeOfOneElement / uboAlignment) * uboAlignment + ((sizeOfOneElement % uboAlignment) > 0 ? uboAlignment : 0);

		uint32_t framesInFlight = Renderer::GetSpecification().FramesInFlight;
		m_Buffers.resize((size_t)framesInFlight);
		m_Allocations.resize((size_t)framesInFlight);

		for (size_t i = 0; i < framesInFlight; i++)
			m_Allocations[i] = VulkanAllocator::AllocateBuffer((VkDeviceSize)(m_ElementCount * m_AlignmentOfOneElement), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, m_Buffers[i]);

		m_IndexedData.resize((size_t)elements);
		Upload();
	}

	VulkanDynamicUniformBuffer::~VulkanDynamicUniformBuffer()
	{
		auto buffers = m_Buffers;
		auto allocations = m_Allocations;

		Renderer::SubmitFree([buffers, allocations]()
		{
			for (size_t i = 0; i < Renderer::GetSpecification().FramesInFlight; i++)
			{
				if (buffers[i] != VK_NULL_HANDLE)
					VulkanAllocator::DestroyBuffer(buffers[i], allocations[i]);
			}
		});
	}

	void VulkanDynamicUniformBuffer::SetData(void* data, size_t size)
	{
		LV_PROFILE_SCOPE("VulkanDynamicUniformBuffer::SetData");

		// TODO(Jorben): Make assert?
		if (size != m_ElementCount * m_SizeOfOneElement)
			LV_LOG_ERROR("Invalid size passed to SetData()");

		for (size_t i = 0; i < Renderer::GetSpecification().FramesInFlight; i++)
		{
			void* mappedMemory = nullptr;
			VulkanAllocator::MapMemory(m_Allocations[i], mappedMemory);
			memcpy(mappedMemory, data, size);
			VulkanAllocator::UnMapMemory(m_Allocations[i]);
		}
	}

	void VulkanDynamicUniformBuffer::SetDataIndexed(uint32_t index, void* data, size_t size)
	{
		m_IndexedData[index] = std::make_pair(data, size);
	}

	void VulkanDynamicUniformBuffer::UploadIndexedData()
	{
		LV_PROFILE_SCOPE("VulkanDynamicUniformBuffer::UploadIndexedData");

		for (size_t i = 0; i < Renderer::GetSpecification().FramesInFlight; i++)
		{
			void* mappedMemory = nullptr;
			VulkanAllocator::MapMemory(m_Allocations[i], mappedMemory);

			for (size_t j = 0; j < m_IndexedData.size(); j++)
			{
				void* srcData = m_IndexedData[j].first;
				size_t srcSize = m_IndexedData[j].second;
				size_t copySize = std::min(srcSize, m_AlignmentOfOneElement); // Ensure not to copy more than the aligned size
				memcpy(static_cast<char*>(mappedMemory) + j * m_AlignmentOfOneElement, srcData, copySize);
			}

			VulkanAllocator::UnMapMemory(m_Allocations[i]);
		}

		m_IndexedData.clear();
		m_IndexedData.resize(m_ElementCount);
	}

	void VulkanDynamicUniformBuffer::Upload()
	{
		Upload(m_Set, m_Element);
	}

	void VulkanDynamicUniformBuffer::Upload(Ref<DescriptorSet> set, UniformElement element)
	{
		LV_PROFILE_SCOPE("VulkanUniformBuffer::Upload");

		auto vkSet = RefHelper::RefAs<VulkanDescriptorSet>(set);

		for (size_t i = 0; i < Renderer::GetSpecification().FramesInFlight; i++)
		{
			VkDescriptorBufferInfo bufferInfo = {};
			bufferInfo.buffer = m_Buffers[i];
			bufferInfo.offset = 0;
			bufferInfo.range = VK_WHOLE_SIZE;

			VkWriteDescriptorSet descriptorWrite = {};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = vkSet->GetVulkanSet((uint32_t)i);
			descriptorWrite.dstBinding = element.Binding;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
			descriptorWrite.descriptorCount = element.Count;
			descriptorWrite.pBufferInfo = &bufferInfo;

			vkUpdateDescriptorSets(RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetLogicalDevice()->GetVulkanDevice(), 1, &descriptorWrite, 0, nullptr);
		}
	}

	VulkanDynamicUniformBuffer::AlignedData::AlignedData(void* data, size_t size, size_t alignment)
		: Data(data), m_PaddingSize(alignment - size)
	{
		size_t remainder = size % alignment;
		if (remainder != 0) 
		{
			m_PaddingSize = alignment - remainder;
			Padding = new char[m_PaddingSize];
		}
	}

	VulkanDynamicUniformBuffer::AlignedData::~AlignedData()
	{
		if (Padding)
			delete[] Padding;
	}

}