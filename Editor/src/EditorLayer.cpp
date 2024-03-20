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

	auto mainScene = m_Project->GetSceneCollection().GetActive();
	mainScene->SetScriptVariable(mainScene->GetEntityInterface(m_Entity.GetUUID()), "A", 69.0f);

	auto& window = Application::Get().GetWindow();
	if (m_Viewport->GetWidth() != 0 && m_Viewport->GetHeight() != 0) // Note(Jorben): This if state is because glm::perspective doesn't allow the aspectratio to be 0
	{
		static float timer = 0.0f;
		timer += deltaTime;

		Camera camera = {};
		camera.Model = glm::rotate(glm::mat4(1.0f), glm::radians(timer * 6.0f), glm::vec3(0.0f, 0.0f, 1.0f));
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
	ImGui::ShowStyleEditor();

	ImGui::DockSpaceOverViewport();
	ImGui::ShowDemoWindow();

	RenderMenuBar();
	
	m_Viewport->BeginRender();
	m_Viewport->EndRender();

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
	m_Viewport->Resize(e.GetWidth(), e.GetHeight());

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
	auto scene = Scene::Create();

	auto scriptLoader = ScriptLoader::Create("D:\\Code\\C++\\VS\\Lavender\\Editor\\Projects\\First\\Script\\bin\\Debug-windows-x86_64\\Script\\Script.dll");
	scene->SetScript(scriptLoader);

	m_Entity = scene->CreateEntity();
	m_Entity.AddComponent<TagComponent>({ "MYTAG" });
	m_Entity.AddComponent<TransformComponent>({ { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } });

	auto entityInterface = EntityInterface::Create(m_Entity, scriptLoader, "MyEntity");
	scene->AddScriptedEntity(entityInterface);

	m_Project = Project::Create();
	m_Project->AddScene(scene, "Main", true);

	m_EntityPanel = EntitiesPanel::Create(m_Project);
}
