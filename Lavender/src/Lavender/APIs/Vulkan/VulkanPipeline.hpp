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

	class VulkanImage2D;
	class VulkanUniformBuffer;
	class VulkanDescriptorSetGroup;

	class VulkanPipeline : public Pipeline
	{
	public:
		VulkanPipeline();
		VulkanPipeline(PipelineSpecification specs, Ref<DescriptorSetGroup> sets);
		VulkanPipeline(PipelineSpecification specs, Ref<DescriptorSetGroup> sets, Ref<Shader> shader);
		VulkanPipeline(PipelineSpecification specs, Ref<DescriptorSetGroup> sets, Ref<RenderPass> renderpass);
		VulkanPipeline(PipelineSpecification specs, Ref<DescriptorSetGroup> sets, Ref<Shader> shader, Ref<RenderPass> renderpass);
		virtual ~VulkanPipeline();

		void Initialize() override;
		void Use(Ref<RenderCommandBuffer> commandBuffer) override;

		inline void SetShader(Ref<Shader> shader) override { m_Shader = shader; }

		inline PipelineSpecification& GetSpecification() override { return m_Specification; };
		inline Ref<DescriptorSetGroup> GetDescriptorSets() override { return m_Sets; }

		inline VkPipelineLayout GetVulkanLayout() { return m_PipelineLayout; }

	private:
		void CreateGraphicsPipeline();

		VkVertexInputBindingDescription GetBindingDescription();
		std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();

	private:
		Ref<Shader> m_Shader = nullptr;
		Ref<RenderPass> m_RenderPass = nullptr;

		PipelineSpecification m_Specification = {};
		Ref<DescriptorSetGroup> m_Sets = nullptr;

		VkPipeline m_GraphicsPipeline = VK_NULL_HANDLE;
		VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;

		friend class VulkanImage2D;
		friend class VulkanUniformBuffer;
		friend class VulkanDescriptorSetGroup;
	};

}