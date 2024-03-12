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

#include <Lavender/ECS/Registry.hpp>
#include <Lavender/ECS/Entity.hpp>
#include <Lavender/ECS/Components.hpp>

#include <Lavender/Scripting/ScriptLoader.hpp>
#include <Lavender/Scripting/EntityInterface.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_vulkan.h>

#include <iostream> // TODO: Remove

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

struct Camera
{
public:
	glm::mat4 Model = {};
	glm::mat4 View = {};
	glm::mat4 Projection = {};
};

void EditorLayer::OnAttach()
{
	m_Preferences = RefHelper::Create<UIPreferences>();
	PreferencesSerializer serializer(m_Preferences);
	serializer.Deserialize("EditorPreferences.lvepref");

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
		{ UniformDataType::Image, 0, 0, "u_Image", UniformElement::ShaderStage::Fragment },
		{ UniformDataType::UniformBuffer, 0, 1, "u_Camera", UniformElement::ShaderStage::Vertex }
	};

	PipelineSpecification pipelineSpecs = {};
	pipelineSpecs.Bufferlayout = bufferLayout;
	pipelineSpecs.Uniformlayout = uniformLayout;

	pipelineSpecs.Polygonmode = PipelineSpecification::PolygonMode::Fill;
	pipelineSpecs.LineWidth = 1.0f;
	pipelineSpecs.Cullingmode = PipelineSpecification::CullingMode::Back;

	m_Pipeline = Pipeline::Create(pipelineSpecs, shader, m_Viewport->GetRenderPass());
	m_Pipeline->Initialize();

	m_Image = Image2D::Create(m_Pipeline, uniformLayout.GetElementByName(0, "u_Image"), "assets/images/test.jpg");
	m_CameraBuffer = UniformBuffer::Create(m_Pipeline, uniformLayout.GetElementByName(0, "u_Camera"), sizeof(Camera));

	// Test area
	Ref<ScriptLoader> loader = ScriptLoader::Create("E:\\Code\\C++\\VS\\Lavender\\Editor\\Projects\\First\\Script\\bin\\Debug-windows-x86_64\\Script\\Script.dll");
	Ref<EntityInterface> interface = EntityInterface::Create(loader, "MyEntity");

	interface->InvokeOnCreate();
	interface->InvokeOnUpdate(1.0f);
}

void EditorLayer::OnDetach()
{
	PreferencesSerializer serializer(m_Preferences);
	serializer.Serialize("EditorPreferences.lvepref");
}

void EditorLayer::OnUpdate(float deltaTime)
{
	LV_PROFILE_SCOPE("EditorLayer::OnUpdate");
	auto& window = Application::Get().GetWindow();
	if (m_Viewport->GetWidth() != 0 && m_Viewport->GetHeight() != 0) // Note(Jorben): This if state is because glm::perspective doesnt allow the aspectratio to be 0
	{
		auto& window = Application::Get().GetWindow();

		Camera camera = {};
		camera.Model = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		camera.View = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		camera.Projection = glm::perspective(glm::radians(45.0f), (float)m_Viewport->GetWidth() / (float)m_Viewport->GetHeight(), 0.1f, 10.0f);

		if (Renderer::GetAPI() == RenderingAPI::Vulkan)
			camera.Projection[1][1] *= -1;

		m_CameraBuffer->SetData((void*)&camera, sizeof(Camera));
	}
}

void EditorLayer::OnRender()
{
	LV_PROFILE_SCOPE("EditorLayer::OnRender");
	m_Viewport->BeginFrame();

	m_Pipeline->Use(m_Viewport->GetRenderPass()->GetCommandBuffer());

	m_VertexBuffer->Bind(m_Viewport->GetRenderPass()->GetCommandBuffer());
	m_IndexBuffer->Bind(m_Viewport->GetRenderPass()->GetCommandBuffer());

	Renderer::DrawIndexed(m_Viewport->GetRenderPass()->GetCommandBuffer(), m_IndexBuffer);

	m_Viewport->EndFrame();
}

void EditorLayer::OnImGuiRender()
{
	ImGui::DockSpaceOverViewport();

	m_Viewport->BeginRender();
	m_Viewport->EndRender();

	ImGui::Begin("A");
	ImGui::Text((std::string("FPS: ") + std::to_string((int)ImGui::GetIO().Framerate)).c_str());
	ImGui::End();
}

void EditorLayer::OnEvent(Event& e)
{
	EventHandler handler(e);

	handler.Handle<WindowResizeEvent>(LV_BIND_EVENT_FN(EditorLayer::OnResizeEvent));
}

bool EditorLayer::OnResizeEvent(WindowResizeEvent& e)
{
	m_Viewport->SetShouldResize(true);

	return false;
}
