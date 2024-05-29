#pragma once

#include <filesystem>

#include "Lavender/Utils/Utils.hpp"

#include <glm/glm.hpp>

#include "Lavender/Renderer/VertexBuffer.hpp"
#include "Lavender/Renderer/IndexBuffer.hpp"
#include "Lavender/Renderer/PipelineLayout.hpp"

namespace Lavender
{

	struct MeshVertex
	{
	public:
		glm::vec3 Position = { };
		glm::vec2 TexCoord = { };
		glm::vec3 Normal = { };

	public:
		static BufferLayout GetLayout();
	};

	class Mesh
	{
	public:
		Mesh() = default;
		Mesh(const std::filesystem::path& path);
		virtual ~Mesh();

		Ref<VertexBuffer>& GetVertexBuffer() { return m_VertexBuffer; }
		Ref<IndexBuffer>& GetIndexBuffer() { return m_IndexBuffer; }

		Ref<Mesh> Copy();

		static Ref<Mesh> Create(const std::filesystem::path& path);

	private:
		Ref<VertexBuffer> m_VertexBuffer = nullptr;
		Ref<IndexBuffer> m_IndexBuffer = nullptr;
		
	};

}