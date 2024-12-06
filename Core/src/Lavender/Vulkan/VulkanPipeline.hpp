#pragma once

#include <memory>
#include <unordered_map>

#include "Lavender/Core/Core.hpp"
#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Renderer/Shader.hpp"
#include "Lavender/Renderer/Pipeline.hpp"
#include "Lavender/Renderer/RenderPass.hpp"
#include "Lavender/Renderer/Descriptors.hpp"

#include <vulkan/vulkan.h>

namespace Lavender
{

	class VulkanDescriptorSets;

	VkPipelineBindPoint PipelineBindPointToVulkanBindPoint(PipelineBindPoint bindPoint);

	class VulkanPipeline : public Pipeline
	{
	public:
		VulkanPipeline(PipelineSpecification specs, Ref<DescriptorSets> sets, Ref<Shader> shader, Ref<RenderPass> renderpass);
		VulkanPipeline(PipelineSpecification specs, Ref<DescriptorSets> sets, Ref<ComputeShader> shader);
		virtual ~VulkanPipeline();

		void Use(Ref<CommandBuffer> commandBuffer, PipelineBindPoint bindPoint) override;

		inline PipelineSpecification& GetSpecification() override { return m_Specification; };
		inline Ref<DescriptorSets> GetDescriptorSets() override { return m_Sets; }

		inline VkPipelineLayout GetVulkanLayout() { return m_PipelineLayout; }

	private:
		void CreateGraphicsPipeline();
		void CreateComputePipeline();
		void CreateRayTracingPipeline(); // TODO: Implement

		VkVertexInputBindingDescription GetBindingDescription();
		std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();

	private:
		Ref<Shader> m_Shader = nullptr;
		Ref<ComputeShader> m_ComputeShader = nullptr;
		Ref<RenderPass> m_RenderPass = nullptr;

		PipelineSpecification m_Specification = {};
		Ref<DescriptorSets> m_Sets = nullptr;

		VkPipeline m_GraphicsPipeline = VK_NULL_HANDLE;
		VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;

		friend class VulkanDescriptorSets;
	};

}