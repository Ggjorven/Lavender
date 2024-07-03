#include "RuntimeLayer.hpp"

#include <Lavender/Core/Application.hpp>
#include <Lavender/Utils/Profiler.hpp>

#include <Lavender/FileSystem/ProjectSerializer.hpp>

#include <Lavender/WorkSpace/Project.hpp>
#include <Lavender/WorkSpace/EngineTracker.hpp>

void RuntimeLayer::OnAttach()
{
	m_Project = Project::Create({}, WorkSpace::State::Runtime);

	// Load a project from .lvproj file // TODO: Change this to a runtime format
	if (__argc == 2)
	{
		std::filesystem::path projectPath = __argv[1];

		if (projectPath.extension() != ".lvproj")
		{
			APP_LOG_ERROR("File loaded is not a Lavender Project!");
		}
		else
		{
			ProjectSerializer serializer(m_Project);
			serializer.Deserialize(projectPath);
		}
	}
	else
	{
		ProjectSerializer serializer(m_Project);
		serializer.Deserialize(Track::Lavender::Directory / "Editor/Resources/Projects/Example/Example.lvproj");
	}

	Application::Get().GetWindow().SetTitle(fmt::format("{0} - {1}", m_Project->GetInfo().Name, Track::Lavender::VersionTitle));
}

void RuntimeLayer::OnDetach()
{
}

void RuntimeLayer::OnUpdate(float deltaTime)
{
	APP_PROFILE_SCOPE("Runtime::Update");

	//Application::Get().GetWindow().SetTitle(fmt::format("{0} - {1} | FPS: {2} | Frametime: {3:.3f}ms", m_Project->GetInfo().Name, Track::Lavender::VersionTitle, Track::Frame::FPS, Track::Frame::FrameTime)); // TODO: Remove takes 300microsecs

	m_Project->OnUpdate(deltaTime);
}

void RuntimeLayer::OnRender()
{
	APP_PROFILE_SCOPE("Runtime::Render");
	m_Project->OnRender();
}

void RuntimeLayer::OnEvent(Event& e)
{
	m_Project->OnEvent(e);
}