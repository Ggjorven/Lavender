#include "lvpch.h"
#include "Scene.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/Renderer.hpp"
#include "Lavender/Renderer/RenderCommandBuffer.hpp"
#include "Lavender/Renderer/FrameResources.hpp"

#include "Lavender/Workspace/SceneRenderer.hpp"

namespace Lavender
{

	///////////////////////////////////////////////////////////////////////////
	// SceneCollection
	///////////////////////////////////////////////////////////////////////////
	Scene::Scene(Ref<Viewport> viewport)
		: Scene(viewport, UUID::Create())
	{
	}

	Scene::Scene(Ref<Viewport> viewport, const UUID& uuid)
		: m_Viewport(viewport), m_Collection(RegistryCollection::Create()), m_Assets(AssetManager::Create()), m_UUID(uuid)
	{
		SceneRenderer::Init();
	}

	Scene::~Scene()
	{
		m_Assets->Serialize();
		SceneRenderer::Destroy();
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
			UpdateEditor(deltaTime);
			break;
		case Scene::State::Runtime:
			UpdateRuntime(deltaTime);
			break;

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
			RenderEditor(cmdBuffer);
			break;
		case Scene::State::Runtime:
			RenderRuntime(cmdBuffer);
			break;

		default:
			LV_LOG_FATAL("No proper state specified.");
			break;
		}
	}

	void Scene::OnEvent(Event& e)
	{
		switch (m_State)
		{
		case Scene::State::Editor:
			m_EditorCamera->OnEvent(e);
			break;
		case Scene::State::Runtime:
			break;

		default:
			LV_LOG_FATAL("No proper state specified.");
			break;
		}
	}

	void Scene::SetScript(Ref<ScriptLoader> script)
	{
		m_Script = script;
		m_RegistryInterface = RegistryInterface::Create(m_Collection, m_Script);
		m_EntityInterfaces.clear();
	}

	void Scene::ReloadScript()
	{
		m_Script->Reload();
		m_RegistryInterface->Reload();

		for (auto& e : m_EntityInterfaces)
			e.second->Reload();
	}

	Ref<Scene> Scene::Create(Ref<Viewport> viewport)
	{
		return RefHelper::Create<Scene>(viewport);
	}

	Ref<Scene> Scene::Create(Ref<Viewport> viewport, const UUID& uuid)
	{
		return RefHelper::Create<Scene>(viewport, uuid);
	}

	void Scene::UpdateEditor(float deltaTime)
	{
		m_EditorCamera->OnUpdate(deltaTime);
	}

	void Scene::RenderEditor(Ref<RenderCommandBuffer> cmdBuffer)
	{
		m_EditorCamera->BindDescriptorSet(FrameResources::GetPipeline(), cmdBuffer);
		SceneRenderer::RenderScene(this, m_EditorCamera, cmdBuffer);
	}

	void Scene::UpdateRuntime(float deltaTime)
	{
	}

	void Scene::RenderRuntime(Ref<RenderCommandBuffer> cmdBuffer)
	{
	}

	///////////////////////////////////////////////////////////////////////////
	// SceneCollection
	///////////////////////////////////////////////////////////////////////////
	void SceneCollection::Clear()
	{
		m_Scenes.clear();
	}

	void SceneCollection::Add(Ref<Scene> scene, bool active)
	{
		m_Scenes[scene->GetSceneID()] = scene;

		if (active)
			m_ActiveScene = std::make_pair(scene->GetSceneID(), scene);
	}

	void SceneCollection::Remove(UUID uuid)
	{
		m_Scenes.erase(uuid);
	}

	Ref<Scene> SceneCollection::Get(UUID uuid)
	{
		auto obj = m_Scenes.find(uuid);
		if (obj != m_Scenes.end())
			return m_Scenes[uuid];

		LV_LOG_WARN("Failed to find scene by uuid: '{0}'...", uuid.Get());
		return nullptr;
	}

	void SceneCollection::Each(SceneCollection::EachSceneFn function)
	{
		for (auto& scene : m_Scenes)
			function(scene.second);
	}

}
