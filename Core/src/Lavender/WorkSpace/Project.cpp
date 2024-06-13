#include "lvpch.h"
#include "Project.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/FileSystem/SceneSerializer.hpp"

namespace Lavender
{

	static Ref<Project> s_Project = nullptr;

	Project::Project(const WorkSpace::ProjectInfo& info)
		: m_Info(info)
	{
	}

	Project::~Project()
	{
		m_Scripting.reset();
	}

	void Project::OnUpdate(float deltaTime)
	{
		m_Scenes.GetActive()->OnUpdate(deltaTime);
	}

	void Project::OnRender()
	{
		m_Scenes.GetActive()->OnRender();
	}

	void Project::OnEvent(Event& e)
	{
		m_Scenes.GetActive()->OnEvent(e);
	}

	Ref<Project> Project::Create(const WorkSpace::ProjectInfo& info)
	{
		s_Project = RefHelper::Create<Project>(info);
		return s_Project;
	}

	Ref<Project>& Project::Get()
	{
		return s_Project;
	}

	void Project::Init()
	{
		// Initialize startscene
		Ref<Scene> startScene = Scene::Create(m_Info.StartScene);
		SceneSerializer serializer(startScene);
		serializer.Deserialize(m_Info.Directory / m_Info.Scenes / m_Scenes.GetAll()[m_Info.StartScene].Path);

		m_Scenes.SetActive(m_Info.StartScene, startScene);

		// Initialize script
		if (std::filesystem::exists(m_Info.Directory / m_Info.Script / m_Info.ScriptsPath))
		{
			ScriptingSpecification scriptingSpecs = {};
			scriptingSpecs.Type = m_Info.ScriptType;
			scriptingSpecs.Path = m_Info.Directory / m_Info.Script / m_Info.ScriptsPath;

			m_Scripting = ScriptingBackend::Create(scriptingSpecs);
		}
	}

	void Project::Destroy()
	{
		// TODO: Maybe save all scenes??
		SceneSerializer serializer(m_Scenes.GetActive());
		serializer.Serialize();
	}

}