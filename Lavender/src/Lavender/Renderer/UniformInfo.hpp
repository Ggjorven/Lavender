#pragma once

#include <string>
#include <memory>
#include <unordered_set>
#include <array>

#include "Lavender/APIs/Vulkan/Setup/VulkanResources.hpp"
#include "Lavender/Utils/Utils.hpp"

namespace Lavender
{

	enum class UniformDataType
	{
		None = 0, UBO, Image
	};

	enum ShaderStageEnum
	{
		UniformElement_Stage_None = -1, UniformElement_Stage_Vertex = BIT(0), UniformElement_Stage_Fragment = BIT(1)
	};

	struct UniformElement
	{
	public:
		std::string Name = "Empty";
		uint32_t Binding = 0;
		UniformDataType Type = UniformDataType::None;
		uint32_t Count = 1;
		ShaderStageEnum ShaderStage = UniformElement_Stage_None;

		UniformElement() = default;
		UniformElement(UniformDataType type, uint32_t binding, const std::string& name, ShaderStageEnum shaderstage, uint32_t count = 1);
	};

	class UniformLayout
	{
	public:
		UniformLayout() = default;
		UniformLayout(const std::initializer_list<UniformElement>& elements);

		inline const std::vector<UniformElement>& GetElements() const { return m_Elements; }

		std::vector<UniformElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<UniformElement>::iterator end() { return m_Elements.end(); }
		std::vector<UniformElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<UniformElement>::const_iterator end() const { return m_Elements.end(); }

		uint32_t UniqueCount() const;
		std::unordered_set<UniformDataType> UniqueTypes() const;
		uint32_t AmountOf(UniformDataType type) const;

	private:
		std::vector<UniformElement> m_Elements = { };
	};

}