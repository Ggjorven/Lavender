#include "EditorLayer.hpp"

#include <fstream>

#include <Lavender/Core/Application.hpp>
#include <Lavender/Core/Input/Input.hpp>
#include <Lavender/Utils/Profiler.hpp>

#include <Lavender/Renderer/Shader.hpp>

#include <Lavender/FileSystem/ProjectSerializer.hpp>
#include <Lavender/FileSystem/Serialization.hpp>

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
	Application::Get().GetWindow().SetTitle(Track::Lavender::VersionTitle);

	Application::Get().GetWindow().SetDragDropCallBack([](const std::vector<std::filesystem::path>& paths)
	{
		for (auto& path : paths)
			APP_LOG_TRACE("Dropped file: '{0}'", path.string());
	});
	
	m_Project = Project::Create({}, WorkSpace::State::Editor);

	LoadProject();
	InitStyles();
	InitUI();

	// Get Editor Preferences
	UI::PreferencesSerializer serializer(Track::Lavender::Directory / "Editor/Preferences.lvepref");
	serializer.Deserialize();
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

	m_Project->OnUpdate(deltaTime);
}

void EditorLayer::OnRender()
{
	APP_PROFILE_SCOPE("Editor::Render");

	m_Project->OnRender();
}

void EditorLayer::OnEvent(Event& e)
{
	EventHandler handler(e);

	handler.Handle<KeyPressedEvent>(APP_BIND_EVENT_FN(EditorLayer::OnKeyPress));

	m_Project->OnEvent(e);
}

void EditorLayer::OnUIRender()
{
	APP_PROFILE_SCOPE("Editor::UI");

	ImGui::DockSpaceOverViewport();

	//m_Editor.RenderWindow();
	//ImGui::ShowStyleEditor();

	m_GlobalUIStyles.Push();
	m_GlobalUIColours.Push();

	MenuBar();

	m_Entities->RenderUI();
	m_Components->RenderUI();
	m_Debug->RenderUI();
	m_Viewport->RenderUI();

	m_GlobalUIStyles.Pop();
	m_GlobalUIColours.Pop();
}

bool EditorLayer::OnKeyPress(KeyPressedEvent& e)
{
	// Reload
	if (e.GetKeyCode() == Key::F5)
	{
		if (m_Project->GetState() == WorkSpace::State::Runtime)
		{
			APP_LOG_WARN("Tried to reload project while it is running.");
			return false;
		}

		std::filesystem::path path = m_Project->GetInfo().Path;

		m_Project = Project::Create({}, WorkSpace::State::Editor);

		ProjectSerializer serializer(m_Project);
		serializer.Deserialize(path);
	}
	// Save
	else if (e.GetKeyCode() == Key::S && Input::IsKeyPressed(Key::LeftControl))
	{
		if (m_Project->GetState() == WorkSpace::State::Runtime)
		{
			APP_LOG_WARN("Tried to save project while it is running.");
			return false;
		}

		ProjectSerializer serializer(m_Project);
		serializer.Serialize();
	}

	return false;
}

/////////////////////////////////////////////////////////////////
// Custom Functionality
/////////////////////////////////////////////////////////////////
void EditorLayer::LoadProject(const std::filesystem::path& file)
{
	// Load a project from .lvproj file
	if (__argc >= 2)
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
		serializer.Deserialize(Track::Lavender::Directory / file);
	}
}

void EditorLayer::InitStyles()
{
	m_GlobalUIStyles = UI::StyleList({
		{ UI::StyleType::WindowPadding, { 8.0f, 8.0f } },
		{ UI::StyleType::FramePadding, { 4.0f, 3.0f } },
		{ UI::StyleType::CellPadding, { 2.0f, 2.0f } },
		{ UI::StyleType::ItemSpacing, { 8.0f, 4.0f } },
		{ UI::StyleType::ItemInnerSpacing, { 4.0f, 4.0f } },
		//{ UI::StyleType::TouchExtraPadding, { 0.0f, 0.0f } },
		{ UI::StyleType::IndentSpacing, 21.0f },
		{ UI::StyleType::ScrollbarSize, 14.0f },
		{ UI::StyleType::GrabMinSize, 10.0f },

		{ UI::StyleType::WindowBorderSize, 1.0f },
		{ UI::StyleType::ChildBorderSize, 1.0f },
		{ UI::StyleType::PopupBorderSize, 1.0f },
		{ UI::StyleType::FrameBorderSize, 0.0f },
		//{ UI::StyleType::TabBorderSize, 0.0f },

		{ UI::StyleType::WindowRounding, 0.0f },
		{ UI::StyleType::ChildRounding, 0.0f },
		{ UI::StyleType::FrameRounding, 0.0f },
		{ UI::StyleType::PopupRounding, 0.0f },
		{ UI::StyleType::ScrollbarRounding, 9.0f },
		{ UI::StyleType::GrabRounding, 0.0f },
		//{ UI::StyleType::LogSliderDeadzone, 4.0f },
		{ UI::StyleType::TabRounding, 4.0f },

		{ UI::StyleType::WindowTitleAlign, { 0.50f, 0.50f } },
		// TODO: Alignment
		{ UI::StyleType::ButtonTextAlign, { 0.50f, 0.50f } },
		{ UI::StyleType::SelectableTextAlign, { 0.0f, 0.0f } },

		//{ UI::StyleType::DisplaySafeAreaPadding, { 3.0f, 3.0f } },
	});
	
	m_GlobalUIColours = UI::StyleList({
		{ UI::StyleColourType::Text, { 1.00f, 1.00f, 1.00f, 1.00f } },
		{ UI::StyleColourType::TextDisabled, { 0.50f, 0.50f, 0.50f, 1.00f } },
		{ UI::StyleColourType::TextSelectedBg, { 0.26f, 0.59f, 0.98f, 0.35f } },

		{ UI::StyleColourType::WindowBg, { 0.06f, 0.06f, 0.06f, 1.00f } },
		{ UI::StyleColourType::ChildBg, { 0.00f, 0.00f, 0.00f, 0.00f } },
		{ UI::StyleColourType::PopupBg, { 0.08f, 0.08f, 0.08f, 0.94f } },

		{ UI::StyleColourType::Border, { 0.43f, 0.43f, 0.50f, 0.50f } },
		{ UI::StyleColourType::BorderShadow, { 0.00f, 0.00f, 0.00f, 0.00f } },

		{ UI::StyleColourType::FrameBg, { 0.16f, 0.29f, 0.48f, 0.54f } },
		{ UI::StyleColourType::FrameBgHovered, { 0.26f, 0.59f, 0.98f, 0.40f } },
		{ UI::StyleColourType::FrameBgActive, { 0.26f, 0.59f, 0.98f, 0.67f } },

		{ UI::StyleColourType::TitleBg, { 0.04f, 0.04f, 0.04f, 1.00f } },
		{ UI::StyleColourType::TitleBgActive, { 0.16f, 0.29f, 0.48f, 1.00f } },
		{ UI::StyleColourType::TitleBgCollapsed, { 0.00f, 0.00f, 0.00f, 0.51f } },

		{ UI::StyleColourType::MenuBarBg, { 0.14f, 0.14f, 0.14f, 1.00f } },

		{ UI::StyleColourType::ScrollbarBg, { 0.02f, 0.02f, 0.02f, 0.53f } },
		{ UI::StyleColourType::ScrollbarGrab, { 0.31f, 0.31f, 0.31f, 1.00f } },
		{ UI::StyleColourType::ScrollbarGrabHovered, { 0.41f, 0.41f, 0.41f, 1.00f } },
		{ UI::StyleColourType::ScrollbarGrabActive, { 0.51f, 0.51f, 0.51f, 1.00f } },

		{ UI::StyleColourType::CheckMark, { 0.26f, 0.59f, 0.98f, 1.00f } },

		{ UI::StyleColourType::SliderGrab, { 0.24f, 0.52f, 0.88f, 1.00f } },
		{ UI::StyleColourType::SliderGrabActive, { 0.26f, 0.59f, 0.98f, 1.00f } },

		{ UI::StyleColourType::Button, { 0.26f, 0.59f, 0.98f, 0.40f } },
		{ UI::StyleColourType::ButtonHovered, { 0.26f, 0.59f, 0.98f, 1.00f } },
		{ UI::StyleColourType::ButtonActive, { 0.06f, 0.53f, 0.98f, 1.00f } },

		{ UI::StyleColourType::Header, { 0.26f, 0.59f, 0.98f, 0.31f } },
		{ UI::StyleColourType::HeaderHovered, { 0.26f, 0.59f, 0.98f, 0.80f } },
		{ UI::StyleColourType::HeaderActive, { 0.26f, 0.59f, 0.98f, 1.00f } },

		{ UI::StyleColourType::Separator, { 0.43f, 0.43f, 0.50f, 0.50f } },
		{ UI::StyleColourType::SeparatorHovered, { 0.10f, 0.40f, 0.75f, 0.78f } },
		{ UI::StyleColourType::SeparatorActive, { 0.10f, 0.40f, 0.75f, 1.00f } },

		{ UI::StyleColourType::ResizeGrip, { 0.26f, 0.59f, 0.98f, 0.20f } },
		{ UI::StyleColourType::ResizeGripHovered, { 0.26f, 0.59f, 0.98f, 0.67f } },
		{ UI::StyleColourType::ResizeGripActive, { 0.26f, 0.59f, 0.98f, 0.95f } },

		{ UI::StyleColourType::Tab, { 0.18f, 0.35f, 0.58f, 0.86f } },
		{ UI::StyleColourType::TabHovered, { 0.26f, 0.59f, 0.98f, 0.80f } },
		{ UI::StyleColourType::TabActive, { 0.20f, 0.41f, 0.68f, 1.00f } },
		{ UI::StyleColourType::TabUnfocused, { 0.07f, 0.10f, 0.15f, 0.97f } },
		{ UI::StyleColourType::TabUnfocusedActive, { 0.14f, 0.26f, 0.42f, 1.00f } },

		{ UI::StyleColourType::DockingPreview, { 0.26f, 0.59f, 0.98f, 0.70f } },
		{ UI::StyleColourType::DockingEmptyBg, { 0.20f, 0.20f, 0.20f, 1.00f } },

		{ UI::StyleColourType::PlotLines, { 0.61f, 0.61f, 0.61f, 1.00f } },
		{ UI::StyleColourType::PlotLinesHovered, { 1.00f, 0.43f, 0.35f, 1.00f } },
		{ UI::StyleColourType::PlotHistogram, { 0.90f, 0.70f, 0.00f, 1.00f } },
		{ UI::StyleColourType::PlotHistogramHovered, { 1.00f, 0.60f, 0.00f, 1.00f } },

		{ UI::StyleColourType::TableHeaderBg, { 0.19f, 0.19f, 0.20f, 1.00f } },
		{ UI::StyleColourType::TableBorderStrong, { 0.31f, 0.31f, 0.35f, 1.00f } },
		{ UI::StyleColourType::TableBorderLight, { 0.23f, 0.23f, 0.25f, 1.00f } },
		{ UI::StyleColourType::TableRowBg, { 0.00f, 0.00f, 0.00f, 0.00f } },
		{ UI::StyleColourType::TableRowBgAlt, { 1.00f, 1.00f, 1.00f, 0.06f } },

		{ UI::StyleColourType::DragDropTarget, { 1.00f, 1.00f, 0.00f, 0.90f } },

		{ UI::StyleColourType::NavHighlight, { 0.26f, 0.59f, 0.98f, 1.00f } },
		{ UI::StyleColourType::NavWindowingHighlight, { 1.00f, 1.00f, 0.00f, 0.90f } },
		{ UI::StyleColourType::NavWindowingDimBg, { 0.80f, 0.80f, 0.80f, 0.20f } },

		{ UI::StyleColourType::ModalWindowDimBg, { 0.80f, 0.80f, 0.80f, 0.35f } }
	});
}

void EditorLayer::InitUI()
{
	m_Entities = UI::Entities::Create();
	m_Components = UI::Components::Create(m_Entities);
	m_Debug = UI::Debug::Create();
	m_Viewport = UI::Viewport::Create(m_Entities);
}

void EditorLayer::MenuBar()
{
	ImGui::BeginMainMenuBar();

	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("New project", "Ctrl+N"))
		{
			// TODO: ...
		}

		if (ImGui::MenuItem("Open project", "Ctrl+O"))
		{
			if (m_Project->GetState() == WorkSpace::State::Runtime)
			{
				APP_LOG_WARN("Tried to open a project while another project is running.");
			}
			else
			{
				auto file = Utils::ToolKit::OpenFile(".lvproj\0*.lvproj\0All Files\0*.*\0", m_Project->GetInfo().Directory);

				if (!file.empty())
				{
					// Save old project
					{
						ProjectSerializer serializer(m_Project);
						serializer.Serialize();
					}

					m_Project = Project::Create({}, WorkSpace::State::Editor);

					LoadProject(file);
				}
			}
		}

		if (ImGui::MenuItem("Save project", "Ctrl+S"))
		{
			if (m_Project->GetState() == WorkSpace::State::Runtime)
			{
				APP_LOG_WARN("Tried to save a project while it is running.");
			}
			else
			{
				ProjectSerializer serializer(m_Project);
				serializer.Serialize();
			}
		}

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Edit"))
	{
		// TODO: ...

		ImGui::EndMenu();
	}

	ImGui::EndMainMenuBar();
}

// TODO: Update + clean
void EditorLayer::CreateProject(const std::string& name, const std::filesystem::path& directory)
{
	// TODO: ...
}