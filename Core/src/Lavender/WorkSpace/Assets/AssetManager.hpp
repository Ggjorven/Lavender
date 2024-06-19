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

		Ref<Asset> GetAsset(const AssetHandle& handle);

		void CopyCollection(AssetCollection& collection); // Copies this into 'collection'

	private:
		Dict<AssetHandle, Ref<Asset>> m_Assets = { };
	};

	struct AssetCache
	{
	public:
		AssetCache() = default;
		virtual ~AssetCache() = default;

		inline void Add(const AssetData& data) { m_Assets[data.Handle] = data; }
		inline void Remove(const AssetHandle& handle) { m_Assets.erase(handle); }

		inline bool Exists(const AssetHandle& handle) const { return (m_Assets.find(handle) != m_Assets.end()); }

		inline bool Empty() const { return m_Assets.empty(); }

		inline AssetData GetAssetData(const AssetHandle& handle) { return m_Assets[handle]; }
		inline Dict<AssetHandle, AssetData>& GetAssets() { return m_Assets; }

	private:
		Dict<AssetHandle, AssetData> m_Assets = { };
	};

	class AssetManager
	{
	public:
		AssetManager() = default;
		virtual ~AssetManager() = default;

		// !TODO: Serialize

		void AddAsset(Ref<Asset> asset);
		void AddToCache(const AssetData& data);

		void UpdateCache(const std::filesystem::path& directory);

		bool Exists(const AssetHandle& handle);
		bool Loaded(const AssetHandle& handle); 

		Ref<Asset> GetAsset(const AssetHandle& handle);

		static Ref<AssetManager> Create();

	private:
		void Load(const AssetData& data);

	private:
		AssetCache m_Cache = {};
		Dict<WorkSpace::State, AssetCollection> m_Assets = { };
	};

	typedef Utils::TypeGroup<MaterialAsset, MeshAsset> AllAssets;

}