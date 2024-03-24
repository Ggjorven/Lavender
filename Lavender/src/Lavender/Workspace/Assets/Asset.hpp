#pragma once

#include <filesystem>

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Utils/UUID.hpp"

namespace Lavender
{

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

	protected:
		AssetHandle m_Handle = {};
	};
	
	class MeshAsset;

	template<typename... Assets>
	struct AssetGroup
	{
	};

	using AllAssets = AssetGroup<MeshAsset>;

}