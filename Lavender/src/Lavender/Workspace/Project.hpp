#pragma once

#include <filesystem>

#include "Lavender/Core/Events.hpp"

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Renderer/Viewport.hpp"

#include "Lavender/Workspace/Scene.hpp"

namespace Lavender
{

	class ProjectSerializer;

	struct ProjectDirectories
	{
	public:
		std::filesystem::path Project = "";
		std::filesystem::path ProjectDir = "";
		std::filesystem::path Assets = "Assets";
		std::filesystem::path Scripts = "Script";

	public:
		ProjectDirectories() = default;
		ProjectDirectories(const ProjectDirectories& other) = default;
		virtual ~ProjectDirectories() = default;
	};

	class Project
	{
	public:
		Project();
		Project(const std::filesystem::path& path);
		virtual ~Project();

		void StartRuntime();
		void StopRuntime();

		void OnUpdate(float deltaTime);
		void OnRender();
		void OnImGuiRender();
		void OnEvent(Event& e);

		Ref<Scene> CreateAndAddScene();
		void AddScene(Ref<Scene> scene);
		void AddScene(const UUID& uuid, const SceneMetaData& data);

		void InitializeStartScene();

		inline SceneCollection& GetSceneCollection() { return m_Scenes; }
		inline Ref<Viewport> GetViewport() { return m_Viewport; }
		inline ProjectDirectories& GetDirectories() { return m_Directories; }

		static Ref<Project> Create();
		static Ref<Project> Create(const std::filesystem::path& path);

		static Project* Get() { return s_Instance; }

	private:
		static Project* s_Instance;

		Ref<Viewport> m_Viewport = nullptr;

		SceneCollection m_Scenes = {};

		ProjectDirectories m_Directories = {};
		std::filesystem::path m_StartScenePath = {};

		friend class ProjectSerializer;
	};

}