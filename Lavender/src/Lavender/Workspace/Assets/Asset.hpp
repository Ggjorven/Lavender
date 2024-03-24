#pragma once

#include <filesystem>

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Utils/UUID.hpp"

namespace Lavender
{

	enum class AssetType
	{
		None = 0, Mesh, Texture, Material
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

	protected:
		AssetHandle m_Handle = {};
	};

}