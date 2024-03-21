#pragma once

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Renderer/Viewport.hpp"

#include "Lavender/Workspace/Scene.hpp"

namespace Lavender
{

	class ProjectSerializer;

	class Project
	{
	public:
		Project();
		virtual ~Project();

		void StartRuntime();
		void StopRuntime();

		void OnUpdate(float deltaTime);
		void OnRender();
		void OnImGuiRender();

		void AddScene(Ref<Scene> scene, const std::string& name = "Unnamed scene", bool active = false);

		inline SceneCollection& GetSceneCollection() { return m_Scenes; }
		inline Ref<Viewport> GetViewport() { return m_Viewport; }

		static Ref<Project> Create();

	private:
		Ref<Viewport> m_Viewport = nullptr;

		SceneCollection m_Scenes = {};

		friend class ProjectSerializer;
	};

}