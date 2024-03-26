#include "lvpch.h"
#include "Entity.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/ECS/Components.hpp"

namespace Lavender
{

	Entity::Entity(Ref<RegistryCollection> registry, UUID uuid)
		: m_Registry(registry), m_UUID(uuid)
	{
		if (!HasComponent<TagComponent>())
			AddComponent<TagComponent>(TagComponent("Unnamed Entity"));
	}

	Entity::~Entity()
	{
		//m_Registry->DeleteEntity(m_UUID);
	}

	void Entity::Destroy()
	{
		m_Registry->DeleteEntity(m_UUID);
	}

	Entity Entity::Create(Ref<RegistryCollection> registry)
	{
		UUID uuid = registry->CreateEntity();
		return Entity(registry, uuid);
	}

	Entity Entity::Create(Ref<RegistryCollection> registry, const UUID& uuid)
	{
		registry->CreateEntity(uuid);
		return Entity(registry, uuid);
	}

}