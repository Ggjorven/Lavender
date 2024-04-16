#include "lvpch.h"
#include "Scene.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Utils/Profiler.hpp"

#include "Lavender/Renderer/Renderer.hpp"
#include "Lavender/Renderer/RenderCommandBuffer.hpp"
#include "Lavender/Renderer/FrameResources.hpp"

#include "Lavender/Workspace/Project.hpp"
#include "Lavender/Workspace/SceneRenderer.hpp"

namespace Lavender
{

	///////////////////////////////////////////////////////////////////////////
	// SceneCollection
	///////////////////////////////////////////////////////////////////////////
	Scene::Scene(Ref<Viewport> viewport, Ref<AssetManager> assets, Ref<ScriptLoader> script)
		: Scene(viewport, assets, script, UUID::Create())
	{
	}

	Scene::Scene(Ref<Viewport> viewport, Ref<AssetManager> assets, Ref<ScriptLoader> script, const UUID& uuid)
		: m_Viewport(viewport), m_Collection(RegistryCollection::Create()), m_AssetsReference(assets), m_ScriptReference(script), m_UUID(uuid)
	{
		m_EditorCamera = EditorCamera::Create(m_Viewport);
	}

	Scene::~Scene()
	{
	}

	void Scene::StartRuntime()
	{
		m_Collection->SwitchRegistry();

		auto& registry = m_Collection->GetMainRegistry()->GetRegistry();
		auto view = registry.view<ScriptComponent>();
		for (auto& script : view)
		{
			ScriptComponent& component = view.get<ScriptComponent>(script);
			UUID uuid = registry.get<UUID>(script);

			m_EntityInterfaces[uuid] = EntityInterface::Create(uuid, m_ScriptReference, component.ClassName);
		}
	}

	void Scene::StopRuntime()
	{
		m_Collection->SwitchRegistry();

		m_RegistryInterface.reset();
		m_EntityInterfaces.clear();
	}

	void Scene::OnUpdate(ProjectState state, float deltaTime)
	{
		switch (state)
		{
		case ProjectState::Editor:
			UpdateEditor(deltaTime);
			break;
		case ProjectState::Runtime:
			UpdateRuntime(deltaTime);
			break;

		default:
			LV_LOG_FATAL("No proper state specified.");
			break;
		}
	}

	void Scene::OnRender(ProjectState state, Ref<RenderCommandBuffer> cmdBuffer)
	{
		switch (state)
		{
		case ProjectState::Editor:
			RenderEditor(cmdBuffer);
			break;
		case ProjectState::Runtime:
			RenderRuntime(cmdBuffer);
			break;

		default:
			LV_LOG_FATAL("No proper state specified.");
			break;
		}
	}

	void Scene::OnEvent(ProjectState state, Event& e)
	{
		switch (state)
		{
		case ProjectState::Editor:
			m_EditorCamera->OnEvent(e);
			break;
		case ProjectState::Runtime:
			break;

		default:
			LV_LOG_FATAL("No proper state specified.");
			break;
		}
	}

	void Scene::SetScript(Ref<ScriptLoader> script)
	{
		m_ScriptReference = script;
		m_RegistryInterface = RegistryInterface::Create(m_Collection, m_ScriptReference);
		m_EntityInterfaces.clear();
	}

	void Scene::ReloadScript()
	{
		m_RegistryInterface->Reload();

		for (auto& e : m_EntityInterfaces)
			e.second->Reload();
	}

	Ref<Scene> Scene::Create(Ref<Viewport> viewport, Ref<AssetManager> assets, Ref<ScriptLoader> script)
	{
		return RefHelper::Create<Scene>(viewport, assets, script);
	}

	Ref<Scene> Scene::Create(Ref<Viewport> viewport, Ref<AssetManager> assets, Ref<ScriptLoader> script, const UUID& uuid)
	{
		return RefHelper::Create<Scene>(viewport, assets, script, uuid);
	}

	void Scene::UpdateEditor(float deltaTime)
	{
		LV_PROFILE_SCOPE("Scene::UpdateEditor");

		m_EditorCamera->OnUpdate(deltaTime);
	}

	void Scene::RenderEditor(Ref<RenderCommandBuffer> cmdBuffer)
	{
		LV_PROFILE_SCOPE("Scene::RenderEditor");

		SceneRenderer::RenderScene(this, m_EditorCamera->GetPosition(), cmdBuffer);
	}

	void Scene::UpdateRuntime(float deltaTime)
	{
		LV_PROFILE_SCOPE("Scene::UpdateRuntime");

		// TODO: Update some physics or something

		for (auto& entity : m_EntityInterfaces)
			entity.second->InvokeOnUpdate(deltaTime);
	}

	void Scene::RenderRuntime(Ref<RenderCommandBuffer> cmdBuffer)
	{
		LV_PROFILE_SCOPE("Scene::RenderRuntime");

		// TODO: Change camera
		SceneRenderer::RenderScene(this, m_EditorCamera->GetPosition(), cmdBuffer);
	}

	///////////////////////////////////////////////////////////////////////////
	// SceneCollection
	///////////////////////////////////////////////////////////////////////////
	void SceneCollection::Clear()
	{
		m_Scenes.clear();
	}

	void SceneCollection::Add(Ref<Scene> scene)
	{
		m_ActiveScene = std::make_pair(scene->GetSceneID(), scene);
		m_Scenes[scene->GetSceneID()] = scene->GetMetaData();
	}

	void SceneCollection::Add(const UUID& uuid, const SceneMetaData& data)
	{
		m_Scenes[uuid] = data;
	}

	void SceneCollection::Remove(const UUID& uuid)
	{
		m_Scenes.erase(uuid);
	}

	SceneMetaData SceneCollection::GetData(const UUID& uuid)
	{
		auto obj = m_Scenes.find(uuid);
		if (obj != m_Scenes.end())
			return m_Scenes[uuid];

		LV_LOG_WARN("Failed to find scene by uuid: '{0}'...", uuid.Get());
		return {};
	}

}
