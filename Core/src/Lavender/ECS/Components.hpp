#pragma once

#include <string>
#include <typeinfo>
#include <type_traits>

#include <glm/glm.hpp>

#include "Lavender/Utils/Utils.hpp"

namespace Lavender
{

	//////////////////////////////////////////////////////////////////////////////////////
	// Components
	//////////////////////////////////////////////////////////////////////////////////////
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
	};

	//////////////////////////////////////////////////////////////////////////////////////
	// Utilities
	//////////////////////////////////////////////////////////////////////////////////////
	enum class Component
	{
		None = 0, Tag, Transform
	};

	template<typename TComponent>
	static Component GetComponentType()
	{
		if constexpr (std::is_same_v<TComponent, TagComponent>)
			return Component::Tag;
		else if constexpr (std::is_same_v<TComponent, TransformComponent>)
			return Component::Transform;

		return Component::None;
	}

	template<typename TComponent>
	static std::string ComponentToString()
	{
		if constexpr (std::is_same_v<TComponent, TagComponent>)
			return "TagComponent";
		else if constexpr (std::is_same_v<TComponent, TransformComponent>)
			return "TransformComponent";

		return "Undefined Component";
	}

	using AllComponents = Utils::TypeGroup<TagComponent, TransformComponent>;

}