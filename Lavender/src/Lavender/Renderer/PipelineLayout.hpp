#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <initializer_list>
#include <unordered_set>

#include "Lavender/Utils/Utils.hpp"

namespace Lavender
{

	enum class DataType
	{
		None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
	};
	uint32_t DataTypeSize(DataType type);

	struct BufferElement
	{
	public:
		std::string Name;
		uint32_t Location;
		DataType Type;
		uint32_t Size;
		size_t Offset;

		BufferElement() = default;
		BufferElement(DataType type, uint32_t location, const std::string& name);
		virtual ~BufferElement() = default;

		uint32_t GetComponentCount() const;
	};

	struct BufferLayout
	{
	public:
		BufferLayout() = default;
		BufferLayout(const std::initializer_list<BufferElement>& elements);
		virtual ~BufferLayout() = default;

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



	typedef uint32_t SetID;

	enum class UniformDataType
	{
		None = 0, UniformBuffer, Image
	};

	// Note(Jorben): To use multiple descriptor set you need have Vulkan selected as the API
	struct UniformElement
	{
	public:
		enum class ShaderStage
		{
			None = 0, Vertex = BIT(0), Fragment = BIT(1)
		};
		// Bitwise operations are created below

	public:
		std::string Name = "Empty";
		SetID Set = 0;
		uint32_t Binding = 0;
		UniformDataType Type = UniformDataType::None;
		uint32_t Count = 1;
		ShaderStage Stage = ShaderStage::None;

		UniformElement() = default;
		UniformElement(UniformDataType type, SetID set, uint32_t binding, const std::string& name, ShaderStage stage, uint32_t count = 1);
		virtual ~UniformElement() = default;
	};

	class UniformLayout
	{
	public:
		UniformLayout() = default;
		UniformLayout(const std::initializer_list<UniformElement>& elements);
		virtual ~UniformLayout() = default;

		inline const std::unordered_map<uint32_t, std::vector<UniformElement>>& GetElements() const { return m_Elements; }

		uint32_t UniqueCount(SetID set) const;
		std::unordered_set<UniformDataType> UniqueTypes(SetID set) const;
		uint32_t AmountOf(SetID set, UniformDataType type) const;

	private:
		// First element is the Set, second are the elements in that Set
		std::unordered_map<SetID, std::vector<UniformElement>> m_Elements = { };
	};
	DEFINE_BITWISE_OPS(UniformElement::ShaderStage)

	// TODO: Add more customizability
	struct PipelineLayout
	{
	public:
		PipelineLayout() = default;
		PipelineLayout(BufferLayout bufferLayout, UniformLayout uniformLayout);
		virtual ~PipelineLayout() = default;

		inline BufferLayout& GetBufferLayout() { return m_BufferLayout; }
		inline UniformLayout& GetUnifomLayout() { return m_UniformLayout; }

		inline void SetBufferLayout(BufferLayout bufferLayout) { m_BufferLayout = bufferLayout; }
		inline void SetUniformLayout(UniformLayout uniformLayout) { m_UniformLayout = uniformLayout; }

	private:
		BufferLayout m_BufferLayout = {};
		UniformLayout m_UniformLayout = {};
	};

}