#include "lvpch.h"
#include "MaterialAsset.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/WorkSpace/Project.hpp"

#include <Flow/Flow.hpp>

using namespace Flow;

namespace Lavender
{

	MaterialAsset::MaterialAsset(const AssetData& data)
		: Asset(data)
	{
	}

	void MaterialAsset::Serialize()
	{
		Yaml::File file = Yaml::File(m_Path, FileMode::Write);
		file << Yaml::FileManip::BeginMap;

		file << Yaml::FileManip::Key << "AssetHandle" << Yaml::FileManip::Value << (uint64_t)m_Handle;
		file << Yaml::FileManip::Key << "AssetName" << Yaml::FileManip::Value << m_Name;

		file << Yaml::FileManip::Key << "MaterialAsset" << Yaml::FileManip::Value << Yaml::FileManip::BeginMap;
		file << Yaml::FileManip::Key << "Albedo" << Yaml::FileManip::Value << m_AlbedoPath.string();

		file << Yaml::FileManip::EndMap;
		file << Yaml::FileManip::EndMap;
	}

	void MaterialAsset::Deserialize(const std::filesystem::path& file)
	{
		Yaml::File flowFile = Yaml::File(m_Path, FileMode::Read);

		auto asset = flowFile["MaterialAsset"];
		if (asset)
		{
			m_AlbedoPath = asset["Albedo"].as<std::string>();
		}

		// Initialize
		auto& projectDirs = Project::Get()->GetInfo();

		MaterialSpecification specs = { };
		specs.Albedo = projectDirs.Directory / projectDirs.Assets / m_AlbedoPath;

		m_Material = Material::Create(specs);
	}

	Ref<MaterialAsset> MaterialAsset::Create(const AssetData& data)
	{
		return RefHelper::Create<MaterialAsset>(data);
	}

	Ref<Asset> MaterialAsset::Copy()
	{
		return Ref<Asset>();
	}

}