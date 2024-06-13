#include "lvpch.h"
#include "Scene.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/WorkSpace/Project.hpp"

namespace Lavender
{

	Scene::Scene(const UUID& uuid)
		: m_ID(uuid), m_Renderer(SceneRenderer::Create(this))
	{
	}

	Scene::~Scene()
	{
	}

	void Scene::OnUpdate(float deltaTime)
	{
		// Update based on state
	}

	void Scene::OnRender()
	{
		// TODO: SceneRenderer with Editor/Runtime camera
	}

	void Scene::OnEvent(Event& e)
	{
		// Event Editor

		// Event Runtime
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