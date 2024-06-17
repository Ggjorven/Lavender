#pragma once

#include "Lavender/Core/Core.hpp"
#include "Lavender/Utils/Utils.hpp"

#include "Lavender/WorkSpace//WorkSpace.hpp"

#include "Lavender/WorkSpace/Assets/Asset.hpp"
#include "Lavender/WorkSpace/Assets/MaterialAsset.hpp"
#include "Lavender/WorkSpace/Assets/MeshAsset.hpp"

namespace Lavender
{

	struct AssetCollection
	{
	public:
		AssetCollection() = default;
		virtual ~AssetCollection() = default;

		void Add(const AssetHandle& handle, Ref<Asset> asset);
		void Remove(const AssetHandle& handle);

		bool Exists(const AssetHandle& handle) const;

		void CopyCollection(AssetCollection& collection); // Copies this to 'collection'

	private:
		Dict<AssetHandle, Ref<Asset>> m_Assets = { };
	};

	struct AssetCache
	{
	public:
		AssetCache() = default;
		virtual ~AssetCache() = default;

		inline void Add(const AssetData& data) { m_Assets[data.Name] = data; }

		inline Dict<std::string, AssetData>& GetAssets() { return m_Assets; }

	private:
		Dict<std::string, AssetData> m_Assets = { };
	};

	class AssetManager
	{
	public:
		AssetManager() = default;
		virtual ~AssetManager() = default;

		void AddAsset(Ref<Asset> asset);
		void AddToCache(const AssetData& data);

		void UpdateCache(const std::filesystem::path& directory);

		static Ref<AssetManager> Create();

	private:
		AssetCache m_Cache = {};
		Dict<WorkSpace::State, AssetCollection> m_Assets = { };
	};

	typedef Utils::TypeGroup<MaterialAsset, MeshAsset> AllAssets;

}