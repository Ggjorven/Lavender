#include "lvpch.h"
#include "MeshAsset.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/FileSystem/YAMLUtils.hpp"

#include <fstream>

namespace Lavender
{

	MeshAsset::MeshAsset(const std::filesystem::path& path)
		: m_Path(path)
	{
		Deserialize(path);
	}

	MeshAsset::MeshAsset(const std::filesystem::path& path, const std::filesystem::path& meshPath)
		: m_Path(path), m_MeshPath(meshPath), m_Mesh(Mesh::Create(meshPath))
	{
	}

	MeshAsset::~MeshAsset() // TODO: Create new vb's and ib's
	{
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

				if (std::filesystem::exists(m_MeshPath))
					m_Mesh = Mesh::Create(m_MeshPath);
				else
					LV_LOG_ERROR("(Mesh) Mesh by path: '{0}' doesn't exist.", m_MeshPath);
			}
		}
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