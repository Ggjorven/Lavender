#include "lvpch.h"
#include "UniformBuffer.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/RenderController.hpp"
#include "Lavender/APIs/Vulkan/Setup/VulkanBufferHelper.hpp"

namespace Lavender
{

	UniformElement::UniformElement(UniformDataType type, uint32_t binding, const std::string& name, ShaderStageEnum shaderstage, uint32_t count)
		: Type(type), Binding(binding), Name(name), ShaderStage(shaderstage), Count(count)
	{
	}

	UniformLayout::UniformLayout(const std::initializer_list<UniformElement>& elements)
		: m_Elements(elements)
	{
	}

	uint32_t UniformLayout::UniqueCount() const
	{
		return (uint32_t)UniqueTypes().size();
	}

	std::unordered_set<UniformDataType> UniformLayout::UniqueTypes() const
	{
		std::unordered_set<UniformDataType> unique = {};

		for (auto& element : m_Elements)
		{
			unique.insert(element.Type);
		}

		return unique;
	}

	uint32_t UniformLayout::AmountOf(UniformDataType type) const
	{
		uint32_t count = 0;

		for (auto& element : m_Elements)
		{
			if (element.Type == type)
				count++;
		}

		return count;
	}
}