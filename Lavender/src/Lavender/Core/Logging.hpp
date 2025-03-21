#pragma once

#include <memory>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Lavender
{

	class Log
	{
	public:
		enum class Level : uint8_t
		{
			Trace = 0, Info, Warn, Error, Fatal, None = 255
		};

		static void Init();

		template<typename ... Args>
		static void LogMessage(Log::Level level, Args&&... args);

		#ifndef LV_DIST
		#define LV_LOG_TRACE(...)	Lavender::Log::LogMessage(Lavender::Log::Level::Trace, __VA_ARGS__);
		#define LV_LOG_INFO(...)	Lavender::Log::LogMessage(Lavender::Log::Level::Info, __VA_ARGS__);
		#define LV_LOG_WARN(...)	Lavender::Log::LogMessage(Lavender::Log::Level::Warn, __VA_ARGS__);
		#define LV_LOG_ERROR(...)	Lavender::Log::LogMessage(Lavender::Log::Level::Error, __VA_ARGS__);
		#define LV_LOG_FATAL(...)	Lavender::Log::LogMessage(Lavender::Log::Level::Fatal, __VA_ARGS__);
		#else
		#define LV_LOG_TRACE(...)
		#define LV_LOG_INFO(...)
		#define LV_LOG_WARN(...)
		#define LV_LOG_ERROR(...)
		#define LV_LOG_FATAL(...)
		#endif

	private:
		static std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> s_Sink;
		static std::shared_ptr<spdlog::logger> s_Logger;
	};

	template<typename ... Args>
	void Log::LogMessage(Log::Level level, Args&&... args)
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