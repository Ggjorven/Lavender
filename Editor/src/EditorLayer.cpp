#include "EditorLayer.hpp"

#include <Lavender/Core/Application.hpp>
#include <Lavender/Core/Logging.hpp>
#include <Lavender/Utils/Utils.hpp>
#include <Lavender/Renderer/Renderer.hpp>

#include <Lavender/Renderer/Shader.hpp>

#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>

static float vertices[] = {
	0.0f,  0.5f, 0.0f,  // Vertex 0: Top
   -0.5f, -0.5f, 0.0f,  // Vertex 1: Bottom-left
	0.5f, -0.5f, 0.0f   // Vertex 2: Bottom-right
};

static uint32_t indices[] = {
	0, 1, 2
};

void EditorLayer::OnAttach()
{
	m_RenderPass = RenderPass::Create();

	ShaderCode code = {};
	code.VertexSPIRV = Shader::ReadSPIRVFile("assets/shaders/vert.spv");
	code.FragmentSPIRV = Shader::ReadSPIRVFile("assets/shaders/frag.spv");
	Ref<Shader> shader = Shader::Create(code);

	m_VertexBuffer = VertexBuffer::Create((void*)vertices, sizeof(vertices));
	m_IndexBuffer = IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));

	BufferLayout bufferLayout = {
		{ DataType::Float3, 0, "a_Position" },
	};

	PipelineLayout pipelineLayout = {};
	pipelineLayout.SetBufferLayout(bufferLayout);

	m_Pipeline = Pipeline::Create(pipelineLayout, shader, m_RenderPass);
	m_Pipeline->Initialize();
}

void EditorLayer::OnDetach()
{
}

void EditorLayer::OnUpdate(float deltaTime)
{
}

void EditorLayer::OnRender()
{
	m_RenderPass->Begin();

	m_Pipeline->Use(m_RenderPass->GetCommandBuffer());

	m_VertexBuffer->Bind(m_RenderPass->GetCommandBuffer());
	m_IndexBuffer->Bind(m_RenderPass->GetCommandBuffer());

	Renderer::DrawIndexed(m_RenderPass->GetCommandBuffer(), m_IndexBuffer);

	m_RenderPass->End();
	m_RenderPass->Submit();

	Renderer::WaitFor(m_RenderPass->GetCommandBuffer());
}

void EditorLayer::OnImGuiRender()
{
	ImGui::Begin("A");

	ImGui::Text("TEXT");
	if (ImGui::Button("BUTTON"))
	{
		LV_LOG_TRACE("BUTTON");
	}

	ImGui::End();
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
