#pragma once

#include <filesystem>

#include "Lavender/Core/Events.hpp"

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Renderer/Viewport.hpp"

#include "Lavender/Scripting/ScriptLoader.hpp"

#include "Lavender/Workspace/Scene.hpp"
#include "Lavender/Workspace/ProjectSettings.hpp"

namespace Lavender
{

	class ProjectSerializer;

	enum class ScriptConfig
	{
		None = 0, Debug, Release, Dist
	};
	ScriptConfig StringToScriptConfig(const std::string& str);
	std::string ScriptConfigToString(ScriptConfig config);

	class Project
	{
	public:
		Project();
		Project(const std::filesystem::path& path);
		virtual ~Project();

		void StartRuntime();
		void StopRuntime();
		void SwitchState();

		void OnUpdate(float deltaTime);
		void OnRender();
		void OnImGuiRender();
		void OnEvent(Event& e);

		Ref<Scene> CreateAndAddScene();
		void AddScene(Ref<Scene> scene);
		void AddScene(const UUID& uuid, const SceneMetaData& data);

		void InitializeStartScene();

		void SetScript(Ref<ScriptLoader> script);
		void ReloadScript();

		inline ProjectState GetState() const { return m_State; }

		inline Ref<Viewport> GetViewport() { return m_Viewport; }
		inline Ref<AssetManager> GetAssetManager() { return m_Assets; }
		inline SceneCollection& GetSceneCollection() { return m_Scenes; }
		inline ProjectDirectories& GetDirectories() { return m_Directories; }

		inline Ref<ScriptLoader> GetScript() { return m_Script; }
		inline ScriptConfig& GetScriptConfig() { return m_ScriptConfig; }

		static Ref<Project> Create();
		static Ref<Project> Create(const std::filesystem::path& path);

		static Project* Get() { return s_Instance; }

	private:
		static Project* s_Instance;

		Ref<Viewport> m_Viewport = nullptr;

		Ref<AssetManager> m_Assets = nullptr;
		Ref<ScriptLoader> m_Script = nullptr;
		ScriptConfig m_ScriptConfig = ScriptConfig::Debug;

		SceneCollection m_Scenes = {};

		ProjectState m_State = ProjectState::Editor;
		ProjectDirectories m_Directories = {};
		std::filesystem::path m_StartScenePath = {};

		friend class Viewport;
		friend class ProjectSerializer;
	};

}