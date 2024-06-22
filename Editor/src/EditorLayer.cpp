#include "EditorLayer.hpp"

#include <fstream>

#include <Lavender/Core/Application.hpp>
#include <Lavender/Core/Input/Input.hpp>
#include <Lavender/Utils/Profiler.hpp>

#include <Lavender/Renderer/Shader.hpp>

#include <Lavender/FileSystem/ProjectSerializer.hpp>

#include <Lavender/WorkSpace/EngineTracker.hpp>

#include <imgui.h>

#include <Lavender/ECS/Entity.hpp>
#include <Lavender/ECS/Components.hpp>

#include "EditorPreferences.hpp"

/////////////////////////////////////////////////////////////////
// Base Functions
/////////////////////////////////////////////////////////////////
void EditorLayer::OnAttach()
{
	m_Project = Project::Create({}, WorkSpace::State::Editor);

	// Load a project from .lvproj file
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

	// Get Editor Preferences
	UI::PreferencesSerializer serializer(Track::Lavender::Directory / "Editor/Preferences.lvepref");
	serializer.Deserialize();

	Application::Get().GetWindow().SetTitle(Track::Lavender::VersionTitle);
}

void EditorLayer::OnDetach()
{
	// Save current project.
	ProjectSerializer projectSerializer(m_Project);
	projectSerializer.Serialize();

	// Save preferences.
	UI::PreferencesSerializer preferencesSerializer(Track::Lavender::Directory / "Editor/Preferences.lvepref");
	preferencesSerializer.Serialize();
}

void EditorLayer::OnUpdate(float deltaTime)
{
	APP_PROFILE_SCOPE("Editor::Update");
	//Application::Get().GetWindow().SetTitle(fmt::format("Editor | FPS: {0} | Frametime: {1:.3f}ms", Track::Frame::FPS, Track::Frame::FrameTime)); // TODO: Remove takes 300microsecs

	m_Project->OnUpdate(deltaTime);
}

void EditorLayer::OnRender()
{
	APP_PROFILE_SCOPE("Editor::Render");

	m_Project->OnRender();
}

void EditorLayer::OnEvent(Event& e)
{
	m_Project->OnEvent(e);
}

void EditorLayer::OnUIRender()
{
	APP_PROFILE_SCOPE("Editor::UI");

	ImGui::DockSpaceOverViewport();

	m_Viewport.RenderUI();
}

/////////////////////////////////////////////////////////////////
// Custom Functionality
/////////////////////////////////////////////////////////////////
// TODO: Update + clean
void EditorLayer::CreateProject(const std::string& name, const std::filesystem::path& directory)
{
	// TODO: ...
}