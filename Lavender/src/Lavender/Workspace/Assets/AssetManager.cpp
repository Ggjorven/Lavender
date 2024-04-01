#include "lvpch.h"
#include "AssetManager.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Workspace/Assets/MeshAsset.hpp"
#include "Lavender/Workspace/Assets/MaterialAsset.hpp"

namespace Lavender
{

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Helper functions
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	static std::vector<std::filesystem::path> GetPathsFromExtension(const std::filesystem::path& directory, const std::string& extension)
	{
		std::vector<std::filesystem::path> result = { };

		if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory)) 
		{
			LV_LOG_ERROR("Directory '{0}' does not exist...", directory.string());
			return result;
		}

		for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) 
		{
			if (entry.is_regular_file() && entry.path().extension() == extension)
			{
				result.push_back(entry.path());
				LV_LOG_TRACE("Loaded asset: {0}", entry.path().string());
			}
		}

		return result;
	}

	template<typename TAsset>
	void DeserializeAsset(AssetManager* instance, const std::filesystem::path& assetPath)
	{
		// If this doesn't work make sure all asset paths are included
		for (auto path : GetPathsFromExtension(assetPath, TAsset::GetExtension())) 
		{
			Ref<TAsset> asset = TAsset::Create(path);
			instance->AddAsset(asset);
		}
	}

	template<typename... TAsset>
	void DeserializeAssets(AssetGroup<TAsset...> group, AssetManager* instance, const std::filesystem::path& assetPath)
	{
		// Note(Jorben): Empty function for when there are no asset types
	}

	template<typename FirstAsset, typename ... RestAssets>
	void DeserializeAssets(AssetGroup<FirstAsset, RestAssets...> group, AssetManager* instance, const std::filesystem::path& assetPath)
	{
		DeserializeAsset<FirstAsset>(instance, assetPath);
		DeserializeAssets<RestAssets...>(AssetGroup<RestAssets...>(), instance, assetPath);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Main functions
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	AssetManager::AssetManager()
	{
	}

	AssetManager::~AssetManager()
	{
		m_PrimaryAssets.clear();
		m_SecondaryAssets.clear();
	}

	void AssetManager::Serialize()
	{
		for (auto& asset : m_PrimaryAssets)
			asset.second->Serialize();
	}

	void AssetManager::GetAssetsFromDirectory(const std::filesystem::path& directory)
	{
		Utils::Timer time = {};
		LV_LOG_TRACE("Started timer for AssetManager");
		DeserializeAssets(AllAssets(), this, directory);
		LV_LOG_TRACE("Time passed: {0}", time.GetPassedTime());
	}

	void AssetManager::AddAsset(Ref<Asset> asset)
	{
		if (m_PrimaryIsPrimary)
			m_PrimaryAssets[asset->GetHandle()] = asset;
		else
			m_SecondaryAssets[asset->GetHandle()] = asset;
	}

	void AssetManager::SwitchAssets()
	{
		if (m_PrimaryIsPrimary)
		{
			for (auto& asset : m_PrimaryAssets)
				m_SecondaryAssets[asset.first] = asset.second->Copy();

			m_PrimaryIsPrimary = false;
		}
		else
		{
			m_SecondaryAssets.clear();
			m_PrimaryIsPrimary = true;
		}
	}

	bool AssetManager::Exists(AssetHandle handle) const
	{
		return m_PrimaryIsPrimary ? m_PrimaryAssets.find(handle) != m_PrimaryAssets.end() : m_SecondaryAssets.find(handle) != m_SecondaryAssets.end();
	}

	Ref<Asset> AssetManager::GetAsset(AssetHandle handle)
	{
		return m_PrimaryIsPrimary ? m_PrimaryAssets[handle] : m_SecondaryAssets[handle];
	}

	Ref<AssetManager> AssetManager::Create()
	{
		return RefHelper::Create<AssetManager>();
	}

}