#include "lvpch.h"
#include "WorkSpace.hpp"

#include "Lavender/Core/Logging.hpp"

namespace Lavender::WorkSpace
{

	// Overall
	std::string ConfigToString(Configuration config)
	{
		switch (config)
		{
		case Configuration::Debug: return "Debug";
		case Configuration::Release: return "Release";
		case Configuration::Dist: return "Dist";

		default:
			APP_LOG_ERROR("Invalid Configuration passed in.");
			break;
		}

		return {};
	}

}