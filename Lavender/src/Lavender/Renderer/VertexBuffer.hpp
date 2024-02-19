#pragma once

#include <memory>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Lavender
{

	enum class BufferDataType
	{
		None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
	};

	uint32_t BufferDataTypeSize(BufferDataType type);

	struct BufferElement
	{
	public:
		std::string Name;
		uint32_t Location;
		BufferDataType Type;
		uint32_t Size;
		size_t Offset;
		bool Normalized;

		BufferElement() = default;
		BufferElement(BufferDataType type, uint32_t location, const std::string& name, bool normalized = false);

		uint32_t GetComponentCount() const;
	};

	class BufferLayout
	{
	public:
		BufferLayout() = default;
		BufferLayout(const std::initializer_list<BufferElement>& elements);

		inline uint32_t GetStride() const { return m_Stride; }
		inline const std::vector<BufferElement>& GetElements() const { return m_Elements; }

		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

	private:
		void CalculateOffsetsAndStride();

	private:
		std::vector<BufferElement> m_Elements = { };
		uint32_t m_Stride = 0;
	};

	class VertexBuffer
	{
	public:
		VertexBuffer(void* data, size_t size);
		virtual ~VertexBuffer();

		void Bind() const;

	private:
		VkBuffer m_Buffer = VK_NULL_HANDLE;
		VmaAllocation m_BufferAllocation = VK_NULL_HANDLE;

	};

}