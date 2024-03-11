#pragma once

#include <string>

#include "Lavender/Utils/Utils.hpp"

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


	template<typename... Component>
	struct ComponentGroup
	{
	};

	using AllComponents = ComponentGroup<TagComponent, TransformComponent>;

}