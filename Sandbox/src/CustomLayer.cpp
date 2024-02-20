#include "CustomLayer.hpp"

#include <Lavender/Core/Application.hpp>
#include <Lavender/Core/Logging.hpp>
#include <Lavender/Renderer/Shader.hpp>
#include <Lavender/Renderer/Renderer.hpp>

#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>

struct Camera
{
public:
	glm::mat4 Model = {};
	glm::mat4 View = {};
	glm::mat4 Projection = {};
};

void CustomLayer::OnAttach() 
{
	m_Controller = std::make_shared<RenderController>();

	m_Mesh = std::make_shared<Mesh>("assets/objects/viking_room.obj");

	BufferLayout bufferLayout = {
		BufferElement(BufferDataType::Float3, 0, "a_Position"),
		BufferElement(BufferDataType::Float2, 1, "a_TexCoord")
	};
	m_Controller->SetBufferLayout(bufferLayout);

	std::shared_ptr<Shader> shader = std::make_shared<Shader>(Shader::ReadSPIRVFile("assets/shaders/vert.spv"), Shader::ReadSPIRVFile("assets/shaders/frag.spv"));
	m_Controller->SetShader(shader);

	UniformElement texElement = UniformElement(UniformDataType::Image, 0, "u_Texture", UniformElement_Stage_Fragment);
	UniformElement camElement = UniformElement(UniformDataType::UBO, 1, "u_Camera", UniformElement_Stage_Vertex);
	UniformLayout uniformLayout = {
		texElement,
		camElement
	};
	m_Controller->SetUniformLayout(uniformLayout);

	m_Texture = std::make_shared<Image>(m_Controller, texElement, "assets/objects/viking_room.png");
	
	m_CameraBuffer = std::make_shared<UniformBuffer>(m_Controller, camElement, sizeof(Camera));

	m_Controller->Initialize();
	m_Texture->UploadToController();
	m_CameraBuffer->UploadToController();
}

void CustomLayer::OnDetach()
{
}

void CustomLayer::OnUpdate(float deltaTime)
{
	Window& window = Application::Get().GetWindow();

	Camera camera = {};
	camera.Model = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	camera.View = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	camera.Projection = glm::perspective(glm::radians(45.0f), (float)window.GetWidth() / (float)window.GetHeight(), 0.1f, 100.0f);
	camera.Projection[1][1] *= -1;

	m_CameraBuffer->SetData((void*)&camera, sizeof(Camera));
}

void CustomLayer::OnRender()
{
	Renderer::AddToQueue([this]()
	{
		m_Controller->Use();

		m_Mesh->GetVertexBuffer()->Bind();
		m_Mesh->GetIndexBuffer()->Bind();

		Renderer::DrawIndexed(m_Mesh->GetIndexBuffer());
	});
}

void CustomLayer::OnImGuiRender()
{
	ImGui::Begin("Abc");
	ImGui::Text(fmt::format("FPS: {:.0f}", ImGui::GetIO().Framerate).c_str());
	ImGui::End();
}

void CustomLayer::OnEvent(Event& e)
{
}
