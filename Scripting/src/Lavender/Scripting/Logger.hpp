#pragma once

#include "Lavender/Scripting/Core.hpp"

#include <memory>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Lavender
{

	// Note(Jorben): This is a base class, as a user you can always define your own using this.
	class ScriptLogger
	{
	public:
		enum class Level : uint8_t
		{
			Trace = 0, Info, Warn, Error, Fatal, None = 255
		};

		static void Init(std::shared_ptr<spdlog::logger>* logger);

		template<typename ... Args>
		static void LogMessage(ScriptLogger::Level level, Args&&... args);
	};

	template<typename ... Args>
	void ScriptLogger::LogMessage(ScriptLogger::Level level, Args&&... args)
	{
		switch (level)
		{
		case Level::Trace:
			spdlog::trace(fmt::format(std::forward<Args>(args)...));
			break;
		case Level::Info:
			spdlog::info(fmt::format(std::forward<Args>(args)...));
			break;
		case Level::Warn:
			spdlog::warn(fmt::format(std::forward<Args>(args)...));
			break;
		case Level::Error:
			spdlog::error(fmt::format(std::forward<Args>(args)...));
			break;
		case Level::Fatal:
			spdlog::critical(fmt::format(std::forward<Args>(args)...));
			break;
		}
	}

}