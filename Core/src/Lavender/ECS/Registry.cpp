#include "lvpch.h"
#include "Registry.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/ECS/Components.hpp"

namespace Lavender
{

	void Registry::AddEntity(const UUID& uuid, const Entity& entity)
	{
		m_Registry.emplace_or_replace<UUID>(entity.GetHandle(), uuid);

		m_Entities[uuid] = entity;
	}

	void Registry::RemoveEntity(const UUID& uuid)
	{
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
	}

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

			// Note(Jorben): For some reason the MeshComponent doesn't copy properly
			/*
			if constexpr (std::is_same_v<TComponent, MeshComponent>)
			{
				MeshComponent& srcMesh = component;
				MeshComponent& dstMesh = dst.get<MeshComponent>(dstEntity);
				dstMesh.MeshObject = srcMesh.MeshObject;
				dstMesh.Material = srcMesh.Material;
			}
			*/
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