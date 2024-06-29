#pragma once
 
#include "Lavender/Core/Core.hpp"
#include "Lavender/Core/Events.hpp"
#include "Lavender/Utils/Utils.hpp"

#include "Lavender/WorkSpace/Scene.hpp"
#include "Lavender/WorkSpace/WorkSpace.hpp"

#include "Lavender/WorkSpace/Assets/AssetManager.hpp"

#include "Lavender/Scripting/ScriptingBackend.hpp"

namespace Lavender
{

	class ProjectSerializer;

	class Project
	{
	public:
		Project(const WorkSpace::ProjectInfo& info, WorkSpace::State initialState);
		virtual ~Project();

		void OnUpdate(float deltaTime);
		void OnRender();
		void OnEvent(Event& e);

		void SwitchState();

		inline const WorkSpace::ProjectInfo& GetInfo() { return m_Info; }
		inline WorkSpace::State& GetState() { return m_State; }
		inline SceneCollection& GetScenes() { return m_Scenes; }
		inline Ref<AssetManager> GetAssets() { return m_Assets; }
		inline Ref<ScriptingBackend> GetScript() { return m_Scripting; }

		static Ref<Project> Create(const WorkSpace::ProjectInfo& info = WorkSpace::ProjectInfo(), WorkSpace::State initialState = WorkSpace::State::Runtime);
		static Ref<Project>& Get(); // Returns a reference since we also use it to destroy the s_Instance

	private:
		void Init();
		void Destroy();

	private:
		WorkSpace::ProjectInfo m_Info = {};
		WorkSpace::State m_State = WorkSpace::State::None;
		SceneCollection m_Scenes = { };

		Ref<AssetManager> m_Assets = nullptr;
		Ref<ScriptingBackend> m_Scripting = nullptr;

		friend class ProjectSerializer;
	};

}