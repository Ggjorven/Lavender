#include "lvpch.h"
#include "VulkanShader.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/Renderer.hpp"
#include "Lavender/APIs/Vulkan/VulkanContext.hpp"

#include <shaderc/shaderc.hpp>

namespace Lavender
{

	static shaderc_shader_kind ShaderStageToShaderCType(ShaderStage stage)
	{
		switch (stage)
		{
		case ShaderStage::Vertex:
			return shaderc_glsl_vertex_shader;
		case ShaderStage::Fragment:
			return shaderc_glsl_fragment_shader;
		}

		return shaderc_glsl_vertex_shader;
	}

	void VulkanShaderCompiler::Compile(ShaderCode& code)
	{
        if (code.FragmentSPIRV.empty() && !code.FragmentGLSL.empty())
            code.FragmentSPIRV = CompileGLSL(code.FragmentGLSL, ShaderStage::Fragment);
        else
            LV_LOG_WARN("Fragment shader GLSL or SPIRV is not provided.");

		if (code.VertexSPIRV.empty() && !code.VertexGLSL.empty())
			code.VertexSPIRV = CompileGLSL(code.VertexGLSL, ShaderStage::Vertex);
        else
            LV_LOG_WARN("Vertex shader GLSL or SPIRV is not provided.");
	}

	std::vector<char> VulkanShaderCompiler::CompileGLSL(const std::string& source, ShaderStage stage)
	{
		shaderc::Compiler compiler = {};
		shaderc::CompileOptions options = {};
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);

		shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, ShaderStageToShaderCType(stage), "", options);

		if (module.GetCompilationStatus() != shaderc_compilation_status_success)
			LV_LOG_ERROR("Error compiling shader: {0}", module.GetErrorMessage());

		// Convert SPIR-V code to vector<char>
		const uint32_t* data = module.cbegin();
		const size_t numWords = module.cend() - module.cbegin();
		const size_t sizeInBytes = numWords * sizeof(uint32_t);
		const char* bytes = reinterpret_cast<const char*>(data);

		return std::vector<char>(bytes, bytes + sizeInBytes);
	}



	VulkanShader::VulkanShader(ShaderCode code)
	{
        if (code.VertexSPIRV.empty() || code.FragmentSPIRV.empty())
            VulkanShaderCompiler::Compile(code);

		if (!code.VertexSPIRV.empty()) 
            m_VertexShader = CreateShaderModule(code.VertexSPIRV);
		if (!code.FragmentSPIRV.empty()) 
            m_FragmentShader = CreateShaderModule(code.FragmentSPIRV);
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