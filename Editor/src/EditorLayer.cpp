#include "EditorLayer.hpp"

#include <Lavender/Core/Application.hpp>
#include <Lavender/Core/Logging.hpp>
#include <Lavender/Utils/Utils.hpp>
#include <Lavender/Renderer/Renderer.hpp>

#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>

void EditorLayer::OnAttach()
{
	m_RenderPass = RenderPass::Create();
}

void EditorLayer::OnDetach()
{
}

void EditorLayer::OnUpdate(float deltaTime)
{
}

void EditorLayer::OnRender()
{
	Renderer::WaitFor(m_RenderPass->GetCommandBuffer());
	
	m_RenderPass->Begin();
	m_RenderPass->End();
	m_RenderPass->Submit();
}

void EditorLayer::OnImGuiRender()
{
}

void EditorLayer::OnEvent(Event& e)
{
	EventHandler handler(e);

	handler.Handle<WindowResizeEvent>(LV_BIND_EVENT_FN(EditorLayer::OnResizeEvent));
}

bool EditorLayer::OnResizeEvent(WindowResizeEvent& e)
{
	m_RenderPass->Resize(e.GetWidth(), e.GetHeight());

	return false;
}
