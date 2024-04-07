#include "lvpch.h"
#include "Mesh.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/Renderer.hpp"

namespace Lavender
{

	BufferLayout MeshVertex::GetLayout()
	{
		static BufferLayout layout = {
			BufferElement(DataType::Float3, 0, "a_Position"),
			BufferElement(DataType::Float2, 1, "a_TexCoord"),
			BufferElement(DataType::Float3, 2, "a_Normal")
		};

		return layout;
	}

	Mesh::Mesh(const std::filesystem::path& path)
	{
		std::vector<MeshVertex> vertices = { };
		std::vector<uint32_t> indices = { };

		LoadModel(path, vertices, indices);

		m_VertexBuffer = VertexBuffer::Create((void*)vertices.data(), sizeof(vertices[0]) * vertices.size());
		m_IndexBuffer = IndexBuffer::Create(indices.data(), (uint32_t)indices.size());
	}

	Mesh::~Mesh()
	{
	}

	Ref<Mesh> Mesh::Copy()
	{
		Ref<Mesh> newMesh = RefHelper::Create<Mesh>();

		newMesh->m_VertexBuffer = m_VertexBuffer->Copy();
		newMesh->m_IndexBuffer = m_IndexBuffer->Copy();

		return newMesh;
	}

	Ref<Mesh> Mesh::Create(const std::filesystem::path& path)
	{
		return RefHelper::Create<Mesh>(path);
	}

	void Mesh::LoadModel(const std::filesystem::path& path, std::vector<MeshVertex>& vertices, std::vector<uint32_t>& indices)
	{
		Assimp::Importer importer = {};

		uint32_t flags = aiProcess_Triangulate | aiProcess_CalcTangentSpace;
		if (Renderer::GetAPI() == RenderingAPI::Vulkan) 
			flags |= aiProcess_FlipUVs;

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
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			ProcessMesh(mesh, scene, vertices, indices);
		}

		// Then do the same for each of its children
		for (unsigned int i = 0; i < node->mNumChildren; i++)
			ProcessNode(node->mChildren[i], scene, vertices, indices);
	}

	void Mesh::ProcessMesh(aiMesh* mesh, const aiScene* scene, std::vector<MeshVertex>& vertices, std::vector<uint32_t>& indices)
	{
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			MeshVertex vertex = {};
			glm::vec3 vector(0.0f);

			// Position
			vertex.Position.x = mesh->mVertices[i].x;
			vertex.Position.y = mesh->mVertices[i].y;
			vertex.Position.z = mesh->mVertices[i].z;

			// Texture coordinates
			if (mesh->HasTextureCoords(0))
			{
				glm::vec2 vec(0.0f);
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoord = vec;
			}
			else
				vertex.TexCoord = glm::vec2(0.0f, 0.0f);

			// Normals
			if (mesh->HasNormals())
			{
				vertex.Normal.x = mesh->mNormals[i].x;
				vertex.Normal.y = mesh->mNormals[i].y;
				vertex.Normal.z = mesh->mNormals[i].z;
			}
			else
				vertex.Normal = glm::vec3(0.0f, 0.0f, 0.0f);

			vertices.push_back(vertex);
		}

		// Index processing
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}
	}

}