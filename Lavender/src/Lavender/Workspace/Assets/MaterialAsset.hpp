#pragma once

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Workspace/Assets/Asset.hpp"

#include "Lavender/Renderer/Image.hpp"
#include "Lavender/Renderer/DescriptorSet.hpp"
#include "Lavender/Renderer/PipelineLayout.hpp"

#include <glm/glm.hpp>

namespace Lavender
{

	struct ShaderMaterial;

	class MaterialAsset : public Asset
	{
	public:
		MaterialAsset() = default;
		MaterialAsset(const std::filesystem::path& path, bool forceLoad = false);
		virtual ~MaterialAsset();

		void Serialize() override;
		void Deserialize(const std::filesystem::path& path) override;

		inline void SetAssetPath(const std::filesystem::path& path) override { m_Path = path; }
		inline std::filesystem::path& GetAssetPath() override { return m_Path; }

		void Upload(Ref<Pipeline> pipeline, Ref<DescriptorSet> set, Ref<Image2D> emptyImage);

		inline constexpr AssetType GetStaticType() const override { return AssetType::MaterialAsset; }
		inline constexpr static std::string GetExtension() { return ".lvmaterial"; }

		Ref<Asset> Copy() override;
		ShaderMaterial AsShaderMaterial();

		static Ref<MaterialAsset> Create();
		static Ref<MaterialAsset> Create(const std::filesystem::path& path);

	private:
		void LoadUUID();

	public:
		std::filesystem::path AlbedoPath = {};
		Ref<Image2D> AlbedoImage = nullptr;
		glm::vec4 AlbedoColour = { 1.0f, 1.0f, 1.0f, 1.0f };

		std::filesystem::path SpecularPath = {};
		Ref<Image2D> SpecularImage = nullptr;
		glm::vec4 SpecularColour = { 0.0f, 0.0f, 0.0f, 1.0f };

		float Shininess = 0.5f;

	private:
		std::filesystem::path m_OriginalPath = {};
		std::filesystem::path m_Path = {};
	};

	struct ShaderMaterial
	{
	public:
		glm::vec4 AlbedoColour = { 1.0f, 1.0f, 1.0f, 1.0f };
		glm::vec4 SpecularColour = { 0.0f, 0.0f, 0.0f, 1.0f };
		float Shininess = 0.5f;
	};

}