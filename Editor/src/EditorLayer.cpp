#include "EditorLayer.hpp"

#include <Lavender/Core/Application.hpp>
#include <Lavender/Core/Logging.hpp>
#include <Lavender/Core/Input/Input.hpp>

#include <Lavender/Utils/Utils.hpp>
#include <Lavender/Utils/Profiler.hpp>

#include <Lavender/APIs/Vulkan/VulkanContext.hpp>
#include <Lavender/APIs/Vulkan/VulkanImage.hpp>
#include <Lavender/APIs/Vulkan/VulkanImGuiLayer.hpp>

#include <Lavender/Renderer/Renderer.hpp>
#include <Lavender/Renderer/Shader.hpp>

#include <Lavender/UI/UI.hpp>
#include <Lavender/UI/Style.hpp>
#include <Lavender/UI/Colours.hpp>

#include <Lavender/Workspace/Assets/MeshAsset.hpp>

#include <Lavender/FileSystem/PreferencesSerializer.hpp>
#include <Lavender/FileSystem/ProjectSerializer.hpp>
#include <Lavender/FileSystem/SceneSerializer.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void EditorLayer::OnAttach()
{
	m_Preferences = RefHelper::Create<UIPreferences>();
	PreferencesSerializer serializer(m_Preferences);
	serializer.Deserialize(Utils::ToolKit::GetEnv() + "/Editor/EditorPreferences.lvepref");

	m_Project = Project::Create();
	ProjectSerializer projSerializer(m_Project);
	projSerializer.Deserialize(Utils::ToolKit::GetEnv() + "/Editor/Projects/First/Project.lvproject");

	m_ContentBrowserPanel = ContentBrowserPanel::Create(m_Project);
	m_EntityPanel = EntitiesPanel::Create(m_Project);
	m_MaterialPanel = MaterialPanel::Create(m_Project); m_MaterialPanel->SetEnabled(true); // TODO: Remove
	m_DebugPanel = DebugPanel::Create(m_Project);

	Application::SetInitialized(true);
}

void EditorLayer::OnDetach()
{
	ProjectSerializer projSerializer(m_Project);
	projSerializer.Serialize();

	SceneSerializer sceneSerializer(m_Project->GetSceneCollection().GetActive());
	sceneSerializer.Serialize();

	PreferencesSerializer prefSerializer(m_Preferences);
	prefSerializer.Serialize("EditorPreferences.lvepref");
}

void EditorLayer::OnUpdate(float deltaTime)
{
	LV_PROFILE_SCOPE("EditorLayer::OnUpdate");

	m_Project->OnUpdate(deltaTime);

	m_ContentBrowserPanel->OnUpdate(deltaTime);
	m_DebugPanel->OnUpdate(deltaTime);
}

void EditorLayer::OnRender()
{
	LV_PROFILE_SCOPE("EditorLayer::OnRender");

	m_Project->OnRender();
}

void EditorLayer::OnImGuiRender()
{
	LV_PROFILE_SCOPE("EditorLayer::OnImGuiRender");

	// Setting colours
	auto& colours = ImGui::GetStyle().Colors;
	auto colour = UI::Colours::ConvertU32Colour(UI::Colours::LighterTint);
	colours[ImGuiCol_SeparatorActive] = ImVec4(colour.r, colour.g, colour.b, colour.a);
	colours[ImGuiCol_SeparatorHovered] = ImVec4(colour.r, colour.g, colour.b, colour.a);

	// Rendering
	ImGui::DockSpaceOverViewport();
	RenderMenuBar();
	//ImGui::ShowDemoWindow(); // TODO: Remove
	//ImGui::ShowStyleEditor(); // TODO: Remove
	
	m_Project->OnImGuiRender();

	//m_ContentBrowserPanel->RenderUI();
	m_EntityPanel->RenderUI();
	m_MaterialPanel->RenderUI();
	m_DebugPanel->RenderUI();
}

void EditorLayer::OnEvent(Event& e)
{
	EventHandler handler(e);

	m_Project->OnEvent(e);

	handler.Handle<KeyPressedEvent>(LV_BIND_EVENT_FN(EditorLayer::OnKeyPressEvent));
	handler.Handle<WindowResizeEvent>(LV_BIND_EVENT_FN(EditorLayer::OnResizeEvent));
}

bool EditorLayer::OnKeyPressEvent(KeyPressedEvent& e)
{
	if (e.GetKeyCode() == Key::F5)
	{
		m_Project->GetSceneCollection().GetActive()->ReloadScript();
	}

	return false;
}

bool EditorLayer::OnResizeEvent(WindowResizeEvent& e)
{
	m_Project->GetViewport()->Resize(e.GetWidth(), e.GetHeight());

	return false;
}

void EditorLayer::RenderMenuBar()
{	
	UI::ScopedStyleList colours = UI::StyleColourList({
		{ UI::StyleColourType::WindowBg, UI::Colours::BackgroundDark },
		{ UI::StyleColourType::Header, UI::Colours::BackgroundPopup },
		{ UI::StyleColourType::HeaderHovered, UI::Colours::LightTint },
		{ UI::StyleColourType::HeaderActive, UI::Colours::LighterTint }
	});

	if (UI::BeginMainMenuBar())
	{
		if (UI::BeginMenu("File##LavenderUI"))
		{
			if (UI::MenuItem("New File##LavenderUI", "Ctrl+N"))
			{
				LV_LOG_TRACE("New");
			}

			UI::EndMenu();
		}

		UI::Dummy({ 1.0f, 0.0f });
		if (UI::MenuItem("Edit##LavenderUI"))
		{
			LV_LOG_TRACE("Edit");
		}

		UI::EndMainMenuBar();
	}
}
