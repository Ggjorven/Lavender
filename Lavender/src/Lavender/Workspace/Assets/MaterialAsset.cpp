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

		// Albedo
		data << YAML::Key << "AlbedoImage";
		data << YAML::Value << AlbedoPath.string();
		data << YAML::Key << "AlbedoColour";
		data << YAML::Value << AlbedoColour;

		// Specular
		data << YAML::Key << "SpecularImage";
		data << YAML::Value << SpecularPath.string();
		data << YAML::Key << "SpecularColour";
		data << YAML::Value << SpecularColour;

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
			// Albedo
			auto albedoImage = metadata["AlbedoImage"];
			if (albedoImage)
			{
				AlbedoPath = std::filesystem::path(albedoImage.as<std::string>());
				auto path = Project::Get()->GetDirectories().ProjectDir / Project::Get()->GetDirectories().Assets / AlbedoPath;

				if (std::filesystem::exists(path) && !path.filename().empty())
				{
					ImageSpecification specs = {};
					specs.Usage = ImageSpecification::ImageUsage::File;
					specs.Flags = ImageSpecification::ImageUsageFlags::Sampled;
					specs.Path = path;
					AlbedoImage = Image2D::Create(specs);
				}
				else
					LV_LOG_ERROR("(Material) Albedo path: '{0}' doesn't exist.", AlbedoPath.string());
			}
			auto albedoColour = metadata["AlbedoColour"];
			if (albedoColour)
			{
				AlbedoColour = albedoColour.as<glm::vec4>();
			}

			// Specular
			auto specularImage = metadata["SpecularImage"];
			if (specularImage)
			{
				SpecularPath = std::filesystem::path(specularImage.as<std::string>());
				auto path = Project::Get()->GetDirectories().ProjectDir / Project::Get()->GetDirectories().Assets / SpecularPath;

				if (std::filesystem::exists(path) && !path.filename().empty())
				{
					ImageSpecification specs = {};
					specs.Usage = ImageSpecification::ImageUsage::File;
					specs.Flags = ImageSpecification::ImageUsageFlags::Sampled;
					specs.Path = path;
					AlbedoImage = Image2D::Create(specs);
				}
				else
					LV_LOG_ERROR("(Material) Specular path: '{0}' doesn't exist.", SpecularPath.string());
			}
			auto specularColour = metadata["SpecularColour"];
			if (specularColour)
			{
				SpecularColour = specularColour.as<glm::vec4>();
			}
		}
	}

	void MaterialAsset::Upload(Ref<Pipeline> pipeline, Ref<DescriptorSet> set, Ref<Image2D> emptyImage)
	{
		if (AlbedoImage)
			AlbedoImage->Upload(set, pipeline->GetSpecification().Uniformlayout.GetElementByName(0, "u_AlbedoImage"));
		else
			emptyImage->Upload(set, pipeline->GetSpecification().Uniformlayout.GetElementByName(0, "u_AlbedoImage"));

		if (SpecularImage)
			SpecularImage->Upload(set, pipeline->GetSpecification().Uniformlayout.GetElementByName(0, "u_SpecularImage"));
		else
			emptyImage->Upload(set, pipeline->GetSpecification().Uniformlayout.GetElementByName(0, "u_SpecularImage"));
	}

	Ref<Asset> MaterialAsset::Copy()
	{
		Ref<MaterialAsset> newAsset = RefHelper::Create<MaterialAsset>();

		newAsset->m_Handle = m_Handle;

		newAsset->m_Path = m_Path;
		newAsset->AlbedoPath = AlbedoPath;

		newAsset->AlbedoImage = AlbedoImage->Copy();

		return newAsset;
	}

	ShaderMaterial MaterialAsset::AsShaderMaterial()
	{
		ShaderMaterial material = {};
		material.AlbedoColour = AlbedoColour;
		material.SpecularColour = SpecularColour;
		material.Shininess = Shininess;

		return material;
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