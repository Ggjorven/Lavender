#include "lvpch.h"
#include "Mesh.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/Renderer.hpp"

namespace Lavender
{

	Mesh::Mesh(const std::filesystem::path& path)
	{
		std::vector<MeshVertex> vertices = { };
		std::vector<uint32_t> indices = { };

		LoadModel(path, vertices, indices);

		m_VertexBuffer = std::make_shared<VertexBuffer>((void*)vertices.data(), sizeof(vertices[0]) * vertices.size());
		m_IndexBuffer = std::make_shared<IndexBuffer>(indices.data(), (uint32_t)indices.size());
	}

	Mesh::~Mesh()
	{
	}

	void Mesh::LoadModel(const std::filesystem::path& path, std::vector<MeshVertex>& vertices, std::vector<uint32_t>& indices)
	{
		Assimp::Importer importer;

		int flags = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace;
		const aiScene* scene = importer.ReadFile(path.string(), flags);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			LV_LOG_ERROR("Failed to load mesh from: \"{}\"", path.string());
			return;
		}

		ProcessNode(scene->mRootNode, scene, vertices, indices);
	}

	void Mesh::ProcessNode(aiNode* node, const aiScene* scene, std::vector<MeshVertex>& vertices, std::vector<uint32_t>& indices)
	{
		// Process all the node's meshes
		for (uint32_t i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			ProcessMesh(mesh, scene, vertices, indices);
		}

		// Then do the same for each of its children
		for (uint32_t i = 0; i < node->mNumChildren; i++)
			ProcessNode(node->mChildren[i], scene, vertices, indices);
	}

	void Mesh::ProcessMesh(aiMesh* mesh, const aiScene* scene, std::vector<MeshVertex>& vertices, std::vector<uint32_t>& indices)
	{
		// Vertex processing
		for (uint32_t i = 0; i < mesh->mNumVertices; i++)
		{
			MeshVertex vertex;
			glm::vec3 vector(0.0f);

			// Position
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;

			// Texture coordinates
			if (mesh->mTextureCoords[0])
			{
				glm::vec2 vec(0.0f);
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoord = vec;
			}
			else
				vertex.TexCoord = glm::vec2(0.0f, 0.0f);

			vertices.push_back(vertex);
		}

		// Index processing
		for (uint32_t i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (uint32_t j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}
	}

}