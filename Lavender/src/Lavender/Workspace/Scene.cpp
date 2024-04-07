#include "lvpch.h"
#include "Scene.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Utils/Profiler.hpp"

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

		m_EditorCamera = EditorCamera::Create(m_Viewport);
	}

	Scene::~Scene()
	{
		m_Assets->Serialize();
		SceneRenderer::Destroy();
	}

	void Scene::StartRuntime()
	{
		m_State = Scene::State::Runtime;
	
		m_Assets->SwitchAssets();
		m_Collection->SwitchRegistry();

		auto& registry = m_Collection->GetMainRegistry()->GetRegistry();
		auto view = registry.view<ScriptComponent>();
		for (auto& script : view)
		{
			ScriptComponent& component = view.get<ScriptComponent>(script);
			UUID uuid = registry.get<UUID>(script);

			m_EntityInterfaces[uuid] = EntityInterface::Create(uuid, m_Script, component.ClassName);
		}
	}

	void Scene::StopRuntime()
	{
		m_State = Scene::State::Editor;

		m_RegistryInterface.reset();
		m_EntityInterfaces.clear();
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
		LV_PROFILE_SCOPE("Scene::UpdateEditor");

		m_EditorCamera->OnUpdate(deltaTime);
	}

	void Scene::RenderEditor(Ref<RenderCommandBuffer> cmdBuffer)
	{
		LV_PROFILE_SCOPE("Scene::RenderEditor");

		SceneRenderer::RenderScene(this, m_EditorCamera, cmdBuffer);
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
		SceneRenderer::RenderScene(this, m_EditorCamera, cmdBuffer);
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
