#include "lvpch.h"
#include "VulkanPipeline.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/Renderer.hpp"
#include "Lavender/Renderer/PipelineLayout.hpp"

#include "Lavender/APIs/Vulkan/VulkanContext.hpp"
#include "Lavender/APIs/Vulkan/VulkanShader.hpp"
#include "Lavender/APIs/Vulkan/VulkanRenderPass.hpp"
#include "Lavender/APIs/Vulkan/VulkanRenderCommandBuffer.hpp"

namespace Lavender
{

	static VkFormat DataTypeToVulkanType(DataType type);
	static VkDescriptorType UniformDataTypeToVulkanDescriptorType(UniformDataType type);
	static VkShaderStageFlags UniformStageFlagsToVulkanStageFlags(UniformElement::ShaderStage flags);

	VulkanPipeline::VulkanPipeline()
	{
	}

	VulkanPipeline::VulkanPipeline(PipelineSpecification specs)
		: m_Specification(specs)
	{
	}

	VulkanPipeline::VulkanPipeline(PipelineSpecification specs, Ref<Shader> shader)
		: m_Specification(specs), m_Shader(shader)
	{
	}

	VulkanPipeline::VulkanPipeline(PipelineSpecification specs, Ref<RenderPass> renderpass)
		: m_Specification(specs), m_RenderPass(renderpass)
	{
	}

	VulkanPipeline::VulkanPipeline(PipelineSpecification specs, Ref<Shader> shader, Ref<RenderPass> renderpass)
		: m_Specification(specs), m_Shader(shader), m_RenderPass(renderpass)
	{
	}

	VulkanPipeline::~VulkanPipeline()
	{
		auto device = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetLogicalDevice()->GetVulkanDevice();

		vkDestroyPipeline(device, m_GraphicsPipeline, nullptr);
		vkDestroyPipelineLayout(device, m_PipelineLayout, nullptr);

		for (auto& pool : m_DescriptorPools)
		{
			vkDestroyDescriptorPool(device, pool.second, nullptr);
		}

		for (auto& layout : m_DescriptorLayouts)
		{
			vkDestroyDescriptorSetLayout(device, layout.second, nullptr);
		}
	}

	void VulkanPipeline::Initialize()
	{
		if (!m_Shader || !m_RenderPass)
		{
			LV_LOG_ERROR("Tried to initialize Pipeline before setting the shader or renderpass.");
			return;
		}

		CreateDescriptorSetLayout();
		CreateGraphicsPipeline();
		CreateDescriptorPool();
		CreateDescriptorSets();
	}

	void VulkanPipeline::Use(Ref<RenderCommandBuffer> commandBuffer)
	{
		auto cmdBuf = RefHelper::RefAs<VulkanRenderCommandBuffer>(commandBuffer);
		uint32_t currentFrame = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetSwapChain()->GetCurrentFrame();

		vkCmdBindPipeline(cmdBuf->GetVulkanCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);

		// Bind all sets to appropriate slots
		for (auto& set : m_DescriptorSets)
		{
			vkCmdBindDescriptorSets(cmdBuf->GetVulkanCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, set.first, 1, &set.second[currentFrame], 0, nullptr);
		}
	}

	// Note(Jorben): Just for myself, this specifies all the different types that are gonna be in a descriptor set
	void VulkanPipeline::CreateDescriptorSetLayout()
	{
		for (auto& set : m_Specification.Uniformlayout.GetElements())
		{
			std::vector<VkDescriptorSetLayoutBinding> layouts = { };

			for (auto& element : set.second)
			{
				VkDescriptorSetLayoutBinding layoutBinding = {};
				layoutBinding.binding = element.Binding;
				layoutBinding.descriptorType = UniformDataTypeToVulkanDescriptorType(element.Type);
				layoutBinding.descriptorCount = element.Count;
				layoutBinding.stageFlags = UniformStageFlagsToVulkanStageFlags(element.Stage);
				layoutBinding.pImmutableSamplers = nullptr; // Optional

				layouts.push_back(layoutBinding);
			}

			VkDescriptorSetLayoutCreateInfo layoutInfo = {};
			layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			layoutInfo.bindingCount =(uint32_t)layouts.size();
			layoutInfo.pBindings = layouts.data();

			m_DescriptorLayouts[set.first] = VK_NULL_HANDLE;
			if (vkCreateDescriptorSetLayout(RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetLogicalDevice()->GetVulkanDevice(), &layoutInfo, nullptr, &m_DescriptorLayouts[set.first]) != VK_SUCCESS)
				LV_LOG_ERROR("Failed to create descriptor set layout!");
		}
	}

	void VulkanPipeline::CreateGraphicsPipeline() // TODO(Jorben): Add customizability
	{
		auto vulkanShader = RefHelper::RefAs<VulkanShader>(m_Shader);

		std::vector<VkPipelineShaderStageCreateInfo> shaderStages = { };

		auto vertex = vulkanShader->GetVertexShader();
		if (vertex)
		{
			VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
			vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
			vertShaderStageInfo.module = vertex;
			vertShaderStageInfo.pName = "main";

			shaderStages.push_back(vertShaderStageInfo);
		}

		auto fragment = vulkanShader->GetFragmentShader();
		if (fragment)
		{
			VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
			fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			fragShaderStageInfo.module = fragment;
			fragShaderStageInfo.pName = "main";

			shaderStages.push_back(fragShaderStageInfo);
		}
		
		auto bindingDescription = GetBindingDescription();
		auto attributeDescriptions = GetAttributeDescriptions();
		
		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.vertexAttributeDescriptionCount = (uint32_t)attributeDescriptions.size();
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
		rasterizer.polygonMode = (VkPolygonMode)(m_Specification.Polygonmode);
		rasterizer.lineWidth = m_Specification.LineWidth;
		rasterizer.cullMode = (VkCullModeFlags)(m_Specification.Cullingmode); // Change to VK_CULL_MODE_BACK_BIT? // VK_CULL_MODE_FRONT_BIT
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;
		
		VkPipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		
		VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = m_Specification.Blending; // Note(Jorben): Set true for transparancy
		
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
		dynamicState.dynamicStateCount = (uint32_t)dynamicStates.size();
		dynamicState.pDynamicStates = dynamicStates.data();
		
		// Descriptor layouts
		std::vector<VkDescriptorSetLayout> descriptorLayouts = { };
		descriptorLayouts.reserve(m_DescriptorLayouts.size());

		for (auto& pair : m_DescriptorLayouts)
			descriptorLayouts.push_back(pair.second);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.setLayoutCount = (uint32_t)descriptorLayouts.size();
		pipelineLayoutInfo.pSetLayouts = descriptorLayouts.data();

		auto device = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetLogicalDevice()->GetVulkanDevice();
		
		if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
			LV_LOG_ERROR("Failed to create pipeline layout!");
		
		// Create the actual graphics pipeline (where we actually use the shaders and other info)
		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = (uint32_t)shaderStages.size();
		pipelineInfo.pStages = shaderStages.data();
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = m_PipelineLayout;
		pipelineInfo.renderPass = RefHelper::RefAs<VulkanRenderPass>(m_RenderPass)->GetVulkanRenderPass(); 
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		pipelineInfo.basePipelineIndex = -1; // Optional
		
		if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline) != VK_SUCCESS)
			LV_LOG_ERROR("Failed to create graphics pipeline!");
	}

	void VulkanPipeline::CreateDescriptorPool()
	{
		auto device = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetLogicalDevice()->GetVulkanDevice();

		for (auto& set : m_Specification.Uniformlayout.GetElements())
		{
			// Note(Jorben): Just for myself, the poolSizes is just the amount of elements of a certain type to able to allocate per pool
			std::vector<VkDescriptorPoolSize> poolSizes = { };
			poolSizes.resize((size_t)m_Specification.Uniformlayout.UniqueCount(set.first));
			poolSizes.clear(); // Note(Jorben): For some reason without this line there is a VK_SAMPLER or something in the list.

			for (auto& type : m_Specification.Uniformlayout.UniqueTypes(set.first))
			{
				VkDescriptorPoolSize poolSize = {};
				poolSize.type = UniformDataTypeToVulkanDescriptorType(type);
				poolSize.descriptorCount = m_Specification.Uniformlayout.AmountOf(set.first, type) * Renderer::GetSpecification().FramesInFlight;

				poolSizes.push_back(poolSize);
			}

			VkDescriptorPoolCreateInfo poolInfo = {};
			poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			poolInfo.poolSizeCount = (uint32_t)poolSizes.size();
			poolInfo.pPoolSizes = poolSizes.data();
			poolInfo.maxSets = (uint32_t)Renderer::GetSpecification().FramesInFlight; // A set for every frame in flight

			m_DescriptorPools[set.first] = VK_NULL_HANDLE;
			if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &m_DescriptorPools[set.first]) != VK_SUCCESS)
				LV_LOG_ERROR("Failed to create descriptor pool!");
		}
	}

	void VulkanPipeline::CreateDescriptorSets()
	{
		auto device = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetLogicalDevice()->GetVulkanDevice();
		
		for (auto& set : m_Specification.Uniformlayout.GetElements())
		{
			std::vector<VkDescriptorSetLayout> layouts(Renderer::GetSpecification().FramesInFlight, m_DescriptorLayouts[set.first]);

			VkDescriptorSetAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = m_DescriptorPools[set.first];
			allocInfo.descriptorSetCount = (uint32_t)Renderer::GetSpecification().FramesInFlight;
			allocInfo.pSetLayouts = layouts.data();

			m_DescriptorSets[set.first].resize((size_t)Renderer::GetSpecification().FramesInFlight);
			if (vkAllocateDescriptorSets(device, &allocInfo, m_DescriptorSets[set.first].data()) != VK_SUCCESS)
				LV_LOG_ERROR("Failed to allocate descriptor sets!");
		}
	}

	VkVertexInputBindingDescription VulkanPipeline::GetBindingDescription()
	{
		VkVertexInputBindingDescription description = {};
		description.binding = 0;
		description.stride = m_Specification.Bufferlayout.GetStride();
		description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return description;
	}

	std::vector<VkVertexInputAttributeDescription> VulkanPipeline::GetAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions = {};
		attributeDescriptions.resize(m_Specification.Bufferlayout.GetElements().size());

		auto& elements = m_Specification.Bufferlayout.GetElements();
		for (size_t i = 0; i < elements.size(); i++)
		{
			attributeDescriptions[i].binding = 0;
			attributeDescriptions[i].location = elements[i].Location;
			attributeDescriptions[i].format = DataTypeToVulkanType(elements[i].Type);
			attributeDescriptions[i].offset = (uint32_t)elements[i].Offset;
		}

		return attributeDescriptions;
	}

	static VkFormat DataTypeToVulkanType(DataType type)
	{
		switch (type)
		{
		case DataType::Float:   return VK_FORMAT_R32_SFLOAT;
		case DataType::Float2:  return VK_FORMAT_R32G32_SFLOAT;
		case DataType::Float3:  return VK_FORMAT_R32G32B32_SFLOAT;
		case DataType::Float4:  return VK_FORMAT_R32G32B32A32_SFLOAT;
		case DataType::Mat3:    return VK_FORMAT_UNDEFINED;		// TODO(Jorben): Implement these
		case DataType::Mat4:    return VK_FORMAT_UNDEFINED;		// TODO(Jorben): Implement these
		case DataType::Int:     return VK_FORMAT_UNDEFINED;		// TODO(Jorben): Implement these
		case DataType::Int2:    return VK_FORMAT_UNDEFINED;		// TODO(Jorben): Implement these
		case DataType::Int3:    return VK_FORMAT_UNDEFINED;		// TODO(Jorben): Implement these
		case DataType::Int4:    return VK_FORMAT_UNDEFINED;		// TODO(Jorben): Implement these
		case DataType::Bool:    return VK_FORMAT_UNDEFINED;		// TODO(Jorben): Implement these
		}

		return VK_FORMAT_UNDEFINED;
	}

	static VkDescriptorType UniformDataTypeToVulkanDescriptorType(UniformDataType type)
	{
		switch (type)
		{
		case UniformDataType::UniformBuffer: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		case UniformDataType::Image: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		// TODO(Jorben): Implement the rest
		}

		return VK_DESCRIPTOR_TYPE_MAX_ENUM;
	}

	static VkShaderStageFlags UniformStageFlagsToVulkanStageFlags(UniformElement::ShaderStage flags)
	{
		VkShaderStageFlags result = 0;
		if (flags & UniformElement::ShaderStage::Vertex)
			result |= VK_SHADER_STAGE_VERTEX_BIT;
		if (flags & UniformElement::ShaderStage::Fragment)
			result |= VK_SHADER_STAGE_FRAGMENT_BIT;

		return result;
	}

}