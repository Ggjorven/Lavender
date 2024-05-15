#include "EditorLayer.hpp"

#include <Lavender/Core/Application.hpp>
#include <Lavender/Core/Input/Input.hpp>

#include <Lavender/Renderer/Shader.hpp>

#include <imgui.h>

void EditorLayer::OnAttach()
{
}

void EditorLayer::OnDetach()
{
}

void EditorLayer::OnUpdate(float deltaTime)
{
	// Note(Jorben): All of this below is just to show some stats in the titlebar
	static float timer = 0.0f;
	static uint32_t FPS = 0;
	static uint32_t tempFPS = 0;
	timer += deltaTime;
	tempFPS += 1u;

	if (timer >= 1.0f)
	{
		FPS = (uint32_t)((float)tempFPS / timer);
		Application::Get().GetWindow().SetTitle(fmt::format("Editor | FPS: {0} | Frametime: {1:.3f}ms", FPS, timer / (float)FPS * 1000.0f));
		timer = 0.0f;
		tempFPS = 0u;
	}
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
