#include "Components.hpp"

namespace Lavender
{

	// Note(Jorben): Has to be kept up to date with the original Ones

	TagComponent::TagComponent(const std::string& str)
		: Tag(str)
	{
	}

	TransformComponent::TransformComponent(const glm::vec3& position, const glm::vec3& size, const glm::vec3& rotation)
		: Position(position), Size(size), Rotation(rotation)
	{
	}

	MeshComponent::MeshComponent(Ref<MeshAsset> mesh, Ref<MaterialAsset> material)
		: MeshObject(mesh), Material(material)
	{
	}

	MeshComponent::MeshComponent(const MeshComponent& other) = default;

	ScriptComponent::ScriptComponent(const std::string& name)
		: ClassName(name)
	{
	}

}