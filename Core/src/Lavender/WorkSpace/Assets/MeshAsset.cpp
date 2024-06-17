#include "lvpch.h"
#include "MeshAsset.hpp"

#include "Lavender/Core/Logging.hpp"

#include <Flow/Flow.hpp>

namespace Lavender
{

	MeshAsset::MeshAsset(const AssetData& data)
		: Asset(data)
	{
	}

	void MeshAsset::Serialize()
	{
		// TODO: ...
	}

	void MeshAsset::Deserialize(const std::filesystem::path& file)
	{
		// TODO: ...
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