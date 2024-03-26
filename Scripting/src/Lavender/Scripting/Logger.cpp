#include "Logger.hpp"

namespace Lavender
{

	void ScriptLogger::Init(std::shared_ptr<spdlog::logger>* logger)
	{
		spdlog::set_default_logger(*logger);
		spdlog::set_level(spdlog::level::trace);
	}

}