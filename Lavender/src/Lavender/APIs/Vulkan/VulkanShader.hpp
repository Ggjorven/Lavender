#pragma once

#include "Lavender/Renderer/Shader.hpp"

#include <vulkan/vulkan.h>

namespace Lavender
{

	// TODO: ...
	enum class ShaderStage
	{
		None = 0, Vertex, Fragment
	};

	class VulkanShaderCompiler
	{
	public:
		static void Compile(ShaderCode& code);
		static std::vector<char> CompileGLSL(const std::string& source, ShaderStage stage);
	};

	class VulkanShader : public Shader
	{
	public:
		VulkanShader(ShaderCode code);
		virtual ~VulkanShader();

		VkShaderModule& GetVertexShader() { return m_VertexShader; }
		VkShaderModule& GetFragmentShader() { return m_FragmentShader; }

	private:
		VkShaderModule CreateShaderModule(const std::vector<char>& data);

	private:
		VkShaderModule m_VertexShader = VK_NULL_HANDLE;
		VkShaderModule m_FragmentShader = VK_NULL_HANDLE;
	};

}