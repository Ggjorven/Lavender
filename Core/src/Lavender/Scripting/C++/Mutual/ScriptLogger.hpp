#pragma once

#include <spdlog/spdlog.h>

#include <Lavender/Scripting/C++/Mutual/Functions.hpp>

#include <Insight/Insight.hpp>

namespace Lavender
{

	class ScriptLogger
	{
	public:
		enum class Level : uint8_t
		{
			Trace = 0, Info, Warn, Error, Fatal, None = 255
		};
	public:
		template<typename ...TArgs>
		static void Log(Level level, TArgs&&... args)
		{
			LoggingFunctions::LogMessage((uint8_t)level, fmt::format(std::forward<TArgs>(args)...).c_str());
		}
	};
	
}