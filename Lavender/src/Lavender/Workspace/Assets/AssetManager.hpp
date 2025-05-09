#pragma once

#include <string>
#include <mutex>

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Workspace/Assets/Asset.hpp"

namespace Lavender
{

	class AssetManager
	{
	public:
		AssetManager();
		virtual ~AssetManager();

		void Serialize();
		void GetAssetsFromDirectory(const std::filesystem::path& directory);

		void AddAsset(Ref<Asset> asset);

		void SwitchAssets();

		bool Exists(AssetHandle handle) const;
		Ref<Asset> GetAsset(AssetHandle handle);
		inline Dict<AssetHandle, Ref<Asset>> GetAssets() { return m_PrimaryIsPrimary ? m_PrimaryAssets : m_SecondaryAssets; }

		static Ref<AssetManager> Create();

	private:
		void LoadAsset(Ref<Asset> asset);

	private:
		Dict<AssetHandle, Ref<Asset>> m_PrimaryAssets = { };
		Dict<AssetHandle, Ref<Asset>> m_SecondaryAssets = { };
		bool m_PrimaryIsPrimary = true;
	};

}