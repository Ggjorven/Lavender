#include "GameLayer.hpp"

#include <Lavender/Core/Application.hpp>
#include <Lavender/Utils/Profiler.hpp>

#include <Lavender/FileSystem/ProjectSerializer.hpp>

#include <Lavender/WorkSpace/Project.hpp>
#include <Lavender/WorkSpace/EngineTracker.hpp>

#include <fstream> // TODO: ...

struct LoggingFile
{
public:
	LoggingFile(const std::filesystem::path& path)
		: m_Path(path)
	{
		Load(path);
	}

	void Load(const std::filesystem::path& path)
	{
		m_Path = path;

		// Check if it even exists
		if (std::filesystem::exists(path))
		{
			std::ifstream read = std::ifstream(path);

			if (read.is_open() && read.good())
			{
				std::stringstream stream = {};
				std::string line = {};

				while (std::getline(read, line))
					stream << line << '\n';

				m_Data = stream.str();
			}
		}
	}

	LoggingFile& operator << (const std::string& data)
	{
		Load(m_Path);

		std::ofstream write = std::ofstream(m_Path);
		
		write << m_Data << '\n';
		write << data << '\n';

		write.close();

		return *this;
	}

private:
	std::filesystem::path m_Path = {};
	std::string m_Data = {};
};



void GameLayer::OnAttach()
{
	Track::Lavender::Directory = std::filesystem::path(__argv[0]).parent_path().parent_path().parent_path().parent_path().parent_path().parent_path();

	m_Project = Project::Create({}, WorkSpace::State::Runtime);

	// Load a project from .lvproj file // TODO: Change this to a runtime format
	if (__argc == 2)
	{
		std::filesystem::path projectPath = __argv[1];

		if (projectPath.extension() != ".lvproj")
		{
			//APP_ASSERT(false, "File loaded is not a Lavender Project!");
			APP_LOG_ERROR("File loaded is not a Lavender Project!");
		}
		else
		{
			ProjectSerializer serializer(m_Project);
			serializer.Deserialize(Track::Lavender::Directory / projectPath);
		}
	}
	else
	{
		//APP_ASSERT(false, "No path given to load.");
		APP_LOG_ERROR("No path given to load");
	}

	Application::Get().GetWindow().SetTitle(fmt::format("{0} - {1}", m_Project->GetInfo().Name, Track::Lavender::VersionTitle));
}

void GameLayer::OnDetach()
{
}

void GameLayer::OnUpdate(float deltaTime)
{
	m_Project->OnUpdate(deltaTime);
}

void GameLayer::OnRender()
{
	m_Project->OnRender();
}

void GameLayer::OnEvent(Event& e)
{
	m_Project->OnEvent(e);
}