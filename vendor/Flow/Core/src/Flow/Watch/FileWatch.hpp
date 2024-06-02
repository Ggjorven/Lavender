#pragma once

#include <filewatch/FileWatch.hpp>

#include <memory>
#include <filesystem>
#include <functional>

namespace Flow::Watcher
{

	enum class EventType : uint8_t
	{
		Added = 0,
		Removed,
		Modified,
		RenamedOld,
		RenamedNew
	};

	typedef std::function<void(const std::filesystem::path&, const EventType)> WatchFunc;

	class Watch
	{
	public:
		Watch(const std::filesystem::path& watchPath, WatchFunc callback);
		virtual ~Watch() = default;

	private:
		filewatch::FileWatch<std::filesystem::path> m_Watcher;
	};

	std::string EventToString(const std::filesystem::path& path, const EventType type);

}