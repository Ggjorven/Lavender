#pragma once

#include <filesystem>

namespace Lavender
{

	struct ProjectDirectories
	{
	public:
		std::filesystem::path Project = "";
		std::filesystem::path ProjectDir = "";
		std::filesystem::path Assets = "Assets";
		std::filesystem::path Scripts = "Script";

	public:
		ProjectDirectories() = default;
		ProjectDirectories(const ProjectDirectories& other) = default;
		virtual ~ProjectDirectories() = default;
	};

	enum class ProjectState
	{
		None = 0, Editor, Runtime
	};

}