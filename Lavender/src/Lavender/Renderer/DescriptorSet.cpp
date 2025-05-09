#include "lvpch.h"
#include "DescriptorSet.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/Renderer.hpp"

#include "Lavender/APIs/Vulkan/VulkanDescriptorSet.hpp"

namespace Lavender
{

	DescriptorSetGroup::DescriptorCount::DescriptorCount()
	{
		m_Counts[0] = 1;
		m_Counts[1] = 1;
		m_Counts[2] = 1;
		m_Counts[3] = 1;
	}

	DescriptorSetGroup::DescriptorCount::DescriptorCount(Dict<SetID, uint32_t> count)
		: m_Counts(count)
	{
	}

	void DescriptorSetGroup::DescriptorCount::SetCount(SetID set, uint32_t count)
	{
		m_Counts[set] = count;
	}

	uint32_t DescriptorSetGroup::DescriptorCount::GetCount(SetID set) const
	{
		auto it = m_Counts.find(set);
		return it->second;
	}

	Ref<DescriptorSetGroup> DescriptorSetGroup::Create(const UniformLayout& layout, DescriptorCount count)
	{
		switch (Renderer::GetAPI())
		{
		case RenderingAPI::Vulkan:
			return RefHelper::Create<VulkanDescriptorSetGroup>(layout, count);

		default:
			LV_LOG_ERROR("Invalid API selected.");
			break;
		}

		return nullptr;
	}

}