#include "lvpch.h"
#include "Scene.hpp"

#include "Lavender/Core/Logging.hpp"
#include "Lavender/Utils/Profiler.hpp"

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

			// Remove
			m_Camera->OnUpdate(deltaTime);

			// TODO: Runtime updates
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

			// Remove
			m_Renderer->Render(m_Camera);

			// TODO: Runtime camera
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
			m_Camera->OnEvent(e);
			break;
		case WorkSpace::State::Runtime:

			// Remove
			m_Camera->OnEvent(e);

			// TODO: Runtime camera
			break;

		default:
			APP_LOG_ERROR("Invalid State passed in.");
			break;
		}
	}

	void Scene::StartRuntime()
	{
		Ref<ScriptingBackend> script = Project::Get()->GetScript();

		m_Registries[WorkSpace::State::Runtime].CopyRegistry(m_Registries[WorkSpace::State::Editor]);

		if (script)
		{
			script->Reload();
			script->OnCreateAll();
		}
	}

	void Scene::EndRuntime()
	{
		m_Registries[WorkSpace::State::Runtime].Clear();
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
	void SceneCollection::Add(const UUID& uuid, const WorkSpace::SceneInfo& info)
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

}