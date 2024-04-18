#pragma once

#include <string>
#include <utility>
#include <functional>
#include <filesystem>
#include <unordered_map>

#include "Lavender/Utils/Utils.hpp"
#include "Lavender/Utils/UUID.hpp"

#include "Lavender/ECS/Entity.hpp"
#include "Lavender/ECS/Registry.hpp"

#include "Lavender/Scripting/EntityInterface.hpp"
#include "Lavender/Scripting/RegistryInterface.hpp"

#include "Lavender/Renderer/Viewport.hpp"
#include "Lavender/Renderer/Camera.hpp"
#include "Lavender/Renderer/UniformBuffer.hpp"

#include "Lavender/Workspace/ProjectSettings.hpp"
#include "Lavender/Workspace/Assets/AssetManager.hpp"

#ifdef LV_PLATFORM_WINDOWS
#include "Lavender/Platforms/Windows/WindowsEntityInterface.hpp"
#endif

namespace Lavender
{

	class SceneSerializer;
	class RenderCommandBuffer;

	template<typename T>
	void SetScriptVariable(Ref<EntityInterface> entityInterface, const std::string& name, T value);
	template<typename T>
	T GetScriptVariable(Ref<EntityInterface> entityInterface, const std::string& name);
	
	struct SceneMetaData
	{
	public:
		std::filesystem::path Path = {};
	};

	class Scene
	{
	public:
		Scene(Ref<Viewport> viewport, Ref<AssetManager> assets, Ref<ScriptLoader> script);
		Scene(Ref<Viewport> viewport, Ref<AssetManager> assets, Ref<ScriptLoader> script, const UUID& uuid);
		virtual ~Scene();

		void StartRuntime();
		void StopRuntime();

		void OnUpdate(ProjectState state, float deltaTime);
		void OnRender(ProjectState state, Ref<RenderCommandBuffer> cmdBuffer);
		void OnEvent(ProjectState state, Event& e);

		void SetScript(Ref<ScriptLoader> script);
		void ReloadScript();
		void AddScriptedEntity(Ref<EntityInterface> entityInterface) { m_EntityInterfaces[entityInterface->GetEntity().GetUUID()] = entityInterface; }


		inline Ref<EditorCamera>& GetCamera() { return m_EditorCamera; }
		inline Ref<RegistryCollection> GetCollection() { return m_Collection; }
		inline Ref<Viewport> GetViewport() { return m_Viewport; }
		inline Ref<EditorCamera> GetEditorCamera() { return m_EditorCamera; }
		inline Ref<AssetManager> GetAssetManager() { return m_AssetsReference; }
		inline Ref<RegistryInterface> GetRegistryInterface() { return m_RegistryInterface; }
		inline Ref<EntityInterface> GetEntityInterface(const UUID& uuid) { return m_EntityInterfaces[uuid]; }
		inline UUID GetSceneID() { return m_UUID; }
		inline SceneMetaData GetMetaData() { return m_Data; }

		inline Entity CreateEntity() { return Entity::Create(m_Collection); }
		inline Entity CreateEntityWithUUID(const UUID& uuid) { return Entity::Create(m_Collection, uuid); }
		inline void DeleteEntity(const UUID& uuid) { m_Collection->DeleteEntity(uuid); }

		static Ref<Scene> Create(Ref<Viewport> viewport, Ref<AssetManager> assets, Ref<ScriptLoader> script);
		static Ref<Scene> Create(Ref<Viewport> viewport, Ref<AssetManager> assets, Ref<ScriptLoader> script, const UUID& uuid);

	private:
		void UpdateEditor(float deltaTime);
		void UpdateRuntime(float deltaTime);

		void RenderEditor(Ref<RenderCommandBuffer> cmdBuffer);
		void RenderRuntime(Ref<RenderCommandBuffer> cmdBuffer);

	private:
		UUID m_UUID = {};

		Ref<AssetManager> m_AssetsReference = nullptr;
		Ref<ScriptLoader> m_ScriptReference = nullptr;

		Ref<RegistryCollection> m_Collection = nullptr;
		Ref<RegistryInterface> m_RegistryInterface = nullptr;
		Dict<UUID, Ref<EntityInterface>> m_EntityInterfaces = { };

		Ref<Viewport> m_Viewport = nullptr;
		Ref<EditorCamera> m_EditorCamera = nullptr;

		SceneMetaData m_Data = {};

		friend class SceneSerializer;
	};

	struct SceneCollection
	{
	public:
		SceneCollection() = default;
		SceneCollection(const SceneCollection& other) = default;
		virtual ~SceneCollection() = default;

		void Clear();
		void Add(Ref<Scene> scene);
		void Add(const UUID& uuid, const SceneMetaData& data);
		void Remove(const UUID& uuid);
		SceneMetaData GetData(const UUID& uuid);

		inline UUID GetActiveUUID() { return m_ActiveScene.first; }
		inline Ref<Scene> GetActive() { return m_ActiveScene.second; }

	private:
		Dict<UUID, SceneMetaData> m_Scenes = { };
		std::pair<UUID, Ref<Scene>> m_ActiveScene = {};
	};

	////////////////////////////////////////////////////////////////////////////////
	// Helper functions for scripts
	////////////////////////////////////////////////////////////////////////////////
	template<typename T>
	inline void SetScriptVariable(Ref<EntityInterface> entityInterface, const std::string& name, T value)
	{
		#ifdef LV_PLATFORM_WINDOWS
		Ref<WindowsEntityInterface> i = RefHelper::RefAs<WindowsEntityInterface>(entityInterface);
		if (i->HasVariable(name))
		{
			i->Set<T>(name, value);
			return;
		}
		#endif

		LV_LOG_WARN("Variable by name '{0}' doesn't exist.", name);
		return;
	}

	template<typename T>
	inline T GetScriptVariable(Ref<EntityInterface> entityInterface, const std::string& name)
	{
		#ifdef LV_PLATFORM_WINDOWS
		Ref<WindowsEntityInterface> i = RefHelper::RefAs<WindowsEntityInterface>(entityInterface);
		if (i->HasVariable(name))
		{
			return i->Get<T>(name);
		}
		#endif

		LV_LOG_WARN("Variable by name '{0}' doesn't exist.", name);
		return T();
	}

}