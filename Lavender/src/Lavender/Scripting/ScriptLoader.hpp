#pragma once

#include <filesystem>

#include "Lavender/Utils/Utils.hpp"

namespace Lavender
{

	class ScriptLoader
	{
	public:
		ScriptLoader() = default;
		virtual ~ScriptLoader() = default;

		static Ref<ScriptLoader> Create(const std::filesystem::path& filepath);
	};

}