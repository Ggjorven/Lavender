#include "lvpch.h"
#include "MaterialAsset.hpp"

#include "Lavender/Core/Logging.hpp"

namespace Lavender
{

	MaterialAsset::MaterialAsset(const AssetData& data)
		: Asset(data)
	{
	}

	void MaterialAsset::Serialize()
	{
		// TODO: ...
	}

	void MaterialAsset::Deserialize(const std::filesystem::path& file)
	{
		// TODO: ...
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