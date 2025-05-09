#pragma once

#include <typeinfo>
#include <type_traits>
#include <filesystem>

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Utils/UUID.hpp"

namespace Lavender
{

	enum class AssetType
	{
		None = 0, MeshAsset, MaterialAsset
	};

	typedef UUID AssetHandle;

	struct AssetMetaData
	{
	public:
		AssetHandle Handle = AssetHandle::Empty;
		AssetType Type = AssetType::None;

		std::filesystem::path Path = {};
	};

	class Asset
	{
	public:
		Asset();
		virtual ~Asset() = default;

		virtual void SetAssetPath(const std::filesystem::path& path) = 0;
		virtual std::filesystem::path& GetAssetPath() = 0;

		virtual void Serialize() = 0;
		virtual void Deserialize(const std::filesystem::path& path) = 0;

		inline AssetHandle GetHandle() { return m_Handle; }
		inline void SetHandle(AssetHandle handle) { m_Handle = handle; }

		inline bool IsLoaded() const { return m_Loaded; }
		inline void SetLoaded(bool loaded) { m_Loaded = loaded; }

		virtual AssetType GetStaticType() const = 0;

		virtual Ref<Asset> Copy() = 0;

	protected:
		AssetHandle m_Handle = {};
		bool m_Loaded = false;
	};
	
	class MeshAsset;
	class MaterialAsset;

	static std::string AssetTypeToString(AssetType type)
	{
		switch (type)
		{
		case AssetType::MeshAsset:
			return "MeshAsset";
		case AssetType::MaterialAsset:
			return "MaterialAsset";
		}

		return "Undefined Asset";
	}

	template<typename... Assets>
	struct AssetGroup
	{
	};

	using AllAssets = AssetGroup<MeshAsset, MaterialAsset>;

}