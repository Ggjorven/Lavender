#include "lvpch.h"
#include "ProjectSerializer.hpp"

#include "Lavender/Core/Logging.hpp"

#include <Flow/Flow.hpp>

using namespace Flow;

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
		auto& info = m_Project->m_Info;

		Yaml::File file = Yaml::File(info.Path, FileMode::Write);
		file << Yaml::FileManip::BeginMap;

		// Project Name
		file << Yaml::FileManip::Key << "Project";
		file << Yaml::FileManip::Value << info.Name;

		// Directories
		file << Yaml::FileManip::Key << "Directories";
		file << Yaml::FileManip::Value << Yaml::FileManip::BeginMap;

		file << Yaml::FileManip::Key << "Assets";
		file << Yaml::FileManip::Value << info.Assets.string();

		file << Yaml::FileManip::Key << "Script";
		file << Yaml::FileManip::Value << info.Script.string();

		file << Yaml::FileManip::Key << "Scenes";
		file << Yaml::FileManip::Value << info.Scenes.string();

		file << Yaml::FileManip::EndMap;
	
		// Scenes
		file << Yaml::FileManip::Key << "Scenes";
		file << Yaml::FileManip::Value << Yaml::FileManip::BeginSeq;

		for (auto& [id, info] : m_Project->m_Scenes.GetAll())
		{
			file << Yaml::FileManip::BeginMap;

			file << Yaml::FileManip::Key << "Scene";
			file << Yaml::FileManip::Value << (uint64_t)id;

			file << Yaml::FileManip::Key << "Path";
			file << Yaml::FileManip::Value << info.Path.string();

			file << Yaml::FileManip::EndMap;
		}
		
		file << Yaml::FileManip::EndSeq;

		file << Yaml::FileManip::Key << "StartScene";
		file << Yaml::FileManip::Value << (uint64_t)info.StartScene;

		// Scripting
		file << Yaml::FileManip::Key << "Scripting";
		file << Yaml::FileManip::Value << Yaml::FileManip::BeginMap;

		file << Yaml::FileManip::Key << "Type";
		file << Yaml::FileManip::Value << (uint32_t)info.ScriptType;

		file << Yaml::FileManip::EndMap;


		file << Yaml::FileManip::EndMap;

		m_Project->Destroy();
	}

	void ProjectSerializer::Deserialize(const std::filesystem::path& filepath)
	{
		auto& info = m_Project->m_Info;
		info.Path = filepath;
		info.Directory = filepath.parent_path();

		Yaml::File file = Yaml::File(filepath, FileMode::Read);
		// Name
		info.Name = file["Project"].as<std::string>();

		// Directories
		auto directories = file["Directories"];
		info.Assets = directories["Assets"].as<std::string>();
		info.Script = directories["Script"].as<std::string>();
		info.Scenes = directories["Scenes"].as<std::string>();

		// Scenes
		auto& collection = m_Project->m_Scenes;
		auto scenes = file["Scenes"];
		for (auto scene : scenes)
			collection.Add(scene["Scene"].as<uint64_t>(), { scene["Path"].as<std::string>()});

		info.StartScene = file["StartScene"].as<uint64_t>();

		// Scripting
		auto scripting = file["Scripting"];
		info.ScriptType = (WorkSpace::ScriptType)scripting["Type"].as<uint32_t>();

		m_Project->Init();
	}

}