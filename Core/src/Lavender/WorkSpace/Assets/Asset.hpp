#pragma once

#include "Lavender/Core/Core.hpp"
#include "Lavender/Utils/Utils.hpp"
#include "Lavender/Utils/UUID.hpp"

namespace Lavender
{

	typedef UUID AssetHandle;

	enum class AssetType
	{
		None = 0, Mesh, Material
	};

	struct AssetData
	{
	public:
		std::string Name = {};
		std::filesystem::path Path = {};
		
		AssetHandle Handle = AssetHandle::Empty;
		AssetType Type = AssetType::None;
	};

	class Asset
	{
	public:
		Asset(const AssetData& data);
		virtual ~Asset() = default;

		virtual void Serialize() = 0;
		virtual void Deserialize(const std::filesystem::path& file) = 0;

		inline void SetHandle(const AssetHandle& handle) { m_Handle = handle; }
		inline AssetHandle GetHandle() { return m_Handle; }

		inline std::string& GetName() { return m_Name; }
		inline std::filesystem::path GetPath() { return m_Path; }

		inline AssetData GetAssetData() const
		{
			AssetData data = {};
			data.Handle = m_Handle;
			data.Path = m_Path;

			return data;
		}

		inline virtual AssetType GetStaticType() const { return AssetType::None; }

	protected:
		AssetHandle m_Handle = AssetHandle::Empty;

		std::string m_Name = {};
		std::filesystem::path m_Path = {};
	};

}