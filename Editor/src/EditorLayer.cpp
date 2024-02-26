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
	LV_LOG_TRACE("A");
	Renderer::WaitFor(m_RenderPass->GetCommandBuffer());
	LV_LOG_TRACE("A");

	m_RenderPass->Begin();
	LV_LOG_TRACE("A");
	m_RenderPass->End();
	LV_LOG_TRACE("A");
	m_RenderPass->Submit();

	LV_LOG_TRACE("A");
}

void EditorLayer::OnImGuiRender()
{
}

void EditorLayer::OnEvent(Event& e)
{
}
