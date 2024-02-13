#pragma once

#include <memory>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Lavender
{

	class IndexBuffer
	{
	public:
		IndexBuffer(uint32_t* data, uint32_t count);
		virtual ~IndexBuffer();

		void Bind() const;

		uint32_t GetCount() const { return m_Count; }

	private:
		VkBuffer m_Buffer = VK_NULL_HANDLE;
		VmaAllocation m_BufferAllocation = VK_NULL_HANDLE;

		uint32_t m_Count = 0;
	};

}