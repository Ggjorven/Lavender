#include "lvpch.h"
#include "Components.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Workspace/Project.hpp"

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

	MeshComponent::MeshComponent(Ref<MeshAsset> mesh, Ref<MaterialAsset> material)
		: MeshObject(mesh), Material(material)
	{
	}

	MeshComponent::MeshComponent(const MeshComponent& other)
	{
		auto assetmanager = Project::Get()->GetSceneCollection().GetActive()->GetAssetManager();
		if (MeshObject) MeshObject = RefHelper::RefAs<MeshAsset>(assetmanager->GetAsset(MeshObject->GetHandle()));
		if (Material) Material = RefHelper::RefAs<MaterialAsset>(assetmanager->GetAsset(Material->GetHandle()));
	}

	ScriptComponent::ScriptComponent(const std::string& name)
		: ClassName(name)
	{
	}

	DirectionalLightComponent::DirectionalLightComponent(const glm::vec3& direction, const glm::vec4& colour, float intensity)
		: DirectionalLightComponent(colour, direction, intensity)
	{
	}

	DirectionalLightComponent::DirectionalLightComponent(const glm::vec4& colour, const glm::vec3& direction, float intensity)
		: Colour(colour), Direction(direction), Intensity(intensity)
	{
	}

}