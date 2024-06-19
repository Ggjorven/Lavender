#include "lvpch.h"
#include "Asset.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/WorkSpace/Assets/AssetManager.hpp"

#include <Flow/Flow.hpp>

namespace Lavender
{

	///////////////////////////////////////////////////////////////////////
	// Utils
	///////////////////////////////////////////////////////////////////////
	template<typename TAsset>
	bool IsExtensionEqualTo(const std::filesystem::path& extension)
	{
		return (TAsset::GetStaticExtension() == extension);
	}

	template<typename... TAsset>
	AssetType GetAssetType(Utils::TypeGroup<TAsset...> group, const std::filesystem::path& extension)
	{
		// Note(Jorben): Empty function for when there are no asset types
		return AssetType::None;
	}

	template<typename FirstAsset, typename ... RestAssets>
	AssetType GetAssetType(Utils::TypeGroup<FirstAsset, RestAssets...> group, const std::filesystem::path& extension)
	{
		if (IsExtensionEqualTo<FirstAsset>(extension))
			return FirstAsset::GetStaticType();
		else
			return GetAssetType<RestAssets...>(Utils::TypeGroup<RestAssets...>(), extension);
	}

	///////////////////////////////////////////////////////////////////////
	// Main Functions
	///////////////////////////////////////////////////////////////////////
	AssetData::AssetData(const std::filesystem::path& file)
		: Name(file.filename().string()), Path(file), Type(GetAssetType(AllAssets(), file.extension()))
	{
	}

	void AssetData::UpdateHandle()
	{
		Flow::Yaml::File file = Flow::Yaml::File(Path, Flow::FileMode::Read);

		if (!file["AssetHandle"])
		{
			APP_LOG_ERROR("File '{0}' doesn't have an AssetHandle stored.", Path.string());
			return;
		}

		Handle = file["AssetHandle"].as<uint64_t>();
	}

	Asset::Asset(const AssetData& data)
		: m_Handle(data.Handle), m_Path(data.Path), m_Name(data.Name)
	{
	}

}