#include "lvpch.h"
#include "Entity.hpp"

#include "Lavender/Core/Logging.hpp"

namespace Lavender
{

	Entity::Entity(Ref<RegistryCollection> registry, UUID uuid)
		: m_Registry(registry), m_UUID(uuid)
	{
	}

	Entity::~Entity()
	{
		m_Registry->DeleteEntity(m_UUID);
	}

	Entity Entity::Create(Ref<RegistryCollection> registry)
	{
		UUID uuid = registry->CreateEntity();
		return Entity(registry, uuid);
	}

}