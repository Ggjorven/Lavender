#include "lvpch.h"
#include "Registry.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/ECS/Components.hpp"

#include <entt/entt.hpp>

namespace Lavender
{

	////////////////////////////////////////////////////////////////////////////
	// Helper functions
	////////////////////////////////////////////////////////////////////////////
	template<typename TComponent>
	void CopyComponent(entt::registry& src, entt::registry& dst, SortedDict<UUID, Entity>& entityMap)
	{
		auto view = src.view<TComponent>();

		for (auto& entity : view)
		{
			UUID uuid = src.get<UUID>(entity);
			auto dstEntity = entityMap[uuid].GetHandle();
			auto& component = src.get<TComponent>(entity);
			dst.emplace_or_replace<TComponent>(dstEntity, component);
		}
	}

	template<typename... TComponents>
	void CopyComponents(Utils::TypeGroup<TComponents...> group, entt::registry& src, entt::registry& dst, SortedDict<UUID, Entity>& entityMap)
	{
		// Note(Jorben): Empty function for when there are no components
	}

	template<typename FirstComponent, typename ... RestComponents>
	void CopyComponents(Utils::TypeGroup<FirstComponent, RestComponents...> group, entt::registry& src, entt::registry& dst, SortedDict<UUID, Entity>& entityMap)
	{
		CopyComponent<FirstComponent>(src, dst, entityMap);
		CopyComponents<RestComponents...>(Utils::TypeGroup<RestComponents...>(), src, dst, entityMap);
	}

	/////////////////////////////////////////////////////////////////////////////

	template<typename TComponent>
	void RemoveComponent(entt::registry& registry, const entt::entity& entity)
	{
		auto view = registry.view<TComponent>();

		if (view.contains(entity))
			registry.remove<TComponent>(entity);
	}

	template<typename... TComponents>
	void RemoveComponents(Utils::TypeGroup<TComponents...> group, entt::registry& registry, const entt::entity& entity)
	{
		// Note(Jorben): Empty function for when there are no components
	}

	template<typename FirstComponent, typename ... RestComponents>
	void RemoveComponents(Utils::TypeGroup<FirstComponent, RestComponents...> group, entt::registry& registry, const entt::entity& entity)
	{
		RemoveComponent<FirstComponent>(registry, entity);
		RemoveComponents<RestComponents...>(Utils::TypeGroup<RestComponents...>(), registry, entity);
	}

	////////////////////////////////////////////////////////////////////////////
	// Core functions
	////////////////////////////////////////////////////////////////////////////
	void Registry::AddEntity(const UUID& uuid, const Entity& entity)
	{
		m_Registry.emplace_or_replace<UUID>(entity.GetHandle(), uuid);

		m_Entities[uuid] = entity;
	}

	void Registry::RemoveEntity(const UUID& uuid)
	{
		entt::entity entity = m_Entities[uuid].GetHandle();

		RemoveComponents(AllComponents(), m_Registry, entity);

		m_Registry.destroy(entity);
		m_Entities.erase(uuid);
	}

	UUID Registry::CreateEntity(const UUID& uuid)
	{
		Entity entity = Entity(m_Registry);

		AddEntity(uuid, entity);

		return uuid;
	}

	void Registry::Clear()
	{
		m_Registry.clear();
		m_Entities.clear();
	}

	void Registry::CopyRegistry(Registry& registry)
	{
		// Create a new entity for every entity/uuid in the old registry
		for (auto& entity : registry.m_Entities)
		{
			m_Entities[entity.first] = Entity(m_Registry, m_Registry.create());
			m_Registry.emplace<UUID>(m_Entities[entity.first].GetHandle(), entity.first);
		}

		CopyComponents(AllComponents(), registry.m_Registry, m_Registry, m_Entities);
	}

}