#include "lvpch.h"
#include "PipelineLayout.hpp"

#include "Lavender/Core/Logging.hpp"

namespace Lavender
{

	uint32_t DataTypeSize(DataType type)
	{
		switch (type)
		{
		case DataType::Float:    return 4;
		case DataType::Float2:   return 4 * 2;
		case DataType::Float3:   return 4 * 3;
		case DataType::Float4:   return 4 * 4;
		case DataType::Mat3:     return 4 * 3 * 3;
		case DataType::Mat4:     return 4 * 4 * 4;
		case DataType::Int:      return 4;
		case DataType::Int2:     return 4 * 2;
		case DataType::Int3:     return 4 * 3;
		case DataType::Int4:     return 4 * 4;
		case DataType::Bool:     return 1;
		}

		// TODO: Make assert?
		LV_LOG_ERROR("Unknown DataType!");
		return 0;
	}

	BufferElement::BufferElement(DataType type, uint32_t location, const std::string& name)
		: Name(name), Location(location), Type(type), Size(DataTypeSize(type)), Offset(0)
	{
	}

	uint32_t BufferElement::GetComponentCount() const
	{
		switch (Type)
		{
		case DataType::Float:   return 1;
		case DataType::Float2:  return 2;
		case DataType::Float3:  return 3;
		case DataType::Float4:  return 4;
		case DataType::Mat3:    return 3 * 3;
		case DataType::Mat4:    return 4 * 4;
		case DataType::Int:     return 1;
		case DataType::Int2:    return 2;
		case DataType::Int3:    return 3;
		case DataType::Int4:    return 4;
		case DataType::Bool:    return 1;
		}

		// TODO: Make assert?
		LV_LOG_ERROR("Unknown DataType!");
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



	UniformElement::UniformElement(UniformDataType type, SetID set, uint32_t binding, const std::string& name, ShaderStage stage, uint32_t count)
		: Type(type), Set(set), Binding(binding), Name(name), Stage(stage), Count(count)
	{
	}

	UniformLayout::UniformLayout(const std::initializer_list<UniformElement>& elements)
	{
		for (auto& element : elements)
			m_Elements[element.Set].push_back(element);

	}

	UniformElement UniformLayout::GetElementByName(SetID set, const std::string& name)
	{
		for (const auto& setIterator : m_Elements) 
		{
			if (setIterator.first == set) 
			{
				for (const auto& element : setIterator.second)
				{
					if (element.Name == name)
						return element;
				}
			}
		}

		LV_LOG_WARN("Failed to find element by name: {0}", name);
		return {};
	}


	uint32_t UniformLayout::UniqueCount(SetID set) const
	{
		return (uint32_t)UniqueTypes(set).size();
	}

	std::unordered_set<UniformDataType> UniformLayout::UniqueTypes(SetID set) const
	{
		std::unordered_set<UniformDataType> unique;

		auto it = m_Elements.find(set);
		if (it != m_Elements.end()) 
		{
			for (const auto& e : it->second)
				unique.insert(e.Type);
		}

		return unique;
	}

	uint32_t UniformLayout::AmountOf(SetID set, UniformDataType type) const
	{
		uint32_t count = 0;

		auto it = m_Elements.find(set);
		if (it != m_Elements.end()) 
		{
			for (const auto& e : it->second) 
			{
				if (e.Type == type)
					count++;
			}
		}

		return count;
	}




	PipelineLayout::PipelineLayout(BufferLayout bufferLayout, UniformLayout uniformLayout)
		: m_BufferLayout(bufferLayout), m_UniformLayout(uniformLayout)
	{
	}

}