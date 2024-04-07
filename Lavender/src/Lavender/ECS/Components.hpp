#pragma once

#include <string>
#include <typeinfo>
#include <type_traits>

#include <glm/glm.hpp>

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Workspace/Assets/Asset.hpp"
#include "Lavender/Workspace/Assets/MeshAsset.hpp"
#include "Lavender/Workspace/Assets/MaterialAsset.hpp"

namespace Lavender
{

	struct TagComponent
	{
	public:
		std::string Tag = "Unnamed Entity";

	public:
		TagComponent() = default;
		TagComponent(const std::string& str);
		TagComponent(const TagComponent& other) = default;
	};

	struct TransformComponent
	{
	public:
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Size = { 1.0f, 1.0f, 1.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };

	public:
		TransformComponent() = default;
		TransformComponent(const glm::vec3& position, const glm::vec3& size, const glm::vec3& rotation);
		TransformComponent(const TransformComponent& other) = default;
	};

	struct MeshComponent
	{
	public:
		Ref<MeshAsset> MeshObject = nullptr; 
		Ref<MaterialAsset> Material = nullptr;

	public:
		MeshComponent() = default;
		MeshComponent(Ref<MeshAsset> mesh, Ref<MaterialAsset> material);
		MeshComponent(const MeshComponent& other);
	};

	struct ScriptComponent
	{
	public:
		std::string ClassName = {};

	public:
		ScriptComponent() = default;
		ScriptComponent(const std::string& name);
		ScriptComponent(const ScriptComponent& other) = default;
	};

	struct DirectionalLightComponent // TODO: Implement for scripting
	{
	public:
		glm::vec4 Colour = { 1.0f, 0.0f, 0.0f, 1.0f };
		glm::vec3 Direction = { 0.0f, 0.0f, 0.0f };
		//PUBLIC_PADDING(0, 4)
		float Intensity = 1.0f;
		//PUBLIC_PADDING(1, 12)

	public:
		DirectionalLightComponent() = default;
		DirectionalLightComponent(const glm::vec3& direction, const glm::vec4& colour, float intensity);
		DirectionalLightComponent(const glm::vec4& colour, const glm::vec3& direction, float intensity);
		DirectionalLightComponent(const DirectionalLightComponent& other) = default;
	};

	enum class Component
	{
		None = 0, Tag, Transform, Mesh, DirectionalLight
	};

	template<typename TComponent>
	static Component GetComponentType()
	{
		if (typeid(TComponent) == typeid(TagComponent))
			return Component::Tag;
		else if (typeid(TComponent) == typeid(TransformComponent))
			return Component::Transform;
		else if (typeid(TComponent) == typeid(MeshComponent))
			return Component::Mesh;
		else if (typeid(TComponent) == typeid(DirectionalLightComponent))
			return Component::DirectionalLight;

		return Component::None;
	}

	template<typename TComponent>
	static std::string ComponentToString()
	{
		if (typeid(TComponent) == typeid(TagComponent))
			return "TagComponent";
		else if (typeid(TComponent) == typeid(TransformComponent))
			return "TransformComponent";
		else if (typeid(TComponent) == typeid(MeshComponent))
			return "MeshComponent";
		else if (typeid(TComponent) == typeid(DirectionalLightComponent))
			return "DirectionalLightComponent";

		return "Undefined Component";
	}

	template<typename... Components>
	struct ComponentGroup
	{
	};

	using AllComponents = ComponentGroup<TagComponent, TransformComponent, MeshComponent, DirectionalLightComponent>;

}