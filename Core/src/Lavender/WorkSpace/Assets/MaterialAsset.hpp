#pragma once

#include "Lavender/Core/Core.hpp"
#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Renderer/Material.hpp"

#include "Lavender/WorkSpace/Assets/Asset.hpp"

namespace Lavender
{

	class MaterialAsset : public Asset
	{
	public:
		MaterialAsset(const AssetData& data);
		virtual ~MaterialAsset() = default;

		void Serialize() override;
		void Deserialize(const std::filesystem::path& file) override;

		inline Ref<Material> GetMaterial() { return m_Material; }
		inline void SetMaterial(Ref<Material> material) { m_Material = material; }

		static Ref<MaterialAsset> Create(const AssetData& data);

		inline static AssetType GetStaticType() { return AssetType::Material; }
		inline virtual AssetType GetType() const { return GetStaticType(); }
		inline static std::string GetStaticExtension() { return ".lvmaterial"; }
		inline std::string GetExtension() const override { return GetStaticExtension(); }

		Ref<Asset> Copy() override;

	private:
		Ref<Material> m_Material = nullptr;
	};

}