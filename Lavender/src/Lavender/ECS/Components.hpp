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

	struct ScriptComponent // TODO: Implement for scripting
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
		glm::vec3 Ambient = { 1.0f, 1.0f, 1.0f };
		glm::vec3 Diffuse = { 1.0f, 1.0f, 1.0f };
		glm::vec3 Specular = { 0.0f, 0.0f, 0.0f };

		float Constant = 0.5f;
		float Linear = 0.5f;
		float Quadratic = 0.5f;

	public:
		PointLightComponent() = default;
		// TODO: Proper initializer
		PointLightComponent(const PointLightComponent& other) = default;
	};

	enum class Component
	{
		None = 0, Tag, Transform, Script, Mesh, PointLight
	};

	template<typename TComponent>
	static Component GetComponentType()
	{
		if constexpr (std::is_same_v<TComponent, TagComponent>)
			return Component::Tag;
		else if constexpr (std::is_same_v<TComponent, TransformComponent>)
			return Component::Transform;
		else if constexpr (std::is_same_v<TComponent, ScriptComponent>)
			return Component::Script;
		else if constexpr (std::is_same_v<TComponent, MeshComponent>)
			return Component::Mesh;
		else if constexpr (std::is_same_v<TComponent, PointLightComponent>)
			return Component::Light;

		return Component::None;
	}

	template<typename TComponent>
	static std::string ComponentToString()
	{
		if constexpr (std::is_same_v<TComponent, TagComponent>)
			return "TagComponent";
		else if constexpr (std::is_same_v<TComponent, TransformComponent>)
			return "TransformComponent";
		else if constexpr (std::is_same_v<TComponent, ScriptComponent>)
			return "ScriptComponent";
		else if constexpr (std::is_same_v<TComponent, MeshComponent>)
			return "MeshComponent";
		else if constexpr (std::is_same_v<TComponent, PointLightComponent>)
			return "PointLightComponent";

		return "Undefined Component";
	}

	template<typename... Components>
	struct ComponentGroup
	{
	};

	using AllComponents = ComponentGroup<TagComponent, TransformComponent, ScriptComponent, MeshComponent, PointLightComponent>;

}