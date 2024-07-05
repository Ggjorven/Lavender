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

	struct ProjectInfo
	{
	public:
		std::string Name = {};
		UUID StartScene = UUID::Empty;

		std::filesystem::path Path = {};
		std::filesystem::path Directory = {};

		std::filesystem::path Assets = {};
		std::filesystem::path Script = {};
		std::filesystem::path Scenes = {};

		WorkSpace::ScriptType ScriptType = WorkSpace::ScriptType::None;
	};

	class Project
	{
	public:
		Project(const ProjectInfo& info, WorkSpace::State initialState);
		virtual ~Project();

		void OnUpdate(float deltaTime);
		void OnRender();
		void OnEvent(Event& e);

		inline const ProjectInfo& GetInfo() { return m_Info; }
		inline WorkSpace::State& GetState() { return m_State; }
		inline SceneCollection& GetScenes() { return m_Scenes; }
		inline Ref<AssetManager> GetAssets() { return m_Assets; }
		inline Ref<ScriptingBackend> GetScript() { return m_Scripting; }

		void LoadScript();

		static Ref<Project> Create(const ProjectInfo& info = ProjectInfo(), WorkSpace::State initialState = WorkSpace::State::Runtime);
		static Ref<Project> Get();

	private:
		void Init();
		void Destroy();

	private:
		ProjectInfo m_Info = {};
		WorkSpace::State m_State = WorkSpace::State::None;
		SceneCollection m_Scenes = { };

		Ref<AssetManager> m_Assets = nullptr;
		Ref<ScriptingBackend> m_Scripting = nullptr;

		friend class ProjectSerializer;
	};

}