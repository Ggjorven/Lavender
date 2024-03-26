#pragma once

#include <string>

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

		bool Exists(AssetHandle handle) const;
		Ref<Asset> GetAsset(AssetHandle handle);
		inline Dict<AssetHandle, Ref<Asset>> GetAssets() { return m_Assets; }

		static Ref<AssetManager> Create();

	private:
		Dict<AssetHandle, Ref<Asset>> m_Assets = { };
	};

}