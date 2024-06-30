#include "lvpch.h"
#include "Project.hpp"

#include "Lavender/Core/Logging.hpp"
#include "Lavender/Utils/Profiler.hpp"

#include "Lavender/FileSystem/SceneSerializer.hpp"

#include "Lavender/WorkSpace/EngineTracker.hpp"

namespace Lavender
{

	static WeakRef<Project> s_Project = {};

	Project::Project(const WorkSpace::ProjectInfo& info, WorkSpace::State initialState)
		: m_Info(info), m_State(initialState), m_Assets(AssetManager::Create())
	{
	}

	Project::~Project()
	{
	}

	void Project::OnUpdate(float deltaTime)
	{
		APP_PROFILE_SCOPE("Project::OnUpdate");
		m_Scenes.GetActive()->OnUpdate(deltaTime);
	}

	void Project::OnRender()
	{
		APP_PROFILE_SCOPE("Project::OnRender");
		m_Scenes.GetActive()->OnRender();
	}

	void Project::OnEvent(Event& e)
	{
		m_Scenes.GetActive()->OnEvent(e);
	}

	void Project::SwitchState()
	{
		switch (m_State)
		{
		case WorkSpace::State::Editor:
			m_State = WorkSpace::State::Runtime;
			m_Scenes.GetActive()->StartRuntime();
			break;
		case WorkSpace::State::Runtime:
			m_State = WorkSpace::State::Editor;
			m_Scenes.GetActive()->EndRuntime();
			break;

		default:
			APP_LOG_ERROR("Invalid State passed in.");
			break;
		}
	}

	Ref<Project> Project::Create(const WorkSpace::ProjectInfo& info, WorkSpace::State initialState)
	{
		Ref<Project> project = RefHelper::Create<Project>(info, initialState);
		s_Project = project;
		return project;
	}

	Ref<Project> Project::Get()
	{
		if (auto proj = s_Project.lock())
			return proj;

		return nullptr;
	}

	void Project::Init()
	{
		// Update Asset Cache
		m_Assets->UpdateCache(m_Info.Directory / m_Info.Assets);

		// Initialize script
		std::filesystem::path path = m_Info.Directory / m_Info.Script / "bin" / fmt::format("{0}-{1}", ConfigurationToString(Track::Lavender::Config), PlatformToString(Track::Lavender::Platform, false)) / "Script/Script.dll";
		if (std::filesystem::exists(path))
		{
			ScriptingSpecification scriptingSpecs = {};
			scriptingSpecs.Type = m_Info.ScriptType;
			scriptingSpecs.Path = path;

			m_Scripting = ScriptingBackend::Create(scriptingSpecs);
		}

		// Initialize startscene
		Ref<Scene> startScene = Scene::Create(m_Info.StartScene);
		SceneSerializer serializer(startScene);
		serializer.Deserialize(m_Info.Directory / m_Info.Scenes / m_Scenes.GetAll()[m_Info.StartScene].Path);

		m_Scenes.SetActive(m_Info.StartScene, startScene);
	}

	void Project::Destroy()
	{
		// TODO: Maybe save all scenes??
		SceneSerializer serializer(m_Scenes.GetActive());
		serializer.Serialize();

		m_Assets->Serialize();
	}

}