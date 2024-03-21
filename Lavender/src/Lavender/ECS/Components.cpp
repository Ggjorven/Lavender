#include "lvpch.h"
#include "Components.hpp"

namespace Lavender
{

	TagComponent::TagComponent(const std::string& str)
		: Tag(str)
	{
	}

	TransformComponent::TransformComponent(const glm::vec3& position, const glm::vec3& size, const glm::vec3& rotation)
		: Position(position), Size(size), Rotation(rotation)
	{
	}

	MeshComponent::MeshComponent(Mesh& mesh)
		: MeshObject(mesh)
	{
	}

}