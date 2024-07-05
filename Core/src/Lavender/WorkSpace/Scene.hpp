#pragma once

#include "Lavender/Core/Core.hpp"
#include "Lavender/Core/Events.hpp"
#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Utils/UUID.hpp"

#include "Lavender/ECS/Registry.hpp"

#include "Lavender/WorkSpace/WorkSpace.hpp"
#include "Lavender/WorkSpace/Rendering/SceneRenderer.hpp"
#include "Lavender/WorkSpace/Rendering/EditorCamera.hpp"

namespace Lavender
{

	class SceneSerializer;

	struct SceneInfo
	{
	public:
		std::filesystem::path Path = {};
	};

	class Scene
	{
	public:
		Scene(const UUID& uuid);
		virtual ~Scene();

		void OnUpdate(float deltaTime);
		void OnRender();
		void OnEvent(Event& e);

		void StartRuntime();
		void EndRuntime();

		Ref<Camera> GetActiveCamera();

		inline void SetID(const UUID& uuid) { m_ID = uuid; }
		inline const UUID& GetID() { return m_ID; }
		inline Registry& GetRegistry() { return m_Registry; }
		inline Ref<EditorCamera> GetEditorCamera() { return m_Camera; }
		inline Ref<SceneRenderer> GetRenderer() { return m_Renderer; }
		
		static Ref<Scene> Create(const UUID& uuid = UUID());
		static Ref<Scene> Get(); // Returns the active scene

	private:
		UUID m_ID = UUID::Empty;
		SceneInfo m_Info = {};

		Registry m_Registry = { };

		Ref<EditorCamera> m_Camera = nullptr;
		Ref<SceneRenderer> m_Renderer = nullptr;

		friend class SceneSerializer;
	};





	struct SceneCollection
	{
	public:
		SceneCollection() = default;
		SceneCollection(const SceneCollection& other) = default;
		virtual ~SceneCollection() = default;

		void Add(const UUID& uuid, const SceneInfo& info);
		void Remove(const UUID& uuid); // Only removes the info
		void Clear();

		inline void SetActive(const UUID& uuid, Ref<Scene> scene) { m_ActiveScene = scene; m_LoadedScenes[uuid] = scene; }
		inline Ref<Scene> GetActive() { return m_ActiveScene; }
		Dict<UUID, SceneInfo> GetAll() { return m_ScenesInfo; }

	private:
		Dict<UUID, SceneInfo> m_ScenesInfo = { };

		Dict<UUID, Ref<Scene>> m_LoadedScenes = { };
		Ref<Scene> m_ActiveScene = nullptr;
	};

}