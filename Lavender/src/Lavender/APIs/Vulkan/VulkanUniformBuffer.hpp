#pragma once

#include <vector>
#include <utility>

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Renderer/Pipeline.hpp"
#include "Lavender/Renderer/UniformBuffer.hpp"
#include "Lavender/Renderer/DescriptorSet.hpp"

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Lavender
{

	class VulkanUniformBuffer : public UniformBuffer
	{
	public:
		VulkanUniformBuffer(size_t dataSize);
		VulkanUniformBuffer(Ref<DescriptorSet> set, UniformElement element, size_t dataSize);
		virtual ~VulkanUniformBuffer();

		void SetData(void* data, size_t size) override;

		void Upload() override;
		void Upload(Ref<DescriptorSet> set, UniformElement element) override;

	private:
		Ref<DescriptorSet> m_Set = nullptr;
		UniformElement m_Element = {};
		size_t m_Size = 0;

		std::vector<VkBuffer> m_Buffers = { };
		std::vector<VmaAllocation> m_Allocations = { };
	};

	class VulkanDynamicUniformBuffer : public DynamicUniformBuffer
	{
	public:
		VulkanDynamicUniformBuffer(uint32_t elements, size_t sizeOfOneElement);
		VulkanDynamicUniformBuffer(Ref<DescriptorSet> set, UniformElement element, uint32_t elements, size_t sizeOfOneElement);
		virtual ~VulkanDynamicUniformBuffer();

		void SetData(void* data, size_t size) override;

		void SetDataIndexed(uint32_t index, void* data, size_t size) override;
		void UploadIndexedData() override;

		inline size_t GetAlignment() const override { return m_AlignmentOfOneElement; }

		void Upload(Ref<DescriptorSet> set, UniformElement element, size_t offset) override;

	private:
		Ref<DescriptorSet> m_Set = nullptr;
		UniformElement m_Element = {};

		uint32_t m_ElementCount = 0;
		size_t m_SizeOfOneElement = 0;
		size_t m_AlignmentOfOneElement = 0;

		std::vector<std::pair<void*, size_t>> m_IndexedData = { };

		std::vector<VkBuffer> m_Buffers = { };
		std::vector<VmaAllocation> m_Allocations = { };
	};

}