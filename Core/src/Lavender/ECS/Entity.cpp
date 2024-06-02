#include "lvpch.h"
#include "Entity.hpp"

#include "Lavender/Core/Logging.hpp"

namespace Lavender
{

	Entity::Entity(entt::registry& registry)
		: Entity(registry, registry.create())
	{
	}

	Entity::Entity(entt::registry& registry, const entt::entity& entity)
		: m_RegistryRef(&registry), m_Entity(entity)
	{
	}

}