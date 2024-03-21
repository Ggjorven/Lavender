#pragma once

#include <memory>
#include <unordered_map>

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Renderer/Pipeline.hpp"
#include "Lavender/Renderer/Shader.hpp"
#include "Lavender/Renderer/RenderPass.hpp"

#include <vulkan/vulkan.h>

namespace Lavender
{

	class VulkanUniformBuffer;
	class VulkanImage2D;

	class VulkanPipeline : public Pipeline
	{
	public:
		VulkanPipeline();
		VulkanPipeline(PipelineSpecification specs);
		VulkanPipeline(PipelineSpecification specs, Ref<Shader> shader);
		VulkanPipeline(PipelineSpecification specs, Ref<RenderPass> renderpass);
		VulkanPipeline(PipelineSpecification specs, Ref<Shader> shader, Ref<RenderPass> renderpass);
		virtual ~VulkanPipeline();

		void Initialize() override;
		void Use(Ref<RenderCommandBuffer> commandBuffer) override;

		inline void SetShader(Ref<Shader> shader) override { m_Shader = shader; }

		inline PipelineSpecification& GetSpecification() override { return m_Specification; };

	private:
		void CreateDescriptorSetLayout();
		void CreateGraphicsPipeline();
		void CreateDescriptorPool();
		void CreateDescriptorSets();

		VkVertexInputBindingDescription GetBindingDescription();
		std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();

	private:
		Ref<Shader> m_Shader = nullptr;
		Ref<RenderPass> m_RenderPass = nullptr;

		PipelineSpecification m_Specification = {};

		VkPipeline m_GraphicsPipeline = VK_NULL_HANDLE;
		VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;

		std::unordered_map<SetID, VkDescriptorSetLayout> m_DescriptorLayouts = { };
		std::unordered_map<SetID, VkDescriptorPool> m_DescriptorPools = { };

		// Note(Jorben): The first index is the index of the set and the second are RenderConfig::FramesInFlight of sets.
		std::unordered_map<SetID, std::vector<VkDescriptorSet>> m_DescriptorSets = { };

		friend class VulkanUniformBuffer;
		friend class VulkanImage2D;
	};

}