#pragma once

#include <string>
#include <utility>
#include <functional>
#include <unordered_map>

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/ECS/Entity.hpp"
#include "Lavender/ECS/Registry.hpp"

#include "Lavender/Scripting/ScriptLoader.hpp"
#include "Lavender/Scripting/EntityInterface.hpp"
#include "Lavender/Scripting/RegistryInterface.hpp"

#ifdef LV_PLATFORM_WINDOWS
#include "Lavender/Platforms/Windows/WindowsEntityInterface.hpp"
#endif

namespace Lavender
{

	class SceneSerializer;

	class Scene
	{
	public:
		enum class State
		{
			None = 0, Editor, Runtime
		};

	public:
		Scene();
		virtual ~Scene();

		void ReloadScript();

		void StartRuntime();
		void StopRuntime();

		void OnUpdate(float deltaTime);
		void OnRender();

		void SetScript(Ref<ScriptLoader> script);
		void AddScriptedEntity(Ref<EntityInterface> entityInterface) { m_EntityInterfaces[entityInterface->GetEntity().GetUUID()] = entityInterface; }

		inline Ref<EntityInterface> GetEntityInterface(UUID uuid) { return m_EntityInterfaces[uuid]; }

		inline Ref<RegistryCollection> GetCollection() { return m_Collection; }
		inline Entity CreateEntity() { return Entity::Create(m_Collection); }

		static Ref<Scene> Create();

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
		}

	private:
		State m_State = State::Editor;

		Ref<RegistryCollection> m_Collection = nullptr;

		Ref<ScriptLoader> m_Script = nullptr;
		Ref<RegistryInterface> m_RegistryInterface = nullptr;
		std::unordered_map<UUID, Ref<EntityInterface>> m_EntityInterfaces = { };

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
		inline Ref<Scene> GetActive() { return m_ActiveScene.second; }

		typedef std::function<void(Ref<Scene>)> EachSceneFn;
		void Each(EachSceneFn function);

	private:
		std::unordered_map<std::string, Ref<Scene>> m_Scenes = { };
		std::pair<std::string, Ref<Scene>> m_ActiveScene = {};
	};

}