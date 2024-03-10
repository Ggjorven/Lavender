#include "lvpch.h"
#include "Shader.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/Renderer.hpp"

#include "Lavender/APIs/Vulkan/VulkanShader.hpp"

namespace Lavender
{

	ShaderCode::ShaderCode(const std::vector<char>& fragment, const std::vector<char>& vertex)
		: FragmentSPIRV(fragment), VertexSPIRV(vertex)
	{
	}

	std::vector<char> Shader::ReadSPIRVFile(const std::filesystem::path& path)
	{
		std::ifstream file(path, std::ios::ate | std::ios::binary);

		if (!file.is_open() || !file.good())
			LV_LOG_ERROR("Failed to open file!");

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();
		return buffer;
	}

	Ref<Shader> Shader::Create(ShaderCode code)
	{
		switch (Renderer::GetAPI())
		{
		case RenderingAPI::Vulkan:
			return RefHelper::Create<VulkanShader>(code);

		default:
			LV_LOG_ERROR("Invalid API selected.");
			break;
		}

		return nullptr;
	}

}
