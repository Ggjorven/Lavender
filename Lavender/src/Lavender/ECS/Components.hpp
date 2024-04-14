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

	struct PointLightComponent // TODO: Implement for scripting
	{
	public:
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f }; // TODO: Use transformComponent
		PUBLIC_PADDING(0, 4);

		glm::vec3 Ambient = {};
		PUBLIC_PADDING(1, 4);
		glm::vec3 Diffuse = {};
		PUBLIC_PADDING(2, 4);
		glm::vec3 Specular = {};
		PUBLIC_PADDING(3, 4);

		float Constant = 0.5f;
		float Linear = 0.5f;
		float Quadratic = 0.5f;
		PUBLIC_PADDING(4, 4);

	public:
		PointLightComponent() = default;
		// TODO: Proper initializer
		PointLightComponent(const PointLightComponent& other) = default;
	};

	enum class Component
	{
		None = 0, Tag, Transform, Mesh, PointLight
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
		else if (typeid(TComponent) == typeid(PointLightComponent))
			return Component::Light;

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
		else if (typeid(TComponent) == typeid(PointLightComponent))
			return "PointLightComponent";

		return "Undefined Component";
	}

	template<typename... Components>
	struct ComponentGroup
	{
	};

	using AllComponents = ComponentGroup<TagComponent, TransformComponent, MeshComponent, PointLightComponent>;

}