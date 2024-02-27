#pragma once

#include <memory>

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Renderer/Pipeline.hpp"

#include <vulkan/vulkan.h>

namespace Lavender
{

	class VulkanPipeline : public Pipeline
	{
	public:
		VulkanPipeline();
		VulkanPipeline(PipelineLayout layout);
		virtual ~VulkanPipeline();

		void Use() override;

	private:
		void CreateDescriptorSetLayout();
		void CreateGraphicsPipeline();
		void CreateDescriptorPool();
		void CreateDescriptorSets();

		VkVertexInputBindingDescription GetBindingDescription();
		std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();

	private:
		PipelineLayout m_Layout = {};

		VkPipeline m_GraphicsPipeline = VK_NULL_HANDLE;
		VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;

		std::vector<VkDescriptorSetLayout> m_DescriptorLayouts = { };
		std::vector<VkDescriptorPool> m_DescriptorPools = { };

		// Note(Jorben): The first index is the index of the descriptor and the second are RenderConfig::FramesInFlight of sets.
		std::vector<std::vector<VkDescriptorSet>> m_DescriptorSets = { };
	};

}