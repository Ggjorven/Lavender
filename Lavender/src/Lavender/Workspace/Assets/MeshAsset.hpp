#pragma once

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Workspace/Assets/Asset.hpp"

#include "Lavender/Renderer/Mesh.hpp"

namespace Lavender
{

	class MeshAsset : public Asset
	{
	public:
		MeshAsset() = default;
		MeshAsset(const std::filesystem::path& path);
		MeshAsset(const std::filesystem::path& path, const std::filesystem::path& meshPath);
		virtual ~MeshAsset();

		void Serialize() override;
		void Deserialize(const std::filesystem::path& path) override;

		inline void SetAssetPath(const std::filesystem::path& path) override { m_Path = path; }
		inline std::filesystem::path& GetAssetPath() override { return m_Path; }

		inline Ref<Mesh> GetMesh() { return m_Mesh; }

		inline constexpr static std::string GetExtension() { return ".lvmesh"; }

		static Ref<MeshAsset> Create();
		static Ref<MeshAsset> Create(const std::filesystem::path& path);
		static Ref<MeshAsset> Create(const std::filesystem::path& path, const std::filesystem::path& meshPath);

	private:
		std::filesystem::path m_Path = {};
		std::filesystem::path m_MeshPath = {};

		Ref<Mesh> m_Mesh = nullptr;
	};

}