#pragma once

#include <filesystem>

#include <assimp/Importer.hpp>   
#include <assimp/scene.h>        
#include <assimp/postprocess.h>  
#include <glm/glm.hpp>

#include <vulkan/vulkan.h>

#include "Lavender/Renderer/VertexBuffer.hpp"
#include "Lavender/Renderer/IndexBuffer.hpp"

namespace Lavender
{

	struct MeshVertex
	{
	public:
		glm::vec3 Position = { };
		glm::vec2 TexCoord = { };

	};

	class Mesh
	{
	public:
		Mesh() = default;
		Mesh(const std::filesystem::path& path);
		virtual ~Mesh();

		std::shared_ptr<VertexBuffer>& GetVertexBuffer() { return m_VertexBuffer; }
		std::shared_ptr<IndexBuffer>& GetIndexBuffer() { return m_IndexBuffer; }

	private:
		void LoadModel(const std::filesystem::path& path, std::vector<MeshVertex>& vertices, std::vector<uint32_t>& indices);
		void ProcessNode(aiNode* node, const aiScene* scene, std::vector<MeshVertex>& vertices, std::vector<uint32_t>& indices);
		void ProcessMesh(aiMesh* mesh, const aiScene* scene, std::vector<MeshVertex>& vertices, std::vector<uint32_t>& indices);

	private:
		std::shared_ptr<VertexBuffer> m_VertexBuffer = nullptr;
		std::shared_ptr<IndexBuffer> m_IndexBuffer = nullptr;
	};

}
