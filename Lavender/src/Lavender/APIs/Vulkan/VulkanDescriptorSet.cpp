#include "lvpch.h"
#include "VulkanDescriptorSet.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Utils/Profiler.hpp"

#include "Lavender/Renderer/Renderer.hpp"
#include "Lavender/Renderer/PipelineLayout.hpp"
#include "Lavender/Renderer/Pipeline.hpp"
#include "Lavender/Renderer/RenderCommandBuffer.hpp"

#include "Lavender/APIs/Vulkan/VulkanUtils.hpp"
#include "Lavender/APIs/Vulkan/VulkanContext.hpp"
#include "Lavender/APIs/Vulkan/VulkanPipeline.hpp"
#include "Lavender/APIs/Vulkan/VulkanRenderCommandBuffer.hpp"

namespace Lavender
{

	static VkDescriptorType UniformDataTypeToVulkanDescriptorType(UniformDataType type);
	static VkShaderStageFlags UniformStageFlagsToVulkanStageFlags(UniformElement::ShaderStage flags);

	VulkanDescriptorSet::VulkanDescriptorSet(SetID id, std::vector<VkDescriptorSet> sets)
		: m_ID(id), m_DescriptorSets(sets)
	{
	}

	VulkanDescriptorSet::~VulkanDescriptorSet()
	{
	}

	void VulkanDescriptorSet::Bind(Ref<Pipeline> pipeline, Ref<RenderCommandBuffer> cmdBuffer)
	{
		LV_PROFILE_SCOPE("VulkanDescriptorSet::Bind");

		auto vkPipelineLayout = RefHelper::RefAs<VulkanPipeline>(pipeline)->GetVulkanLayout();
		auto vkCmdBuf = RefHelper::RefAs<VulkanRenderCommandBuffer>(cmdBuffer)->GetVulkanCommandBuffer();
		auto currentFrame = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetSwapChain()->GetCurrentFrame();

		vkCmdBindDescriptorSets(vkCmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipelineLayout, m_ID, 1, &m_DescriptorSets[currentFrame], 0, nullptr);
	}

	VulkanDescriptorSetGroup::VulkanDescriptorSetGroup(const UniformLayout& layout, DescriptorSetGroup::DescriptorCount count)
		: m_Layout(layout), m_Count(count)
	{
		CreateDescriptorSetLayout();
		CreateDescriptorPool();
		CreateDescriptorSets();
	}

	VulkanDescriptorSetGroup::~VulkanDescriptorSetGroup()
	{
		auto device = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetLogicalDevice()->GetVulkanDevice();

		for (auto& pool : m_DescriptorPools)
		{
			vkDestroyDescriptorPool(device, pool.second, nullptr);
		}

		for (auto& layout : m_DescriptorLayouts)
		{
			vkDestroyDescriptorSetLayout(device, layout.second, nullptr);
		}
	}

	void VulkanDescriptorSetGroup::AddMoreSetsTo(SetID id, uint32_t amount)
	{
		m_Count.SetCount(id, m_Count.GetCount(id) + amount);
		
		auto device = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetLogicalDevice()->GetVulkanDevice();
		vkDestroyDescriptorPool(device, m_DescriptorPools[id], nullptr);

		// Note(Jorben): Just for myself, the poolSizes is just the amount of elements of a certain type to able to allocate per pool
		std::vector<VkDescriptorPoolSize> poolSizes = { };
		poolSizes.resize((size_t)m_Layout.UniqueCount(id));
		poolSizes.clear(); // Note(Jorben): For some reason without this line there is a VK_SAMPLER or something in the list.

		for (auto& type : m_Layout.UniqueTypes(id))
		{
			VkDescriptorPoolSize poolSize = {};
			poolSize.type = UniformDataTypeToVulkanDescriptorType(type);
			poolSize.descriptorCount = m_Layout.AmountOf(id, type) * Renderer::GetSpecification().FramesInFlight;

			poolSizes.push_back(poolSize);
		}

		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = (uint32_t)poolSizes.size();
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = (uint32_t)Renderer::GetSpecification().FramesInFlight * m_Count.GetCount(id); // A set for every frame in flight

		m_DescriptorPools[id] = VK_NULL_HANDLE;
		if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &m_DescriptorPools[id]) != VK_SUCCESS)
			LV_LOG_ERROR("Failed to create descriptor pool!");

		//////////////////////////////////////////////////////////////////////////////////////////////////
		//
		//////////////////////////////////////////////////////////////////////////////////////////////////
		Dict<SetID, std::vector<VkDescriptorSet>> descriptorSets = { };

		std::vector<VkDescriptorSetLayout> layouts(Renderer::GetSpecification().FramesInFlight * m_Count.GetCount(id), m_DescriptorLayouts[id]);

		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_DescriptorPools[id];
		allocInfo.descriptorSetCount = (uint32_t)Renderer::GetSpecification().FramesInFlight * m_Count.GetCount(id);
		allocInfo.pSetLayouts = layouts.data();

		descriptorSets[id].resize((size_t)Renderer::GetSpecification().FramesInFlight * m_Count.GetCount(id));
		if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets[id].data()) != VK_SUCCESS)
			LV_LOG_ERROR("Failed to allocate descriptor sets!");

		m_Sets[id].clear();
		ConvertToVulkanDescriptorSets(descriptorSets);
	}

	std::vector<Ref<DescriptorSet>> VulkanDescriptorSetGroup::GetSets(SetID id)
	{
		return m_Sets[id];
	}

	// Note(Jorben): Just for myself, this specifies all the different types that are gonna be in a descriptor set
	void VulkanDescriptorSetGroup::CreateDescriptorSetLayout()
	{
		for (auto& set : m_Layout.GetElements())
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
			layoutInfo.bindingCount = (uint32_t)layouts.size();
			layoutInfo.pBindings = layouts.data();

			m_DescriptorLayouts[set.first] = VK_NULL_HANDLE;
			if (vkCreateDescriptorSetLayout(RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetLogicalDevice()->GetVulkanDevice(), &layoutInfo, nullptr, &m_DescriptorLayouts[set.first]) != VK_SUCCESS)
				LV_LOG_ERROR("Failed to create descriptor set layout!");
		}
	}

	void VulkanDescriptorSetGroup::CreateDescriptorPool()
	{
		auto device = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetLogicalDevice()->GetVulkanDevice();

		for (auto& set : m_Layout.GetElements())
		{
			// Note(Jorben): Just for myself, the poolSizes is just the amount of elements of a certain type to able to allocate per pool
			std::vector<VkDescriptorPoolSize> poolSizes = { };
			poolSizes.resize((size_t)m_Layout.UniqueCount(set.first));
			poolSizes.clear(); // Note(Jorben): For some reason without this line there is a VK_SAMPLER or something in the list.

			for (auto& type : m_Layout.UniqueTypes(set.first))
			{
				VkDescriptorPoolSize poolSize = {};
				poolSize.type = UniformDataTypeToVulkanDescriptorType(type);
				poolSize.descriptorCount = m_Layout.AmountOf(set.first, type) * Renderer::GetSpecification().FramesInFlight;

				poolSizes.push_back(poolSize);
			}

			VkDescriptorPoolCreateInfo poolInfo = {};
			poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			poolInfo.poolSizeCount = (uint32_t)poolSizes.size();
			poolInfo.pPoolSizes = poolSizes.data();
			poolInfo.maxSets = (uint32_t)Renderer::GetSpecification().FramesInFlight * m_Count.GetCount(set.first); // A set for every frame in flight

			m_DescriptorPools[set.first] = VK_NULL_HANDLE;
			if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &m_DescriptorPools[set.first]) != VK_SUCCESS)
				LV_LOG_ERROR("Failed to create descriptor pool!");
		}
	}

	void VulkanDescriptorSetGroup::CreateDescriptorSets()
	{
		auto device = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetLogicalDevice()->GetVulkanDevice();
		
		Dict<SetID, std::vector<VkDescriptorSet>> descriptorSets = { };

		for (auto& set : m_Layout.GetElements())
		{
			std::vector<VkDescriptorSetLayout> layouts(Renderer::GetSpecification().FramesInFlight * m_Count.GetCount(set.first), m_DescriptorLayouts[set.first]);

			VkDescriptorSetAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = m_DescriptorPools[set.first];
			allocInfo.descriptorSetCount = (uint32_t)Renderer::GetSpecification().FramesInFlight * m_Count.GetCount(set.first);
			allocInfo.pSetLayouts = layouts.data();

			descriptorSets[set.first].resize((size_t)Renderer::GetSpecification().FramesInFlight * m_Count.GetCount(set.first));

			VkResult res = vkAllocateDescriptorSets(device, &allocInfo, descriptorSets[set.first].data());
			if (res != VK_SUCCESS)
				LV_LOG_ERROR("Failed to allocate descriptor sets! Error: {0}", VKResultToString(res));
		}

		ConvertToVulkanDescriptorSets(descriptorSets);
	}

	void VulkanDescriptorSetGroup::ConvertToVulkanDescriptorSets(Dict<SetID, std::vector<VkDescriptorSet>>& vulkanSets)
	{
		uint32_t index = 0;

		// 0, 1, 2 - 3, 4, 5
		for (auto& set : vulkanSets)
		{
			uint32_t index = 0;
			for (uint32_t i = 0; i < m_Count.GetCount(set.first); i++)
			{
				std::vector<VkDescriptorSet> sets((size_t)Renderer::GetSpecification().FramesInFlight);

				for (uint32_t j = 0; j < Renderer::GetSpecification().FramesInFlight; j++)
				{
					sets[j] = set.second[index + j];
				}
				
				m_Sets[set.first].push_back(RefHelper::Create<VulkanDescriptorSet>(set.first, sets));
				index += Renderer::GetSpecification().FramesInFlight;
			}
		}
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