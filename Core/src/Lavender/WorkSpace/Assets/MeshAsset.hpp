#pragma once

#include "Lavender/Core/Core.hpp"
#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Renderer/Mesh.hpp"

#include "Lavender/WorkSpace/Assets/Asset.hpp"

namespace Lavender
{

	class MeshAsset : public Asset
	{
	public:
		MeshAsset(const AssetData& data);
		virtual ~MeshAsset() = default;

		void Serialize() override;
		void Deserialize(const std::filesystem::path& file) override;

		inline Ref<Mesh> GetMesh() { return m_Mesh; }

		inline virtual AssetType GetStaticType() const { return AssetType::Mesh; }

	private:
		Ref<Mesh> m_Mesh = nullptr;
		std::filesystem::path m_MeshPath = {};
	};

}