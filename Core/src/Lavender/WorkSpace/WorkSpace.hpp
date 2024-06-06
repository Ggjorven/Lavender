#pragma once

#include "Lavender/Core/Core.hpp"
#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Utils/UUID.hpp"

#include <filesystem>

namespace Lavender
{
	class Project;
	class Scene;
}

namespace Lavender::WorkSpace
{

	// Overall
	enum class State : uint8_t
	{
		None = 0, Editor, Runtime
	};

	enum class Configuration : uint8_t
	{
		None = 0, Debug, Release, Dist
	};
	std::string ConfigToString(Configuration config);

	// Scene
	struct SceneInfo
	{
	public:
		std::filesystem::path Path = {};
	};

	// Project
	struct ProjectInfo
	{
	public:
		std::string Name = {};
		UUID StartScene = UUID::Empty;

		std::filesystem::path Path = {};
		std::filesystem::path Directory = {};

		std::filesystem::path Assets = {};
		std::filesystem::path Script = {};
		std::filesystem::path Scenes = {};

		std::filesystem::path ScriptsPath = {};
	};

}