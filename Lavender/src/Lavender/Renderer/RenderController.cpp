#include "lvpch.h"
#include "RenderController.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/APIs/Vulkan/VulkanManager.hpp"
#include "Lavender/Renderer/Renderer.hpp"
#include "Lavender/Renderer/GraphicsContext.hpp"

namespace Lavender
{

	static VkFormat BufferDataTypeToVulkanType(BufferDataType type);
	static VkDescriptorType UniformDataTypeToVulkanDescriptorType(UniformDataType type);
	static VkShaderStageFlags UniformStageFlagsToVulkanStageFlags(ShaderStageEnum flags);

	RenderController::RenderController()
	{
	}

	RenderController::~RenderController()
	{
		auto& info = VulkanManager::GetDeviceInfo();

		vkDestroyPipeline(info.Device, m_GraphicsPipeline, nullptr);
		vkDestroyPipelineLayout(info.Device, m_PipelineLayout, nullptr);

		for (auto& pool : m_DescriptorPools)
		{
			vkDestroyDescriptorPool(info.Device, pool, nullptr);
		}

		for (auto& layout : m_DescriptorLayouts)
		{
			vkDestroyDescriptorSetLayout(info.Device, layout, nullptr);
		}
	}

	void RenderController::Use()
	{
		auto& resourceInfo = VulkanManager::GetResourceInfo();

		auto& cmdBuf = resourceInfo.CommandBuffers[Renderer::GetCurrentFrame()];
		vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);
		// TODO(Jorben): Make it support multiple sets
		vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &m_DescriptorSets[0][Renderer::GetCurrentFrame()], 0, nullptr);
	}

	void RenderController::SetShader(std::shared_ptr<Shader>& shader)
	{
		m_Shader = shader;
	}

	void RenderController::SetBufferLayout(BufferLayout& layout)
	{
		m_BufferLayout = layout;
	}

	void RenderController::SetUniformLayout(UniformLayout& layout)
	{
		m_UniformLayout = layout;
	}

	void RenderController::Initialize()
	{
		CreateDescriptorSetLayout();
		CreateGraphicsPipeline();
		CreateDescriptorPool();
		CreateDescriptorSets();
	}

	void RenderController::AddUBO(std::array<VkBuffer, LV_MAX_FRAMES_IN_FLIGHT>& buffers, size_t size, uint32_t binding, uint32_t count)
	{
		// Initialize the descriptor sets/uniforms
		for (size_t i = 0; i < LV_MAX_FRAMES_IN_FLIGHT; i++)
		{
			// Uniform
			VkDescriptorBufferInfo bufferInfo = {};
			bufferInfo.buffer = buffers[i];
			bufferInfo.offset = 0;
			bufferInfo.range = size;

			// TODO(Jorben): Make this be able to use multiple descriptor sets
			VkWriteDescriptorSet descriptorWrite = {};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = m_DescriptorSets[0][i];
			descriptorWrite.dstBinding = binding;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = count;
			descriptorWrite.pBufferInfo = &bufferInfo;

			vkUpdateDescriptorSets(VulkanManager::GetDeviceInfo().Device, 1, &descriptorWrite, 0, nullptr);
		}
	}

	void RenderController::AddImage(VkImageView imageView, VkSampler sampler, uint32_t binding, uint32_t count)
	{
		// Initialize the descriptor sets/uniforms
		for (size_t i = 0; i < LV_MAX_FRAMES_IN_FLIGHT; i++)
		{
			VkDescriptorImageInfo imageInfo = {};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = imageView;
			imageInfo.sampler = sampler;

			// TODO(Jorben): Make this be able to use multiple descriptor sets
			VkWriteDescriptorSet descriptorWrite = {};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = m_DescriptorSets[0][i];
			descriptorWrite.dstBinding = binding;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrite.descriptorCount = count;
			descriptorWrite.pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(VulkanManager::GetDeviceInfo().Device, 1, &descriptorWrite, 0, nullptr);
		}
	}

	void RenderController::CreateDescriptorSetLayout()
	{
		std::vector<VkDescriptorSetLayoutBinding> layouts = { };

		for (auto& element : m_UniformLayout)
		{
			VkDescriptorSetLayoutBinding layoutBinding = {};
			layoutBinding.binding = element.Binding;
			layoutBinding.descriptorType = UniformDataTypeToVulkanDescriptorType(element.Type);
			layoutBinding.descriptorCount = element.Count;
			layoutBinding.stageFlags = UniformStageFlagsToVulkanStageFlags(element.ShaderStage);
			layoutBinding.pImmutableSamplers = nullptr; // Optional

			layouts.push_back(layoutBinding);
		}

		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(m_UniformLayout.GetElements().size());
		layoutInfo.pBindings = layouts.data();

		// TODO(Jorben): Expand to be able to support multiple sets?
		m_DescriptorLayouts.resize((size_t)1);
		if (vkCreateDescriptorSetLayout(VulkanManager::GetDeviceInfo().Device, &layoutInfo, nullptr, m_DescriptorLayouts.data()) != VK_SUCCESS)
			LV_LOG_ERROR("Failed to create descriptor set layout!");
	}

	void RenderController::CreateGraphicsPipeline()
	{
		VkShaderModule vertShaderModule = m_Shader->GetVertexShader();
		VkShaderModule fragShaderModule = m_Shader->GetFragmentShader();

		// Vertex shader info
		VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName = "main";

		// Fragment shader info
		VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";

		// PipelineShader info
		VkPipelineShaderStageCreateInfo shaderStages[2] = { vertShaderStageInfo, fragShaderStageInfo };

		auto bindingDescription = GetBindingDescription();
		auto attributeDescriptions = GetAttributeDescriptions();

		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		VkPipelineRasterizationStateCreateInfo rasterizer = {};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT; // Change to VK_CULL_MODE_BACK_BIT? // VK_CULL_MODE_FRONT_BIT
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;

		VkPipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE; // Note(Jorben): Set true for transparancy

		VkPipelineColorBlendStateCreateInfo colorBlending = {};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;

		VkPipelineDepthStencilStateCreateInfo depthStencil = {};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.minDepthBounds = 0.0f; // Optional
		depthStencil.maxDepthBounds = 1.0f; // Optional
		depthStencil.stencilTestEnable = VK_FALSE;
		depthStencil.front = {}; // Optional
		depthStencil.back = {}; // Optional

		std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicState = {};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(m_DescriptorLayouts.size());
		pipelineLayoutInfo.pSetLayouts = m_DescriptorLayouts.data();

		auto& info = VulkanManager::GetDeviceInfo();

		if (vkCreatePipelineLayout(info.Device, &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
			LV_LOG_ERROR("Failed to create pipeline layout!");

		// Create the actual graphics pipeline (where we actually use the shaders and other info)
		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = m_PipelineLayout;
		pipelineInfo.renderPass = GraphicsContext::Get()->GetResources()->GetRenderPasses()[0]->GetVkRenderPass(); // Note(Jorben): Maybe change this with multiple renderpasses maybe
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		pipelineInfo.basePipelineIndex = -1; // Optional

		if (vkCreateGraphicsPipelines(info.Device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline) != VK_SUCCESS)
			LV_LOG_ERROR("Failed to create graphics pipeline!");
	}

	void RenderController::CreateDescriptorPool()
	{
		std::vector<VkDescriptorPoolSize> poolSizes = { };
		poolSizes.resize((size_t)m_UniformLayout.UniqueCount());
		poolSizes.clear(); // Note(Jorben): For some reason without this line there is a VK_SAMPLER or something in the list.

		for (auto& type : m_UniformLayout.UniqueTypes())
		{
			VkDescriptorPoolSize poolSize = {};
			poolSize.type = UniformDataTypeToVulkanDescriptorType(type);
			poolSize.descriptorCount = m_UniformLayout.AmountOf(type) * LV_MAX_FRAMES_IN_FLIGHT;

			poolSizes.push_back(poolSize);
		}

		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = static_cast<uint32_t>(LV_MAX_FRAMES_IN_FLIGHT); // Amount of sets?

		// TODO(Jorben): Add support for multiple pools
		m_DescriptorPools.resize((size_t)1);
		if (vkCreateDescriptorPool(VulkanManager::GetDeviceInfo().Device, &poolInfo, nullptr, m_DescriptorPools.data()) != VK_SUCCESS)
			LV_LOG_ERROR("Failed to create descriptor pool!");
	}

	void RenderController::CreateDescriptorSets()
	{
		// TODO(Jorben): Add support for multiple sets (remove hardcoded [0])
		std::vector<VkDescriptorSetLayout> layouts(LV_MAX_FRAMES_IN_FLIGHT, m_DescriptorLayouts[0]);

		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_DescriptorPools[0];
		allocInfo.descriptorSetCount = static_cast<uint32_t>(LV_MAX_FRAMES_IN_FLIGHT);
		allocInfo.pSetLayouts = layouts.data();

		m_DescriptorSets.resize((size_t)1);
		m_DescriptorSets[0].resize((size_t)LV_MAX_FRAMES_IN_FLIGHT);
		if (vkAllocateDescriptorSets(VulkanManager::GetDeviceInfo().Device, &allocInfo, m_DescriptorSets[0].data()) != VK_SUCCESS)
			LV_LOG_ERROR("Failed to allocate descriptor sets!");
	}

	VkVertexInputBindingDescription RenderController::GetBindingDescription()
	{
		VkVertexInputBindingDescription description = {};
		description.binding = 0;
		description.stride = m_BufferLayout.GetStride();
		description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return description;
	}

	std::vector<VkVertexInputAttributeDescription> RenderController::GetAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions = {};
		attributeDescriptions.resize(m_BufferLayout.GetElements().size());

		auto& elements = m_BufferLayout.GetElements();
		for (size_t i = 0; i < elements.size(); i++)
		{
			attributeDescriptions[i].binding = 0;
			attributeDescriptions[i].location = elements[i].Location;
			attributeDescriptions[i].format = BufferDataTypeToVulkanType(elements[i].Type);
			attributeDescriptions[i].offset = (uint32_t)elements[i].Offset;
		}

		return attributeDescriptions;
	}

	static VkFormat BufferDataTypeToVulkanType(BufferDataType type)
	{
		switch (type)
		{
		case BufferDataType::Float:   return VK_FORMAT_R32_SFLOAT;
		case BufferDataType::Float2:  return VK_FORMAT_R32G32_SFLOAT;
		case BufferDataType::Float3:  return VK_FORMAT_R32G32B32_SFLOAT;
		case BufferDataType::Float4:  return VK_FORMAT_R32G32B32A32_SFLOAT;
		case BufferDataType::Mat3:    return VK_FORMAT_UNDEFINED;		// TODO(Jorben): Implement these
		case BufferDataType::Mat4:    return VK_FORMAT_UNDEFINED;		// TODO(Jorben): Implement these
		case BufferDataType::Int:     return VK_FORMAT_UNDEFINED;		// TODO(Jorben): Implement these
		case BufferDataType::Int2:    return VK_FORMAT_UNDEFINED;		// TODO(Jorben): Implement these
		case BufferDataType::Int3:    return VK_FORMAT_UNDEFINED;		// TODO(Jorben): Implement these
		case BufferDataType::Int4:    return VK_FORMAT_UNDEFINED;		// TODO(Jorben): Implement these
		case BufferDataType::Bool:    return VK_FORMAT_UNDEFINED;		// TODO(Jorben): Implement these
		}

		return VK_FORMAT_UNDEFINED;
	}

	static VkDescriptorType UniformDataTypeToVulkanDescriptorType(UniformDataType type)
	{
		switch (type)
		{
		case UniformDataType::UBO: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		case UniformDataType::Image: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			// TODO(Jorben): Implement the rest
		}

		return VK_DESCRIPTOR_TYPE_MAX_ENUM;
	}

	static VkShaderStageFlags UniformStageFlagsToVulkanStageFlags(ShaderStageEnum flags)
	{
		VkShaderStageFlags result = 0;
		if (flags & UniformElement_Stage_Vertex)
			result |= VK_SHADER_STAGE_VERTEX_BIT;
		if (flags & UniformElement_Stage_Fragment)
			result |= VK_SHADER_STAGE_FRAGMENT_BIT;

		return result;
	}

}