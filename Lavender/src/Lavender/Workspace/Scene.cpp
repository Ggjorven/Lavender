#include "lvpch.h"
#include "Scene.hpp"

#include "Lavender/Core/Logging.hpp"

namespace Lavender
{

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
			e->Reload();
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
			for (auto& e : m_EntityInterfaces)
				e->InvokeOnUpdate(deltaTime);
			break;
		}
		case Scene::State::Runtime:
		{
			for (auto& e : m_EntityInterfaces)
				e->InvokeOnUpdate(deltaTime);
			break;
		}

		default:
			LV_LOG_FATAL("No proper state specified.");
			break;
		}
	}

	void Scene::OnRender()
	{
		switch (m_State)
		{
		case Scene::State::Editor:
		{
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

	void Scene::OnImGuiRender()
	{
		switch (m_State)
		{
		case Scene::State::Editor:
		{
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

	Ref<Scene> Scene::Create()
	{
		return RefHelper::Create<Scene>();
	}

	void SceneCollection::Clear()
	{
		m_Scenes.clear();
	}

	void SceneCollection::Add(Ref<Scene> scene, const std::string& name)
	{
		m_Scenes[name] = scene;
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

}
