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
	}

	Ref<Project> Project::Create(const WorkSpace::ProjectInfo& info)
	{
		s_Project = RefHelper::Create<Project>(info);
		return s_Project;
	}

	Ref<Project> Project::Get()
	{
		return s_Project;
	}

	void Project::Init()
	{
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
	}

}