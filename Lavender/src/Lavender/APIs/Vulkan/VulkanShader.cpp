#include "lvpch.h"
#include "VulkanShader.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/Renderer.hpp"
#include "Lavender/APIs/Vulkan/VulkanContext.hpp"

namespace Lavender
{

	VulkanShader::VulkanShader(ShaderCode code)
	{
		if (!code.VertexSPIRV.empty()) m_VertexShader = CreateShaderModule(code.VertexSPIRV);
		if (!code.FragmentSPIRV.empty()) m_FragmentShader = CreateShaderModule(code.FragmentSPIRV);
	}

	VulkanShader::~VulkanShader()
	{
		auto fragmentShader = m_FragmentShader;
		auto vertexShader = m_VertexShader;

		Renderer::SubmitFree([fragmentShader, vertexShader]() 
		{
			auto device = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetLogicalDevice()->GetVulkanDevice();

			vkDestroyShaderModule(device, fragmentShader, nullptr);
			vkDestroyShaderModule(device, vertexShader, nullptr);
		});
	}

	VkShaderModule VulkanShader::CreateShaderModule(const std::vector<char>& data)
	{
		auto device = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetLogicalDevice()->GetVulkanDevice();

		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = data.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(data.data());

		VkShaderModule shaderModule = {};
		if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
			LV_LOG_ERROR("Failed to create shader module!");

		return shaderModule;
	}

}