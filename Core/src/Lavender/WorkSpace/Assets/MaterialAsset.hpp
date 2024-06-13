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

		inline virtual AssetType GetStaticType() const { return AssetType::Material; }

	private:
		Ref<Material> m_Material = nullptr;
	};

}