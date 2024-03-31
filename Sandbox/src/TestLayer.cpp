#include "TestLayer.hpp"

#include <Lavender/Core/Application.hpp>
#include <Lavender/Core/Logging.hpp>
#include <Lavender/Core/Input/Input.hpp>

#include <Lavender/Renderer/Renderer.hpp>

#include <Lavender/APIs/Vulkan/VulkanUtils.hpp>
#include <Lavender/APIs/Vulkan/VulkanRenderer.hpp>
#include <Lavender/APIs/Vulkan/VulkanContext.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void TestLayer::OnAttach()
{
	static uint32_t sets = 2;

	// Note(Jorben): Simple test case for vulkan descriptor sets
	auto device = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetLogicalDevice()->GetVulkanDevice();
	VkResult result = VK_SUCCESS;

	///////////////////////////////////////////////////////////////////////////////////////////////
	/// Layout
	///////////////////////////////////////////////////////////////////////////////////////////////
	VkDescriptorSetLayoutBinding layoutBinding = {};
	layoutBinding.binding = 0; 
	layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	layoutBinding.descriptorCount = 1;
	layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT; 
	layoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &layoutBinding;

	VkDescriptorSetLayout descriptorSetLayout;
	result = vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout);
	if (result != VK_SUCCESS) 
		LV_LOG_ERROR("Failed to create descriptor set layout! Error: {0}", VKResultToString(result));

	///////////////////////////////////////////////////////////////////////////////////////////////
	/// Descriptor pool
	///////////////////////////////////////////////////////////////////////////////////////////////
	VkDescriptorPoolSize poolSize = {};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = sets * Renderer::GetSpecification().FramesInFlight;

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = sets * Renderer::GetSpecification().FramesInFlight;

	VkDescriptorPool descriptorPool;
	result = vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool);
	if (result != VK_SUCCESS)
		LV_LOG_ERROR("Failed to create descriptor pool! Error: {0}", VKResultToString(result));

	///////////////////////////////////////////////////////////////////////////////////////////////
	/// Allocate sets
	///////////////////////////////////////////////////////////////////////////////////////////////
	std::vector<VkDescriptorSetLayout> layouts((size_t)(sets * Renderer::GetSpecification().FramesInFlight), descriptorSetLayout);

	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = sets * Renderer::GetSpecification().FramesInFlight;
	allocInfo.pSetLayouts = layouts.data();

	std::vector<VkDescriptorSet> descriptorSets((size_t)(sets * Renderer::GetSpecification().FramesInFlight));
	result = vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data());
	if (result != VK_SUCCESS)
		LV_LOG_ERROR("Failed to allocate descriptor sets! Error: {0}", VKResultToString(result));



	///////////////////////////////////////////////////////////////////////////////////////////////
	/// Cleanup
	///////////////////////////////////////////////////////////////////////////////////////////////
	vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
	vkDestroyDescriptorPool(device, descriptorPool, nullptr);
}

void TestLayer::OnDetach()
{
}

void TestLayer::OnUpdate(float deltaTime)
{
}

void TestLayer::OnRender()
{
}

void TestLayer::OnImGuiRender()
{
}

void TestLayer::OnEvent(Event& e)
{
	EventHandler handler(e);
}