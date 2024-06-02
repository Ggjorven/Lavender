#include "EditorLayer.hpp"

#include <Lavender/Core/Application.hpp>
#include <Lavender/Core/Input/Input.hpp>

#include <Lavender/Renderer/Shader.hpp>

#include <Lavender/FileSystem/ProjectSerializer.hpp>

#include <Lavender/WorkSpace/EngineTracker.hpp>

#include <imgui.h>

#include <Lavender/ECS/Entity.hpp>
#include <Lavender/ECS/Components.hpp>

void EditorLayer::OnAttach()
{
	m_Project = Project::Create();
	m_Project->GetState() = WorkSpace::State::Editor;

	ProjectSerializer serializer(m_Project);
	serializer.Deserialize("Projects/Example/Example.lvproj");
}

void EditorLayer::OnDetach()
{
	ProjectSerializer serializer(m_Project);
	serializer.Serialize();
}

void EditorLayer::OnUpdate(float deltaTime)
{
	Application::Get().GetWindow().SetTitle(fmt::format("Editor | FPS: {0} | Frametime: {1:.3f}ms", Track::Frame::FPS, Track::Frame::FrameTime));
}

void EditorLayer::OnRender()
{
}

void EditorLayer::OnEvent(Event& e)
{
}

void EditorLayer::OnUIRender()
{
	ImGui::Begin("A");
	
	ImGui::Text("Text");
	
	ImGui::End();
}
