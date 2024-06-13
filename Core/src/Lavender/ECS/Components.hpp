#pragma once

#include <string>
#include <typeinfo>
#include <type_traits>

#include <glm/glm.hpp>

#include "Lavender/WorkSpace/Assets/MeshAsset.hpp"
#include "Lavender/WorkSpace/Assets/MaterialAsset.hpp"

namespace Lavender
{

	//////////////////////////////////////////////////////////////////////////////////////
	// Components
	//////////////////////////////////////////////////////////////////////////////////////
	enum class ComponentType
	{
		None = 0, Tag, Transform, Mesh, PointLight
	};

	struct TagComponent
	{
	public:
		std::string Tag = "Unnamed Entity";

	public:
		TagComponent() = default;
		TagComponent(const std::string& tag)
			: Tag(tag)
		{
		}
		TagComponent(const TagComponent& other) = default;

		inline static ComponentType GetStaticType() { return ComponentType::Tag; }
	};

	struct TransformComponent
	{
	public:
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Size = { 1.0f, 1.0f, 1.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };

	public:
		TransformComponent() = default;
		TransformComponent(const glm::vec3& position, const glm::vec3& size, const glm::vec3& rotation)
			: Position(position), Size(size), Rotation(rotation)
		{
		}
		TransformComponent(const TransformComponent& other) = default;

		inline static ComponentType GetStaticType() { return ComponentType::Transform; }
	};

	struct MeshComponent
	{
	public:
		Ref<MeshAsset> Mesh = nullptr;
		Ref<MaterialAsset> Material = nullptr;

	public:
		MeshComponent() = default;
		MeshComponent(Ref<MeshAsset> mesh, Ref<MaterialAsset> material)
			: Mesh(mesh), Material(material)
		{
		}
		MeshComponent(const MeshComponent& other) = default;

		inline static ComponentType GetStaticType() { return ComponentType::Mesh; }
	};

	struct PointLightComponent
	{
	public:
		glm::vec3 Colour = { 1.0f, 1.0f, 1.0f };
		float Radius = 5.0f;
		float Intensity = 1.0f;

	public:
		PointLightComponent() = default;
		PointLightComponent(const glm::vec3& colour, float radius, float intensity)
			: Colour(colour), Radius(radius), Intensity(intensity)
		{
		}
		PointLightComponent(const PointLightComponent& other) = default;

		inline static ComponentType GetStaticType() { return ComponentType::PointLight; }
	};

	//////////////////////////////////////////////////////////////////////////////////////
	// Utilities
	//////////////////////////////////////////////////////////////////////////////////////
	template<typename TComponent>
	static std::string ComponentToString()
	{
		if constexpr (std::is_same_v<TComponent, TagComponent>)
			return "TagComponent";
		else if constexpr (std::is_same_v<TComponent, TransformComponent>)
			return "TransformComponent";
		else if constexpr (std::is_same_v<TComponent, MeshComponent>)
			return "MeshComponent";
		else if constexpr (std::is_same_v<TComponent, PointLightComponent>)
			return "PointLightComponent";

		return "Undefined Component";
	}

	// Note(Jorben): Since I want this header file to also be able to be used on the scripting side
	// We can't really use Utils::TypeGroup, So we create a custom struct here as well.
	template<typename... TComponents>
	struct ComponentGroup
	{
	};

	using AllComponents = ComponentGroup<TagComponent, TransformComponent, MeshComponent, PointLightComponent>;

}