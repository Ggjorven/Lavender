#pragma once

#include <memory>
#include <array>

#include "Lavender/Renderer/Shader.hpp"
#include "Lavender/Renderer/VertexBuffer.hpp"

#include "Lavender/Renderer/UniformInfo.hpp"

namespace Lavender
{

	// TODO: Add more customizability
	class RenderController
	{
	public:
		RenderController();
		virtual ~RenderController();

		void Use();

		void SetShader(std::shared_ptr<Shader>& shader);
		void SetBufferLayout(BufferLayout& layout);
		void SetUniformLayout(UniformLayout& layout);

		void Initialize();

		void AddUBO(std::array<VkBuffer, LV_MAX_FRAMES_IN_FLIGHT>& buffers, size_t size, uint32_t binding = 0, uint32_t count = 1);
		void AddImage(VkImageView imageView, VkSampler sampler, uint32_t binding = 0, uint32_t count = 1);

	private:
		void CreateDescriptorSetLayout();
		void CreateGraphicsPipeline();
		void CreateDescriptorPool();
		void CreateDescriptorSets();

		VkVertexInputBindingDescription GetBindingDescription();
		std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();

	private:
		std::shared_ptr<Shader> m_Shader = nullptr;
		BufferLayout m_BufferLayout = {};
		UniformLayout m_UniformLayout = {};

		VkPipeline m_GraphicsPipeline = VK_NULL_HANDLE;
		VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;

		std::vector<VkDescriptorSetLayout> m_DescriptorLayouts = { };
		std::vector<VkDescriptorPool> m_DescriptorPools = { };

		// Note(Jorben): The first index is the index of the descriptor and the second are LV_MAX_FRAMES_INFLIGHT of sets.
		std::vector<std::vector<VkDescriptorSet>> m_DescriptorSets = { };
	};

}