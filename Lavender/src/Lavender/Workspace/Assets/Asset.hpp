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

		virtual AssetType GetStaticType() const = 0;

	protected:
		AssetHandle m_Handle = {};
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