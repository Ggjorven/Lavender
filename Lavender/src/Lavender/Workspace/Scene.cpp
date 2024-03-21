#include "lvpch.h"
#include "Scene.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/Renderer.hpp"
#include "Lavender/Renderer/RenderCommandBuffer.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
// TODO: Remove ^^

namespace Lavender
{

	// TODO: Move/remove
	struct TempCamera
	{
	public:
		glm::mat4 Model = {};
		glm::mat4 View = {};
		glm::mat4 Projection = {};
	};

	Scene::Scene()
		: m_Collection(RegistryCollection::Create())
	{
	}

	Scene::~Scene()
	{
	}

	void Scene::ReloadScript()
	{
		m_Script->Reload();
		m_RegistryInterface->Reload();
		
		for (auto& e : m_EntityInterfaces)
			e.second->Reload();
	}

	void Scene::StartRuntime()
	{
		m_State = Scene::State::Runtime;
	}

	void Scene::StopRuntime()
	{
		m_State = Scene::State::Editor;
	}

	void Scene::OnUpdate(float deltaTime)
	{
		switch (m_State)
		{
		case Scene::State::Editor:
		{
			if (m_Script && !m_Script->IsDetached())
			{
				for (auto& e : m_EntityInterfaces)
					e.second->InvokeOnUpdate(deltaTime);
			}
			{
				/*
				auto& window = Application::Get().GetWindow();
				if (m_Project->GetViewport()->GetWidth() != 0 && m_Project->GetViewport()->GetHeight() != 0) //Note(Jorben): This if state is because glm::perspective doesn't allow the aspectratio to be 0
				{
					static float timer = 0.0f;
					timer += deltaTime;
				
					Camera camera = {};
					camera.Model = glm::rotate(glm::mat4(1.0f), glm::radians(timer * 6.0f), glm::vec3(0.0f, 0.0f 1.0f));
					camera.View = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec(0.0f, 0.0f, 1.0f));
					camera.Projection = glm::perspective(glm::radians(45.0f), (float)m_Project->GetViewport()>GetWidth() / (float)m_Project->GetViewport()->GetHeight(), 0.1f, 10.0f);
				
					if (Renderer::GetAPI() == RenderingAPI::Vulkan)
						camera.Projection[1][1] *= -1;
				
					m_CameraBuffer->SetData((void*)&camera, sizeof(Camera));
				}
				*/
			}
			break;
		}
		case Scene::State::Runtime:
		{
			if (m_Script && !m_Script->IsDetached())
			{
				for (auto& e : m_EntityInterfaces)
					e.second->InvokeOnUpdate(deltaTime);
			}
			break;
		}

		default:
			LV_LOG_FATAL("No proper state specified.");
			break;
		}
	}

	void Scene::OnRender(Ref<RenderCommandBuffer> cmdBuffer)
	{
		switch (m_State)
		{
		case Scene::State::Editor:
		{
			auto view = m_Collection->GetMainRegistry()->GetRegistry().view<MeshComponent>();
			for (auto& entity : view)
			{
				if ((uint32_t)entity == 0)
					break;

				MeshComponent& mesh = view.get<MeshComponent>(entity);

				m_Assets->GetPipeline(AssetManager::PipelineType::MeshAndImage)->Use(cmdBuffer);
			
				mesh.MeshObject.GetVertexBuffer()->Bind(cmdBuffer);
				mesh.MeshObject.GetIndexBuffer()->Bind(cmdBuffer);

				Renderer::DrawIndexed(cmdBuffer, mesh.MeshObject.GetIndexBuffer());
			}

			break;
		}
		case Scene::State::Runtime:
		{
			break;
		}

		default:
			LV_LOG_FATAL("No proper state specified.");
			break;
		}
	}

	void Scene::SetScript(Ref<ScriptLoader> script)
	{
		m_Script = script;
		m_RegistryInterface = RegistryInterface::Create(m_Collection, m_Script);

		// TODO: Reinitialize everything
	}

	void Scene::InitializeAssets(Ref<RenderPass> renderPass)
	{
		m_Assets = AssetManager::Create(renderPass);

		// TODO: Move to SceneRenderer and change how this works
		Ref<Pipeline> meshPipeline = m_Assets->GetPipeline(AssetManager::PipelineType::MeshAndImage); // Default pipeline?

		auto& layout = meshPipeline->GetSpecification().Uniformlayout;
		m_CameraBuffer = UniformBuffer::Create(meshPipeline, layout.GetElementByName(0, "u_Camera"), sizeof(TempCamera));
	}

	Ref<Scene> Scene::Create()
	{
		return RefHelper::Create<Scene>();
	}

	void SceneCollection::Clear()
	{
		m_Scenes.clear();
	}

	void SceneCollection::Add(Ref<Scene> scene, const std::string& name, bool active)
	{
		m_Scenes[name] = scene;

		if (active)
			m_ActiveScene = std::make_pair(name, scene);
	}

	void SceneCollection::Remove(const std::string& name)
	{
		m_Scenes.erase(name);
	}

	Ref<Scene> SceneCollection::Get(const std::string& name)
	{
		auto obj = m_Scenes.find(name);
		if (obj != m_Scenes.end())
			return m_Scenes[name];

		LV_LOG_WARN("Failed to find scene by name: '{0}'...", name);
		return nullptr;
	}

	void SceneCollection::Each(SceneCollection::EachSceneFn function)
	{
		for (auto& scene : m_Scenes)
			function(scene.second);
	}

}
