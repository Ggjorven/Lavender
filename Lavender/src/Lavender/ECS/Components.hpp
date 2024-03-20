#pragma once

#include <string>

#include <glm/glm.hpp>

namespace Lavender
{

	struct TagComponent
	{
	public:
		std::string Tag = "Empty Tag";

	public:
		TagComponent() = default;
		TagComponent(const std::string& str);
		TagComponent(const TagComponent& tag) = default;
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
		TransformComponent(const TransformComponent& tag) = default;
	};

	enum class Component
	{
		None = 0, Tag, Transform
	};

	template<typename TComponent>
	static Component GetComponentType()
	{
		if (typeid(TComponent) == typeid(TagComponent))
			return Component::Tag;
		else if (typeid(TComponent) == typeid(TransformComponent))
			return Component::Transform;

		return Component::None;
	}

	template<typename TComponent>
	static std::string ComponentToString()
	{
		if (typeid(TComponent) == typeid(TagComponent))
			return "TagComponent";
		else if (typeid(TComponent) == typeid(TransformComponent))
			return "TransformComponent";

		return "Undefined Component";
	}

	template<typename... Component>
	struct ComponentGroup
	{
	};

	using AllComponents = ComponentGroup<TagComponent, TransformComponent>;

}