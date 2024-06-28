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

	Ref<Asset> AssetCollection::GetAsset(const AssetHandle& handle)
	{
		return m_Assets[handle];
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
			APP_LOG_TRACE("[AssetManager] Found asset: {0}", path.string());
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
	void AssetManager::Serialize()
	{
		for (auto& [handle, asset] : m_Assets[WorkSpace::State::Editor].GetAssets())
			asset->Serialize();
	}

	void AssetManager::AddAsset(Ref<Asset> asset)
	{
		if (m_Cache.Exists(asset->GetHandle()))
			m_Cache.Remove(asset->GetHandle());

		m_Assets[Project::Get()->GetState()].Add(asset->GetHandle(), asset);
	}

	void AssetManager::AddToCache(const AssetData& data)
	{
		AssetData newData = data;
		if (newData.Handle == AssetHandle::Empty)
			newData.UpdateData();

		m_Cache.Add(newData);
	}

	void AssetManager::UpdateCache(const std::filesystem::path& directory)
	{
		DeserializeAssets(AllAssets(), this, directory);
	}

	bool AssetManager::Exists(const AssetHandle& handle)
	{
		return (m_Cache.Exists(handle) || m_Assets[Project::Get()->GetState()].Exists(handle));
	}

	bool AssetManager::Loaded(const AssetHandle& handle)
	{
		return m_Assets[Project::Get()->GetState()].Exists(handle);
	}

	Ref<Asset> AssetManager::GetAsset(const AssetHandle& handle)
	{
		if (handle == AssetHandle::Empty)
		{
			APP_LOG_ERROR("Tried to retrieve asset with an empty handle.");
			return nullptr;
		}

		if (!Exists(handle))
		{
			APP_LOG_ERROR("Asset by ID: {0} does not exist.", (uint64_t)handle);
			return nullptr;
		}

		// Load it into memory if not yet loaded
		if (!Loaded(handle))
			Load(m_Cache.GetAssetData(handle));

		return m_Assets[Project::Get()->GetState()].GetAsset(handle);
	}

	AssetCollection& AssetManager::GetCollection()
	{
		return m_Assets[Project::Get()->GetState()];
	}

	Ref<AssetManager> AssetManager::Create()
	{
		return RefHelper::Create<AssetManager>();
	}

	void AssetManager::Load(const AssetData& data)
	{
		auto state = Project::Get()->GetState();
		Ref<Asset> asset = nullptr;

		switch (data.Type)
		{
		case AssetType::Mesh:
			asset = MeshAsset::Create(data);
			break;
		case AssetType::Material:
			asset = MaterialAsset::Create(data);
			break;

		default:
			APP_LOG_ERROR("Invalid AssetType to load.");
			break;
		}

		asset->Deserialize();

		m_Assets[state].Add(data.Handle, asset);
		m_Cache.Remove(data.Handle);
	}

}