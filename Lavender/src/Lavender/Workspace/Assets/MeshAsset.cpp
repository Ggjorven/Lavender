#include "lvpch.h"
#include "MeshAsset.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/FileSystem/YAMLUtils.hpp"

#include "Lavender/Workspace/Project.hpp"

#include <fstream>

namespace Lavender
{

	MeshAsset::MeshAsset(const std::filesystem::path& path)
		: m_Path(path), m_OriginalPath(path)
	{
		Deserialize(path);
	}

	MeshAsset::MeshAsset(const std::filesystem::path& path, const std::filesystem::path& meshPath)
		: m_Path(path), m_OriginalPath(path), m_MeshPath(meshPath), m_Mesh(Mesh::Create(meshPath))
	{
	}

	MeshAsset::~MeshAsset() 
	{
		if (m_Path != m_OriginalPath && std::filesystem::exists(m_OriginalPath))
			std::filesystem::remove(m_OriginalPath);
	}

	void MeshAsset::Serialize()
	{
		YAML::Emitter data = {};

		data << YAML::BeginMap;
		data << YAML::Key << "MeshAsset";
		data << YAML::Value << m_Handle.Get();
		
		data << YAML::Key << "MetaData";
		data << YAML::Value << YAML::BeginMap;

		data << YAML::Key << "Path";
		data << YAML::Value << m_MeshPath.string();

		data << YAML::EndMap;

		data << YAML::EndMap;

		std::ofstream fileStream(m_Path);
		if (!fileStream.good() || !fileStream.is_open())
		{
			LV_LOG_ERROR("Failed to open file {0}", m_Path.string());
			return;
		}

		fileStream << data.c_str();
		fileStream.close();
	}

	void MeshAsset::Deserialize(const std::filesystem::path& path)
	{
		m_Path = path;
		m_OriginalPath = path;

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

		auto handle = data["MeshAsset"];
		if (handle)
			m_Handle = UUID(handle.as<uint64_t>());

		auto metadata = data["MetaData"];
		if (metadata)
		{
			auto meshPath = metadata["Path"];
			if (meshPath)
			{
				m_MeshPath = std::filesystem::path(meshPath.as<std::string>());
				auto path = Project::Get()->GetDirectories().ProjectDir / Project::Get()->GetDirectories().Assets / m_MeshPath;

				if (std::filesystem::exists(path))
					m_Mesh = Mesh::Create(path);
				else
					LV_LOG_ERROR("(Mesh) Mesh by path: '{0}' doesn't exist.", m_MeshPath);
			}
		}
	}

	Ref<Asset> MeshAsset::Copy()
	{
		Ref<MeshAsset> newAsset = RefHelper::Create<MeshAsset>();

		newAsset->m_Handle = m_Handle;

		newAsset->m_Path = m_Path;
		newAsset->m_MeshPath = m_MeshPath;
		
		newAsset->m_Mesh = m_Mesh->Copy();

		return newAsset;
	}

	Ref<MeshAsset> MeshAsset::Create()
	{
		return RefHelper::Create<MeshAsset>();
	}

	Ref<MeshAsset> MeshAsset::Create(const std::filesystem::path& path)
	{
		return RefHelper::Create<MeshAsset>(path);
	}

	Ref<MeshAsset> MeshAsset::Create(const std::filesystem::path& path, const std::filesystem::path& meshPath)
	{
		return RefHelper::Create<MeshAsset>(path, meshPath);
	}

}