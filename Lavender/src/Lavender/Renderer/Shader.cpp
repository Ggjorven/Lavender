#include "lvpch.h"
#include "Shader.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/Renderer.hpp"
#include "Lavender/APIs/Vulkan/VulkanManager.hpp"

namespace Lavender
{

	Shader::Shader(const std::vector<char>& vertex, const std::vector<char>& fragment)
	{
		m_VertexShader = CreateShaderModule(vertex);
		m_FragmentShader = CreateShaderModule(fragment);
	}

	Shader::~Shader()
	{
		auto& info = VulkanManager::GetDeviceInfo();

		vkDestroyShaderModule(info.Device, m_FragmentShader, nullptr);
		vkDestroyShaderModule(info.Device, m_VertexShader, nullptr);
	}

	std::vector<char> Shader::ReadSPIRVFile(const std::filesystem::path& filepath)
	{
		std::ifstream file(filepath, std::ios::ate | std::ios::binary);

		if (!file.is_open() || !file.good())
			LV_LOG_ERROR("Failed to open file!");

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();
		return buffer;
	}

	std::string Shader::ReadGLSLFile(const std::filesystem::path& filepath)
	{
		std::ifstream file(filepath);

		if (!file.is_open() || !file.good())
		{
			LV_LOG_ERROR("Failed to open file!");
			return std::string();
		}

		std::string line;
		std::stringstream ss;

		while (std::getline(file, line))
			ss << line << "\n";

		return ss.str();
	}

	VkShaderModule Shader::CreateShaderModule(const std::vector<char>& data)
	{
		auto& info = VulkanManager::GetDeviceInfo();

		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = data.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(data.data());

		VkShaderModule shaderModule = {};
		if (vkCreateShaderModule(info.Device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
			LV_LOG_ERROR("Failed to create shader module!");

		return shaderModule;
	}

}