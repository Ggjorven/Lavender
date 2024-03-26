#pragma once

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Workspace/Assets/Asset.hpp"

#include "Lavender/Renderer/Image.hpp"
#include "Lavender/Renderer/DescriptorSet.hpp"
#include "Lavender/Renderer/PipelineLayout.hpp"

namespace Lavender
{

	class MaterialAsset : public Asset
	{
	public:
		MaterialAsset() = default;
		MaterialAsset(const std::filesystem::path& path);
		virtual ~MaterialAsset();

		void Serialize() override;
		void Deserialize(const std::filesystem::path& path) override;

		inline void SetAssetPath(const std::filesystem::path& path) override { m_Path = path; }
		inline std::filesystem::path& GetAssetPath() override { return m_Path; }

		void SetAlbedo(Ref<Image2D> image, const std::filesystem::path& path);
		inline Ref<Image2D> GetAlbedo() { return m_Albedo; }

		inline constexpr static std::string GetExtension() { return ".lvmaterial"; }

		void Upload(Ref<DescriptorSet> set, UniformElement element);

		inline constexpr AssetType GetStaticType() const override { return AssetType::MaterialAsset; }

		static Ref<MaterialAsset> Create();
		static Ref<MaterialAsset> Create(const std::filesystem::path& path);

	private:
		std::filesystem::path m_Path = {};

		std::filesystem::path m_AlbedoPath = {};
		Ref<Image2D> m_Albedo = nullptr;
	};

}