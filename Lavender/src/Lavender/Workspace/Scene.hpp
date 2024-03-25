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

#include "Lavender/Scripting/ScriptLoader.hpp"
#include "Lavender/Scripting/EntityInterface.hpp"
#include "Lavender/Scripting/RegistryInterface.hpp"

#include "Lavender/Renderer/Viewport.hpp"
#include "Lavender/Renderer/Camera.hpp"
#include "Lavender/Renderer/UniformBuffer.hpp"

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

	class Scene
	{
	public:
		enum class State
		{
			None = 0, Editor, Runtime
		};

	public:
		Scene(Ref<Viewport> viewport);
		Scene(Ref<Viewport> viewport, const UUID& uuid);
		virtual ~Scene();

		void StartRuntime();
		void StopRuntime();

		void OnUpdate(float deltaTime);
		void OnRender(Ref<RenderCommandBuffer> cmdBuffer);

		void SetScript(Ref<ScriptLoader> script);
		void ReloadScript();
		void AddScriptedEntity(Ref<EntityInterface> entityInterface) { m_EntityInterfaces[entityInterface->GetEntity().GetUUID()] = entityInterface; }

		inline Ref<EntityInterface> GetEntityInterface(UUID uuid) { return m_EntityInterfaces[uuid]; }

		inline Ref<EditorCamera>& GetCamera() { return m_EditorCamera; }
		inline Ref<RegistryCollection> GetCollection() { return m_Collection; }
		inline Ref<Viewport> GetViewport() { return m_Viewport; }
		inline Ref<AssetManager> GetAssetManager() { return m_Assets; }

		inline Entity CreateEntity() { return Entity::Create(m_Collection); }
		inline Entity CreateEntityWithUUID(const UUID& uuid) { return Entity::Create(m_Collection, uuid); }

		static Ref<Scene> Create(Ref<Viewport> viewport);
		static Ref<Scene> Create(Ref<Viewport> viewport, const UUID& uuid);

	private:
		void UpdateEditor(float deltaTime);
		void UpdateRuntime(float deltaTime);

		void RenderEditor(Ref<RenderCommandBuffer> cmdBuffer);
		void RenderRuntime(Ref<RenderCommandBuffer> cmdBuffer);

	private:
		UUID m_UUID = {};

		Ref<AssetManager> m_Assets = nullptr;

		Ref<RegistryCollection> m_Collection = nullptr;
		Ref<ScriptLoader> m_Script = nullptr;
		Ref<RegistryInterface> m_RegistryInterface = nullptr;
		Dict<UUID, Ref<EntityInterface>> m_EntityInterfaces = { };

		Ref<Viewport> m_Viewport = nullptr;
		Ref<EditorCamera> m_EditorCamera = nullptr;

		std::filesystem::path m_Path = "Projects/First/Scenes/first.lvscene";

		State m_State = State::Editor;

		friend class SceneSerializer;
	};

	struct SceneCollection
	{
	public:
		SceneCollection() = default;
		SceneCollection(const SceneCollection& other) = default;
		virtual ~SceneCollection() = default;

		void Clear();
		void Add(Ref<Scene> scene, const std::string& name = "Unnamed Scene", bool active = false);
		void Remove(const std::string& name);
		Ref<Scene> Get(const std::string& name);

		inline std::string GetActiveName() { return m_ActiveScene.first; }
		inline Ref<Scene> GetActive() { return m_ActiveScene.second; }

		typedef std::function<void(Ref<Scene>)> EachSceneFn;
		void Each(EachSceneFn function);

	private:
		Dict<std::string, Ref<Scene>> m_Scenes = { };
		std::pair<std::string, Ref<Scene>> m_ActiveScene = {};
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