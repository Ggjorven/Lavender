#pragma once

#include <memory>
#include <vector>
#include <string>
#include <filesystem>

#include <vulkan/vulkan.h>

namespace Lavender
{

	class Shader
	{
	public:
		Shader(const std::vector<char>& vertex, const std::vector<char>& fragment);
		//Shader(const std::string& vertex, const std::string& fragment);
		virtual ~Shader();

		VkShaderModule& GetVertexShader() { return m_VertexShader; }
		VkShaderModule& GetFragmentShader() { return m_FragmentShader; }

		static std::vector<char> ReadSPIRVFile(const std::filesystem::path& filepath);
		static std::string ReadGLSLFile(const std::filesystem::path& filepath);

	private:
		VkShaderModule CreateShaderModule(const std::vector<char>& data);

	private:
		VkShaderModule m_VertexShader = VK_NULL_HANDLE;
		VkShaderModule m_FragmentShader = VK_NULL_HANDLE;

	};

}