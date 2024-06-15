#pragma once

#include <string>

#include "Lavender/Scripting/C++/Mutual/Core/Handle.hpp"

namespace Lavender::Script
{

	// Note(Jorben): This file is manually updated.

	enum class ComponentType
	{
		None = 0, Tag, Transform, Mesh, PointLight
	};

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
	};

}