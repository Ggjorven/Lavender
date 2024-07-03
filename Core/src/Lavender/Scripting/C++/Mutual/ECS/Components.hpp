#pragma once

#include <string>

#include "Lavender/Scripting/C++/Mutual/Core/Handle.hpp"
#include "Lavender/Scripting/C++/Mutual/Core/Containers.hpp"

namespace Lavender::Script
{

	// Note(Jorben): This file is manually updated.

	enum class ComponentType
	{
		None = 0, Tag, Transform, Mesh, PointLight, Script, Camera
	};

	// Handle doesn't need to be a & since everything inside is based on pointers
	struct TagComponent
	{
	public:
		Handle<std::string> Tag = {};

	public:
		TagComponent() = default;
		TagComponent(const Handle<std::string>& tag)
			: Tag(tag)
		{
		}
		TagComponent(const TagComponent& other) = default;

		inline static ComponentType GetStaticType() { return ComponentType::Tag; }
		inline ComponentType GetType() { return GetStaticType(); }
	};

	// Handle doesn't need to be a & since everything inside is based on pointers
	struct TransformComponent
	{
	public:
		Handle<Vec3> Position = {};
		Handle<Vec3> Size = {};
		Handle<Vec3> Rotation = {};

	public:
		TransformComponent() = default;
		TransformComponent(const Handle<Vec3>& position, const Handle<Vec3>& size, const Handle<Vec3>& rotation)
			: Position(position), Size(size), Rotation(rotation)
		{
		}
		TransformComponent(const TransformComponent& other) = default;

		inline static ComponentType GetStaticType() { return ComponentType::Transform; }
		inline ComponentType GetType() { return GetStaticType();  }
	};

	// TODO: MeshComponent

	// Handle doesn't need to be a & since everything inside is based on pointers
	struct PointLightComponent
	{
	public:
		Handle<Vec3> Colour = {};
		Handle<float> Radius = {};
		Handle<float> Intensity = {};

	public:
		PointLightComponent() = default;
		PointLightComponent(const Handle<Vec3>& colour, const Handle<float>& radius, const Handle<float>& intensity)
			: Colour(colour), Radius(radius), Intensity(intensity)
		{
		}
		PointLightComponent(const PointLightComponent& other) = default;

		inline static ComponentType GetStaticType() { return ComponentType::PointLight; }
		inline ComponentType GetType() { return GetStaticType(); }
	};

	// TODO: ScriptComponent

	// Handle doesn't need to be a & since everything inside is based on pointers
	struct CameraComponent
	{
	public:
		Handle<float> Yaw = {};
		Handle<float> Pitch = {};
		Handle<float> FOV = {};

		Handle<float> Near = {};
		Handle<float> Far = {};

		Handle<bool> Active = {};

	public:
		CameraComponent() = default;
		CameraComponent(const Handle<float>& yaw, const Handle<float>& pitch, const Handle<float>& fov, const Handle<float>& nearr, const Handle<float>& farr, const Handle<bool>& active) // Naming sucks because of Windows #define near & far #undef doesn't work
			: Yaw(yaw), Pitch(pitch), FOV(fov), Near(nearr), Far(farr), Active(active)
		{
		}
		CameraComponent(const CameraComponent& other) = default;

		inline static ComponentType GetStaticType() { return ComponentType::Camera; }
		inline ComponentType GetType() { return GetStaticType(); }
	};

}