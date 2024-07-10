#include "lvpch.h"
#include "ProjectSerializer.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/FileSystem/Serialization.hpp"
#include "Lavender/FileSystem/SerializationUtils.hpp"

#include <Flow/Flow.hpp>

using namespace Flow;
using namespace Flow::Yaml;

//////////////////////////////////////////////////
// Core Functions
//////////////////////////////////////////////////
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

		File<SerializationType::Yaml> file = File<SerializationType::Yaml>(info.Path, SerializationMode::Serialize);

		// Project Name
		file << FileManip::Key << "Project";
		file << FileManip::Value << info.Name;

		// Directories
		file << FileManip::Key << "Directories";
		file << FileManip::Value << FileManip::BeginMap;

		file << FileManip::Key << "Assets";
		file << FileManip::Value << info.Assets.string();

		file << FileManip::Key << "Script";
		file << FileManip::Value << info.Script.string();

		file << FileManip::Key << "Scenes";
		file << FileManip::Value << info.Scenes.string();

		file << FileManip::EndMap;
	
		// Scenes
		file << FileManip::Key << "Scenes";
		file << FileManip::Value << FileManip::BeginSeq;

		for (auto& [id, info] : m_Project->m_Scenes.GetAll())
		{
			file << FileManip::BeginMap;

			file << FileManip::Key << "Scene";
			file << FileManip::Value << (uint64_t)id;

			file << FileManip::Key << "Path";
			file << FileManip::Value << info.Path.string();

			file << FileManip::EndMap;
		}
		
		file << FileManip::EndSeq;

		file << FileManip::Key << "StartScene";
		file << FileManip::Value << (uint64_t)info.StartScene;

		// Scripting
		file << FileManip::Key << "Scripting";
		file << FileManip::Value << FileManip::BeginMap;

		file << FileManip::Key << "Type";
		file << FileManip::Value << (uint32_t)info.ScriptType;

		file << FileManip::EndMap;

		m_Project->Destroy();
	}

	void ProjectSerializer::Deserialize(const std::filesystem::path& filepath)
	{
		auto& info = m_Project->m_Info;
		info.Path = filepath;
		info.Directory = filepath.parent_path();

		File<SerializationType::Yaml> file = File<SerializationType::Yaml>(filepath, SerializationMode::Deserialize);

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