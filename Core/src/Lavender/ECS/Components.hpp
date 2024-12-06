#pragma once

#include <string>
#include <typeinfo>
#include <type_traits>

#include <glm/glm.hpp>

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/WorkSpace/Assets/MeshAsset.hpp"
#include "Lavender/WorkSpace/Assets/MaterialAsset.hpp"

namespace Lavender
{

	//////////////////////////////////////////////////////////////////////////////////////
	// Components
	//////////////////////////////////////////////////////////////////////////////////////
	enum class ComponentType
	{
		None = 0, Tag, Transform, Mesh, PointLight, Script, Camera
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
		inline ComponentType GetType() { return GetStaticType(); }
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
		inline ComponentType GetType() { return GetStaticType(); }
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
		inline ComponentType GetType() { return GetStaticType(); }
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
		inline ComponentType GetType() { return GetStaticType(); }
	};

	struct ScriptComponent
	{
	public:
		std::string Class = {};

	public:
		ScriptComponent() = default;
		ScriptComponent(const std::string& className)
			: Class(className)
		{
		}
		ScriptComponent(const ScriptComponent& other) = default;

		inline static ComponentType GetStaticType() { return ComponentType::Script; }
		inline ComponentType GetType() { return GetStaticType(); }
	};

	struct CameraComponent
	{
	public:
		float Yaw = 0.0f;
		float Pitch = 0.0f;
		float FOV = 45.0f;

		float Near = 0.1f;
		float Far = 1000.0f;

		bool Active = true;

	public:
		CameraComponent() = default;
		CameraComponent(float yaw, float pitch, float fov, bool active = true, float nearr = 0.1f, float farr = 1000.0f) // Naming sucks because of Windows #define near & far #undef doesn't work
			: Yaw(yaw), Pitch(pitch), FOV(fov), Near(nearr), Far(farr), Active(active)
		{
		}
		CameraComponent(const CameraComponent& other) = default;

		inline static ComponentType GetStaticType() { return ComponentType::Camera; }
		inline ComponentType GetType() { return GetStaticType(); }
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
		else if constexpr (std::is_same_v<TComponent, ScriptComponent>)
			return "ScriptComponent";
		else if constexpr (std::is_same_v<TComponent, CameraComponent>)
			return "CameraComponent";
		
		return "Undefined Component";
	}

	using AllComponents = Utils::TypeGroup<TagComponent, TransformComponent, MeshComponent, PointLightComponent, ScriptComponent, CameraComponent>;

}