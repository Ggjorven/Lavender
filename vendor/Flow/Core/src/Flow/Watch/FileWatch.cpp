#include "flpch.h"
#include "FileWatch.hpp"

namespace Flow::Watcher
{

	Watch::Watch(const std::filesystem::path& watchPath, WatchFunc callback)
		: m_Watcher(filewatch::FileWatch<std::filesystem::path>((watchPath.empty() ? std::filesystem::current_path() : watchPath),
		[this, callback](const std::filesystem::path& file, filewatch::Event event) 
		{
			callback(file, (EventType)event);
		}))
	{
	}

	std::string EventToString(const std::filesystem::path& path, const EventType type)
	{
		std::string result = {};

		switch (type)
		{
		case EventType::Added:
			result = std::format("File added: {0}", path.string());
			break;
		case EventType::Removed:
			result = std::format("File removed: {0}", path.string());
			break;
		case EventType::Modified:
			result = std::format("File modified: {0}", path.string());
			break;
		case EventType::RenamedOld:
			result = std::format("File renamed, old name: {0}", path.string());
			break;
		case EventType::RenamedNew:
			result = std::format("File renamed, new name: {0}", path.string());
			break;

		default:
			// TODO: Add logging
			break;
		}

		return result;
	}

}