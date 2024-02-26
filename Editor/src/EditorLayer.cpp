#include "EditorLayer.hpp"

#include <Lavender/Core/Application.hpp>
#include <Lavender/Core/Logging.hpp>
#include <Lavender/Utils/Utils.hpp>
#include <Lavender/Renderer/Renderer.hpp>

#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>

void EditorLayer::OnAttach()
{
	m_CommandBuffer = RenderCommandBuffer::Create();
}

void EditorLayer::OnDetach()
{
}

void EditorLayer::OnUpdate(float deltaTime)
{
}

void EditorLayer::OnRender()
{

	Renderer::WaitFor(m_CommandBuffer);

	m_CommandBuffer->Begin();
	m_CommandBuffer->End();
	m_CommandBuffer->Submit();

}

void EditorLayer::OnImGuiRender()
{
}

void EditorLayer::OnEvent(Event& e)
{
}
