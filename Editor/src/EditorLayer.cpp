#include "EditorLayer.hpp"

#include <Lavender/Core/Application.hpp>
#include <Lavender/Core/Logging.hpp>
#include <Lavender/Utils/Utils.hpp>
#include <Lavender/Utils/Profiler.hpp>
#include <Lavender/FileSystem/PreferencesSerializer.hpp>

#include <Lavender/APIs/Vulkan/VulkanContext.hpp>
#include <Lavender/APIs/Vulkan/VulkanImage.hpp>
#include <Lavender/APIs/Vulkan/VulkanImGuiLayer.hpp>

#include <Lavender/Renderer/Renderer.hpp>
#include <Lavender/Renderer/Shader.hpp>

#include <Lavender/UI/UI.hpp>
#include <Lavender/UI/Style.hpp>
#include <Lavender/UI/Colours.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_vulkan.h>

static float vertices[] = {
	-0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
	0.5f, -0.5, 0.0f, 0.0f, 0.0f,
	0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
	-0.5f, 0.5, 0.0f, 1.0f, 1.0f
};

static uint32_t indices[] = {
	0, 1, 2,
	2, 3, 0
};

void EditorLayer::OnAttach()
{
	m_Preferences = RefHelper::Create<UIPreferences>();
	PreferencesSerializer serializer(m_Preferences);
	serializer.Deserialize("EditorPreferences.lvepref");

	m_Project = Project::Create();
	
	auto scene = m_Project->CreateAndAddScene();

	m_Image = Image2D::Create("assets/objects/viking_room.png");
	m_Mesh = Mesh("assets/objects/viking_room.obj");

	m_Entity = scene->CreateEntity();
	m_Entity.AddOrReplaceComponent<TagComponent>({ "Viking room" });
	m_Entity.AddComponent<TransformComponent>({ { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } });
	m_Entity.AddComponent<MeshComponent>(MeshComponent(m_Mesh, m_Image));

	auto entity2 = scene->CreateEntity();
	entity2.AddOrReplaceComponent<TagComponent>({ "Viking room2" });
	entity2.AddComponent<TransformComponent>({ { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } });
	entity2.AddComponent<MeshComponent>(MeshComponent(m_Mesh, m_Image));

	m_Project->AddScene(scene, "Main", true);

	m_EntityPanel = EntitiesPanel::Create(m_Project);

	// Test area // TODO: Remove
	TEMP();
}

void EditorLayer::OnDetach()
{
	PreferencesSerializer serializer(m_Preferences);
	serializer.Serialize("EditorPreferences.lvepref");
}

void EditorLayer::OnUpdate(float deltaTime)
{
	LV_PROFILE_SCOPE("EditorLayer::OnUpdate");

	m_Project->OnUpdate(deltaTime);

	// TODO: Remove
	static float timer = 0.0f;
	timer += deltaTime;
	if (timer > 1.0f)
	{
		Application::Get().GetWindow().SetTitle(fmt::format("Custom | FPS: {0}", (int)ImGui::GetIO().Framerate));
		timer = 0.0f;
	}
}

void EditorLayer::OnRender()
{
	LV_PROFILE_SCOPE("EditorLayer::OnRender");

	m_Project->OnRender();
}

void EditorLayer::OnImGuiRender()
{
	ImGui::DockSpaceOverViewport();
	RenderMenuBar();
	
	m_Project->OnImGuiRender();

	m_EntityPanel->RenderUI();
}

void EditorLayer::OnEvent(Event& e)
{
	EventHandler handler(e);

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

void EditorLayer::TEMP() // TODO: Remove
{
}
