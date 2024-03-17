#pragma once

#include "Lavender/Utils/Utils.hpp"

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

		inline void AddScene(Ref<Scene> scene, const std::string& name = "Unnamed scene", bool active = false) { m_Scenes.Add(scene, name, active); }

		inline SceneCollection& GetSceneCollection() { return m_Scenes; }

		static Ref<Project> Create();

	private:
		SceneCollection m_Scenes = {};

		friend class ProjectSerializer;
	};

}