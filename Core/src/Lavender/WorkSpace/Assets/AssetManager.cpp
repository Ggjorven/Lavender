#include "lvpch.h"
#include "AssetManager.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/WorkSpace/Project.hpp"

namespace Lavender
{

	/////////////////////////////////////////////////////////////////////////
	// Collection & Cache
	/////////////////////////////////////////////////////////////////////////
	void AssetCollection::Add(const AssetHandle& handle, Ref<Asset> asset)
	{
		m_Assets[handle] = asset;
	}

	void AssetCollection::Remove(const AssetHandle& handle)
	{
		if (Exists(handle))
			m_Assets.erase(handle);
	}

	bool AssetCollection::Exists(const AssetHandle& handle) const
	{
		return (m_Assets.find(handle) != m_Assets.end());
	}

	void AssetCollection::CopyCollection(AssetCollection& collection)
	{
		for (auto& [handle, asset] : m_Assets)
			collection.Add(handle, asset->Copy());
	}

	/////////////////////////////////////////////////////////////////////////
	// Utils
	/////////////////////////////////////////////////////////////////////////
	static std::vector<std::filesystem::path> GetPathsFromExtension(const std::filesystem::path& directory, const std::string& extension)
	{
		std::vector<std::filesystem::path> result = { };

		if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory))
		{
			APP_LOG_ERROR("Directory '{0}' does not exist...", directory.string());
			return result;
		}

		for (const auto& entry : std::filesystem::recursive_directory_iterator(directory))
		{
			if (entry.is_regular_file() && entry.path().extension() == extension)
				result.push_back(entry.path());
		}

		return result;
	}

	template<typename TAsset>
	void DeserializeAsset(AssetManager* instance, const std::filesystem::path& directory)
	{
		// If this doesn't work make sure all asset paths are included
		for (auto path : GetPathsFromExtension(directory, TAsset::GetStaticExtension()))
		{
			instance->AddToCache({ path });
			APP_LOG_TRACE("[AssetManager] Found path: {0}", path.string());
		}
	}

	template<typename... TAsset>
	void DeserializeAssets(Utils::TypeGroup<TAsset...> group, AssetManager* instance, const std::filesystem::path& directory)
	{
		// Note(Jorben): Empty function for when there are no asset types
	}

	template<typename FirstAsset, typename ... RestAssets>
	void DeserializeAssets(Utils::TypeGroup<FirstAsset, RestAssets...> group, AssetManager* instance, const std::filesystem::path& directory)
	{
		DeserializeAsset<FirstAsset>(instance, directory);
		DeserializeAssets<RestAssets...>(Utils::TypeGroup<RestAssets...>(), instance, directory);
	}

	/////////////////////////////////////////////////////////////////////////
	// Main Functions
	/////////////////////////////////////////////////////////////////////////
	void AssetManager::AddAsset(Ref<Asset> asset)
	{
		m_Assets[Project::Get()->GetState()].Add(asset->GetHandle(), asset);
	}

	void AssetManager::AddToCache(const AssetData& data)
	{
		m_Cache.Add(data);
	}

	void AssetManager::UpdateCache(const std::filesystem::path& directory)
	{
		DeserializeAssets(AllAssets(), this, directory);
	}

	Ref<AssetManager> AssetManager::Create()
	{
		return RefHelper::Create<AssetManager>();
	}

}