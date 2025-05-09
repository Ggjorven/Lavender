#pragma once

#include <optional>
#include <filesystem>

#include "Lavender/Utils/Utils.hpp"

namespace Lavender
{

	// TODO: Add GLSL support
	struct ShaderCode
	{
	public:
		std::vector<char> FragmentSPIRV = {};
		std::vector<char> VertexSPIRV = {};
		std::string FragmentGLSL = {};
		std::string VertexGLSL = {};

	public:
		ShaderCode() = default;
		ShaderCode(const std::vector<char>& fragment, const std::vector<char>& vertex);
	};

	class Shader
	{
	public:
		Shader() = default;
		virtual ~Shader() = default;

		static std::string ReadGLSLFile(const std::filesystem::path& path);
		static std::vector<char> ReadSPIRVFile(const std::filesystem::path& path);

		static Ref<Shader> Create(ShaderCode code);
	};

}