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
		std::optional<std::vector<char>> FragmentSPIRV = {};
		std::optional<std::vector<char>> VertexSPIRV = {};

	public:
		ShaderCode(const std::vector<char>& fragment, const std::vector<char>& vertex);
	};

	class Shader
	{
	public:
		Shader() = default;
		virtual ~Shader() = default;

		static std::vector<char> ReadSPIRVFile(const std::filesystem::path& path);

		static Ref<Shader> Create(ShaderCode code);
	};

}