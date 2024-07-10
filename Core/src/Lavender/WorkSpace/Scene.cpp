#include "lvpch.h"
#include "Scene.hpp"

#include "Lavender/Core/Logging.hpp"
#include "Lavender/Utils/Profiler.hpp"
#include "Lavender/Core/Input/Input.hpp"

#include "Lavender/FileSystem/SceneSerializer.hpp"

#include "Lavender/WorkSpace/Project.hpp"

namespace Lavender
{

	Scene::Scene(const UUID& uuid)
		: m_ID(uuid), m_Renderer(SceneRenderer::Create(this)), m_Camera(EditorCamera::Create())
	{
	}

	Scene::~Scene()
	{
	}

	void Scene::OnUpdate(float deltaTime)
	{
		APP_PROFILE_SCOPE("Scene::OnUpdate");

		switch (Project::Get()->GetState())
		{
		case WorkSpace::State::Editor:
		{
			m_Camera->OnUpdate(deltaTime);
			break;
		}
		case WorkSpace::State::Runtime:
		{
			Ref<ScriptingBackend> script = Project::Get()->GetScript();
			if (script)
			{
				script->OnUpdateAll(deltaTime);
			}

			GetActiveCamera()->OnUpdate(deltaTime);
			break;
		}

		default:
			APP_LOG_ERROR("Invalid State passed in.");
			break;
		}
	}

	void Scene::OnRender()
	{
		APP_PROFILE_SCOPE("Scene::OnRender");

		switch (Project::Get()->GetState())
		{
		case WorkSpace::State::Editor:
			m_Renderer->Render(m_Camera);
			break;
		case WorkSpace::State::Runtime:
			m_Renderer->Render(GetActiveCamera());
			break;

		default:
			APP_LOG_ERROR("Invalid State passed in.");
			break;
		}
	}

	void Scene::OnEvent(Event& e)
	{
		m_Renderer->OnEvent(e);

		switch (Project::Get()->GetState())
		{
		case WorkSpace::State::Editor:
		{
			m_Camera->OnEvent(e);
			break;
		}
		case WorkSpace::State::Runtime:
		{
			Ref<ScriptingBackend> script = Project::Get()->GetScript();
			if (script)
			{
				script->OnEventAll(e);
			}

			GetActiveCamera()->OnEvent(e);
			break;
		}

		default:
			APP_LOG_ERROR("Invalid State passed in.");
			break;
		}
	}

	void Scene::StartRuntime()
	{
		Ref<ScriptingBackend> script = Project::Get()->GetScript();
		if (script)
		{
			script->Reload();
			script->OnCreateAll();
		}
	}

	void Scene::EndRuntime()
	{
	}

	Ref<Camera> Scene::GetActiveCamera()
	{
		switch (Project::Get()->GetState())
		{
		case WorkSpace::State::Editor:		return m_Camera;
		case WorkSpace::State::Runtime:
		{
			auto view = m_Registry.GetRegistry().view<CameraComponent>();
			
			// TODO: ScriptAble Camera
			// TODO: Create a better system
			for (auto& entity : view)
			{
				CameraComponent& camera = view.get<CameraComponent>(entity);
				if (!camera.Active) continue;

				auto transformView = m_Registry.GetRegistry().view<TransformComponent>();
				if (!transformView.contains(entity))
				{
					APP_LOG_WARN("Found active camera, but camera has no TransformComponent. Resorting to EditorCamera.");
					return m_Camera;
				}

				TransformComponent& transform = transformView.get<TransformComponent>(entity);
				
				// TODO: Improve this system, damn this sucks
				return Camera::Create(camera.Yaw, camera.Pitch, camera.FOV, transform.Position, camera.Near, camera.Far);
			}

			APP_LOG_WARN("No runtime camera exists (or is set as Active). Resorting to EditorCamera.");
			return m_Camera;
		}

		default:
			APP_LOG_ERROR("Invalid State passed in");
			break;
		}

		return nullptr;
	}

	Ref<Scene> Scene::Create(const UUID& uuid)
	{
		return RefHelper::Create<Scene>(uuid);
	}

	Ref<Scene> Scene::Get()
	{
		return Project::Get()->GetScenes().GetActive();
	}



	// SceneCollection
	void SceneCollection::Add(const UUID& uuid, const SceneInfo& info)
	{
		m_ScenesInfo[uuid] = info;
	}

	void SceneCollection::Remove(const UUID& uuid)
	{
		m_ScenesInfo.erase(uuid);
	}

	void SceneCollection::Clear()
	{
		m_ScenesInfo.clear();
		m_LoadedScenes.clear();
		m_ActiveScene = nullptr;
	}

	void SceneCollection::Unload(const UUID& uuid)
	{
		if (m_LoadedScenes.contains(uuid))
		{
			m_LoadedScenes[uuid] = nullptr;
			if (m_ActiveScene->GetID() == uuid)
				m_ActiveScene = nullptr;
		}
		else
			APP_LOG_ERROR("Scene by ID: {0} doesn't exist.", (uint64_t)uuid);
	}

	void SceneCollection::UnloadActive()
	{
		if (m_ActiveScene)
		{
			m_LoadedScenes[m_ActiveScene->GetID()] = nullptr;
			m_ActiveScene = nullptr;
		}
	}

	void SceneCollection::UnloadAll()
	{
		m_ActiveScene = nullptr;
		m_LoadedScenes.clear();
	}

	void SceneCollection::SetActive(const UUID& uuid, Ref<Scene> scene)
	{
		if (scene)
		{
			m_ActiveScene = scene;
			m_LoadedScenes[uuid] = scene;
		}
		else
		{
			if (m_ScenesInfo.contains(uuid))
			{
				SceneInfo& info = m_ScenesInfo[uuid];

				Ref<Scene> newScene = Scene::Create(uuid);

				SceneSerializer serializer(newScene);
				serializer.Deserialize(info.Path);

				m_ActiveScene = newScene;
				m_LoadedScenes[uuid] = newScene;
			}
			else
				APP_LOG_ERROR("Scene by ID: {0} doesn't exist.", (uint64_t)uuid);
		}
	}

}