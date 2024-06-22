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

	public:
		AssetData() = default;
		AssetData(const std::filesystem::path& file);

		void UpdateData(); // Retrieves AssetHandle from file
	};

	class Asset
	{
	public:
		Asset(const AssetData& data);
		virtual ~Asset() = default;

		virtual void Serialize() = 0; // Must serialize AssetHandle: *id* globally
		inline void Deserialize() { Deserialize(m_Path); }
		virtual void Deserialize(const std::filesystem::path& file) = 0;

		inline void SetHandle(const AssetHandle& handle) { m_Handle = handle; }
		inline AssetHandle GetHandle() { return m_Handle; }

		inline std::string& GetName() { return m_Name; }
		inline std::filesystem::path GetPath() { return m_Path; }

		inline AssetData GetAssetData() const
		{
			AssetData data = {};
			data.Name = m_Name;
			data.Path = m_Path;
			data.Handle = m_Handle;

			return data;
		}

		inline static AssetType GetStaticType() { return AssetType::None; }
		inline virtual AssetType GetType() const { return GetStaticType(); }
		inline static std::string GetStaticExtension() { return ".lvasset"; } // Default handle
		inline virtual std::string GetExtension() const { return GetStaticExtension(); }

		virtual Ref<Asset> Copy() = 0; // Copies this to the return value

	protected:
		AssetHandle m_Handle = AssetHandle::Empty;

		std::string m_Name = {};
		std::filesystem::path m_Path = {};
	};

}