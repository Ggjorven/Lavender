#pragma once

#include <vector>

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Renderer/UniformBuffer.hpp"
#include "Lavender/Renderer/Pipeline.hpp"

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Lavender
{

	class VulkanUniformBuffer : public UniformBuffer
	{
	public:
		VulkanUniformBuffer(size_t dataSize);
		VulkanUniformBuffer(Ref<Pipeline> pipeline, UniformElement element, size_t dataSize);
		virtual ~VulkanUniformBuffer();

		void SetData(void* data, size_t size) override;

		void Upload() override;
		void Upload(Ref<Pipeline> pipeline, UniformElement element) override;

	private:
		Ref<Pipeline> m_Pipeline = nullptr;
		UniformElement m_Element = {};
		size_t m_Size = 0;

		std::vector<VkBuffer> m_Buffers = { };
		std::vector<VmaAllocation> m_Allocations = { };
	};

}