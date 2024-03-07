#include "EditorLayer.hpp"

#include <Lavender/Core/Application.hpp>
#include <Lavender/Core/Logging.hpp>
#include <Lavender/Utils/Utils.hpp>

#include <Lavender/APIs/Vulkan/VulkanContext.hpp>
#include <Lavender/APIs/Vulkan/VulkanImage.hpp>
#include <Lavender/APIs/Vulkan/VulkanImGuiLayer.hpp>

#include <Lavender/Renderer/Renderer.hpp>
#include <Lavender/Renderer/Shader.hpp>

#include <glm/gtc/matrix_transform.hpp>

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
	m_Viewport = Viewport::Create(Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight());

	ShaderCode code = {};
	code.VertexSPIRV = Shader::ReadSPIRVFile("assets/shaders/vert.spv");
	code.FragmentSPIRV = Shader::ReadSPIRVFile("assets/shaders/frag.spv");
	Ref<Shader> shader = Shader::Create(code);

	m_VertexBuffer = VertexBuffer::Create((void*)vertices, sizeof(vertices));
	m_IndexBuffer = IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));

	BufferLayout bufferLayout = {
		{ DataType::Float3, 0, "a_Position" },
		{ DataType::Float2, 1, "a_TexCoord" }
	};

	UniformLayout uniformLayout = {
		{ UniformDataType::Image, 0, 0, "u_Image", UniformElement::ShaderStage::Fragment }
	};

	PipelineSpecification pipelineSpecs = {};
	pipelineSpecs.Bufferlayout = bufferLayout;
	pipelineSpecs.Uniformlayout = uniformLayout;

	pipelineSpecs.Polygonmode = PipelineSpecification::PolygonMode::Fill;
	pipelineSpecs.LineWidth = 1.0f;
	pipelineSpecs.Cullingmode = PipelineSpecification::CullingMode::Front;

	m_Pipeline = Pipeline::Create(pipelineSpecs, shader, m_Viewport->GetRenderPass()->GetRenderPass());
	m_Pipeline->Initialize();

	m_Image = Image2D::Create(m_Pipeline, uniformLayout.GetElementByName(0, "u_Image"), "assets/images/test.jpg");
}

void EditorLayer::OnDetach()
{
}

void EditorLayer::OnUpdate(float deltaTime)
{
}

void EditorLayer::OnRender()
{
	m_Viewport->BeginFrame();

	m_Pipeline->Use(m_Viewport->GetRenderPass()->GetCommandBuffer());

	m_VertexBuffer->Bind(m_Viewport->GetRenderPass()->GetCommandBuffer());
	m_IndexBuffer->Bind(m_Viewport->GetRenderPass()->GetCommandBuffer());

	Renderer::DrawIndexed(m_Viewport->GetRenderPass()->GetCommandBuffer(), m_IndexBuffer);

	m_Viewport->EndFrame();

	Renderer::WaitFor(m_Viewport->GetRenderPass()->GetCommandBuffer());
}

void EditorLayer::OnImGuiRender()
{
	ImGui::Begin("A");

	ImGui::Text("TEXT");
	if (ImGui::Button("BUTTON"))
	{
		LV_LOG_TRACE("BUTTON");
	}

	ImGui::Image(m_Viewport->GetCurrentImGuiTexture(), ImVec2((float)Application::Get().GetWindow().GetWidth(), (float)Application::Get().GetWindow().GetHeight()));

	ImGui::End();
}

void EditorLayer::OnEvent(Event& e)
{
	EventHandler handler(e);

	handler.Handle<WindowResizeEvent>(LV_BIND_EVENT_FN(EditorLayer::OnResizeEvent));
}

bool EditorLayer::OnResizeEvent(WindowResizeEvent& e)
{
	m_Viewport->GetRenderPass()->Resize(e.GetWidth(), e.GetHeight());

	return false;
}
