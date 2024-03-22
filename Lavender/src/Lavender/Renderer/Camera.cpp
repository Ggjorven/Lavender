#include "lvpch.h"
#include "Camera.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/Renderer.hpp"
#include "Lavender/Renderer/Pipeline.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace Lavender
{

	EditorCamera::EditorCamera(Ref<Viewport> viewport)
		: m_Viewport(viewport)
	{
		m_CameraUniform = UniformBuffer::Create(sizeof(Camera));
	}

	EditorCamera::~EditorCamera()
	{
	}

	void EditorCamera::OnUpdate(float deltaTime)
	{
		// TODO: Add more functionality
		{
			if (m_Viewport->GetWidth() != 0 && m_Viewport->GetHeight() != 0) // Note(Jorben): This if state is because glm::perspective doesn't allow the aspectratio to be 0
			{
				static float timer = 0.0f;
				timer += deltaTime;

				m_Camera.Model = glm::rotate(glm::mat4(1.0f), glm::radians(timer * 6.0f), glm::vec3(0.0f, 0.0f, 1.0f));
				m_Camera.View = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
				m_Camera.Projection = glm::perspective(glm::radians(45.0f), (float)m_Viewport->GetWidth() / (float)m_Viewport->GetHeight(), 0.1f, 10.0f);

				if (Renderer::GetAPI() == RenderingAPI::Vulkan)
					m_Camera.Projection[1][1] *= -1;
			}
		}

		UpdateAndUpload();
	}

	void EditorCamera::UpdateAndUpload()
	{
		m_CameraUniform->SetData((void*)&m_Camera, sizeof(Camera));

		auto pipeline = m_Viewport->GetPipeline();
		auto element = pipeline->GetSpecification().Uniformlayout.GetElementByName(0, "u_Camera");
		m_CameraUniform->Upload(pipeline, element);
	}

	Ref<EditorCamera> EditorCamera::Create(Ref<Viewport> viewport)
	{
		return RefHelper::Create<EditorCamera>(viewport);
	}

}