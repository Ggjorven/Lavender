#include "lvpch.h"
#include "ProjectSerializer.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/FileSystem/YAMLUtils.hpp"

#include <fstream>

namespace Lavender
{

	ProjectSerializer::ProjectSerializer(Ref<Project> project)
		: m_Project(project)
	{
	}

	ProjectSerializer::~ProjectSerializer()
	{
	}

	void ProjectSerializer::Serialize()
	{
		YAML::Emitter data = {};

		data << YAML::BeginMap;
		data << YAML::Key << "Project";
		data << YAML::Value << YAML::BeginMap;

		{
			data << YAML::Key << "Directories";
			data << YAML::Value << YAML::BeginMap;

			data << YAML::Key << "Assets";
			data << YAML::Value << m_Project->m_Directories.Assets.string();

			data << YAML::Key << "Scripts";
			data << YAML::Value << m_Project->m_Directories.Scripts.string();

			data << YAML::EndMap;
		}
		{
			data << YAML::Key << "Scenes";
			data << YAML::Value << YAML::BeginMap;

			data << YAML::Key << "Start";
			data << YAML::Value << m_Project->m_StartScenePath.string();

			data << YAML::EndMap;
		}

		data << YAML::EndMap;

		data << YAML::EndMap;

		std::ofstream fileStream(m_Project->m_Directories.Project);
		if (!fileStream.good() || !fileStream.is_open())
		{
			LV_LOG_ERROR("Failed to open file {0}", m_Project->m_Directories.Project.string());
			return;
		}

		fileStream << data.c_str();
		fileStream.close();
	}

	void ProjectSerializer::Deserialize(const std::filesystem::path& path)
	{
		YAML::Node data = {};
		try
		{
			data = YAML::LoadFile(path.string());
		}
		catch (YAML::BadFile e)
		{
			LV_LOG_WARN("Failed to load {0} (Code: {1})", path.string(), e.what());
			return;
		}

		auto project = data["Project"];
		if (project)
		{
			auto directories = project["Directories"];
			if (directories)
			{
				m_Project->m_Directories.Project = path;
				m_Project->m_Directories.ProjectDir = path.parent_path();
				if (directories["Assets"]) m_Project->m_Directories.Assets = std::filesystem::path(directories["Assets"].as<std::string>());
				if (directories["Scripts"]) m_Project->m_Directories.Scripts = std::filesystem::path(directories["Scripts"].as<std::string>());
			}

			auto scenes = project["Scenes"];
			if (scenes)
			{
				if (scenes["Start"])
				{
					m_Project->m_StartScenePath = std::filesystem::path(scenes["Start"].as<std::string>());
					m_Project->InitializeStartScene();
				}
			}
		}
	}

}