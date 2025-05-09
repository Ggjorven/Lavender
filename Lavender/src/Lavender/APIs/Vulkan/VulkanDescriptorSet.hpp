#pragma once

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Renderer/DescriptorSet.hpp"

#include <vulkan/vulkan.h>

namespace Lavender
{
	
	class VulkanPipeline;
	class VulkanDescriptorSetGroup;

	class VulkanDescriptorSet : public DescriptorSet
	{
	public:
		VulkanDescriptorSet(SetID id, std::vector<VkDescriptorSet> sets);
		virtual ~VulkanDescriptorSet();

		void Bind(Ref<Pipeline> pipeline, Ref<RenderCommandBuffer> cmdBuffer) override;
		void Bind(Ref<Pipeline> pipeline, Ref<RenderCommandBuffer> cmdBuffer, const std::vector<uint32_t>& offset) override;
		inline SetID GetSetID() override { return m_ID; }

		VkDescriptorSet GetVulkanSet(uint32_t index) { return m_DescriptorSets[index]; }

	private:
		SetID m_ID = -1;
		std::vector<VkDescriptorSet> m_DescriptorSets = { }; // Amount of FramesInFlight sets

		friend VulkanDescriptorSetGroup;
	};

	class VulkanDescriptorSetGroup : public DescriptorSetGroup
	{
	public:
		VulkanDescriptorSetGroup(const UniformLayout& layout, DescriptorSetGroup::DescriptorCount count);
		virtual ~VulkanDescriptorSetGroup();

		void AddMoreSetsTo(SetID id, uint32_t amount) override;

		inline UniformLayout& GetLayout() override { return m_Layout; }
		inline const DescriptorCount& GetCount() override { return m_Count; }

		std::vector<Ref<DescriptorSet>>& GetSets(SetID id) override;

	private:
		void CreateDescriptorSetLayout();
		void CreateDescriptorPool();
		void CreateDescriptorSets();
		void ConvertToVulkanDescriptorSets(Dict<SetID, std::vector<VkDescriptorSet>>& vulkanSets);

	private:
		UniformLayout m_Layout = {};
		DescriptorSetGroup::DescriptorCount m_Count = {};

		Dict<SetID, VkDescriptorSetLayout> m_DescriptorLayouts = { };
		Dict<SetID, VkDescriptorPool> m_DescriptorPools = { };

		Dict<SetID, std::vector<Ref<DescriptorSet>>> m_Sets = { };

		friend class VulkanPipeline;
		friend class VulkanDescriptorSet;
	};

}