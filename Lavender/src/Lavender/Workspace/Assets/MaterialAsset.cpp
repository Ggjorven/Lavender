#include "lvpch.h"
#include "MaterialAsset.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/FileSystem/YAMLUtils.hpp"

#include "Lavender/Workspace/Project.hpp"

#include <fstream>

namespace Lavender
{

	MaterialAsset::MaterialAsset(const std::filesystem::path& path)
		: m_Path(path), m_OriginalPath(path)
	{
		Deserialize(path);
	}

	MaterialAsset::~MaterialAsset()
	{
		if (m_Path != m_OriginalPath && std::filesystem::exists(m_OriginalPath))
			std::filesystem::remove(m_OriginalPath);
	}

	void MaterialAsset::Serialize()
	{
		YAML::Emitter data = {};

		data << YAML::BeginMap;
		data << YAML::Key << "Material";
		data << YAML::Value << m_Handle.Get();

		data << YAML::Key << "MetaData";
		data << YAML::Value << YAML::BeginMap;

		data << YAML::Key << "Albedo";
		data << YAML::Value << m_AlbedoPath.string();

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

	void MaterialAsset::Deserialize(const std::filesystem::path& path)
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

		auto handle = data["Material"];
		if (handle)
		{
			m_Handle = UUID(handle.as<uint64_t>());
		}

		auto metadata = data["MetaData"];
		if (metadata)
		{
			auto albedo = metadata["Albedo"];
			if (albedo)
			{
				m_AlbedoPath = std::filesystem::path(albedo.as<std::string>());
				auto path = Project::Get()->GetDirectories().ProjectDir / Project::Get()->GetDirectories().Assets / m_AlbedoPath;

				if (std::filesystem::exists(path) && !path.filename().empty())
				{
					ImageSpecification specs = {};
					specs.Usage = ImageSpecification::ImageUsage::File;
					specs.Flags = ImageSpecification::ImageUsageFlags::Sampled;
					specs.Path = path;
					m_Albedo = Image2D::Create(specs);
				}
				else
					LV_LOG_ERROR("(Material) Albedo path: '{0}' doesn't exist.", m_AlbedoPath.string());
			}
		}
	}

	void MaterialAsset::SetAlbedo(Ref<Image2D> image, const std::filesystem::path& path)
	{
		m_Albedo = image;
		m_AlbedoPath = path;
	}

	void MaterialAsset::Upload(Ref<DescriptorSet> set, UniformElement element)
	{
		if (m_Albedo) 
			m_Albedo->Upload(set, element);
	}

	Ref<Asset> MaterialAsset::Copy()
	{
		Ref<MaterialAsset> newAsset = RefHelper::Create<MaterialAsset>();

		newAsset->m_Handle = m_Handle;

		newAsset->m_Path = m_Path;
		newAsset->m_AlbedoPath = m_AlbedoPath;

		newAsset->m_Albedo = m_Albedo->Copy();

		return newAsset;
	}

	Ref<MaterialAsset> MaterialAsset::Create()
	{
		return RefHelper::Create<MaterialAsset>();
	}

	Ref<MaterialAsset> MaterialAsset::Create(const std::filesystem::path& path)
	{
		return RefHelper::Create<MaterialAsset>(path);
	}

}