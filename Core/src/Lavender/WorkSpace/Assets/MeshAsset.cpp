#include "lvpch.h"
#include "MeshAsset.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/WorkSpace/Project.hpp"

#include <Flow/Flow.hpp>

using namespace Flow;

namespace Lavender
{

	MeshAsset::MeshAsset(const AssetData& data)
		: Asset(data)
	{
	}

	void MeshAsset::Serialize()
	{
		Yaml::File file = Yaml::File(m_Path, FileMode::Write);
		file << Yaml::FileManip::BeginMap;

		file << Yaml::FileManip::Key << "AssetHandle" << Yaml::FileManip::Value << (uint64_t)m_Handle;
		file << Yaml::FileManip::Key << "AssetName" << Yaml::FileManip::Value << m_Name;

		file << Yaml::FileManip::Key << "MeshAsset" << Yaml::FileManip::Value << Yaml::FileManip::BeginMap;
		file << Yaml::FileManip::Key << "Mesh" << Yaml::FileManip::Value << m_MeshPath.string();

		file << Yaml::FileManip::EndMap;
		file << Yaml::FileManip::EndMap;
	}

	void MeshAsset::Deserialize(const std::filesystem::path& file)
	{
		Yaml::File flowFile = Yaml::File(m_Path, FileMode::Read);
		
		auto asset = flowFile["MeshAsset"];
		if (asset)
		{
			m_MeshPath = asset["Mesh"].as<std::string>();
		}

		// Initialize
		auto& projectDirs = Project::Get()->GetInfo();
		m_Mesh = Mesh::Create(projectDirs.Directory / projectDirs.Assets / m_MeshPath);
	}

	Ref<MeshAsset> MeshAsset::Create(const AssetData& data)
	{
		return RefHelper::Create<MeshAsset>(data);
	}

	Ref<Asset> MeshAsset::Copy()
	{
		return Ref<Asset>();
	}

}