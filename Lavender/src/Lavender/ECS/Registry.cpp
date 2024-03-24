#include "lvpch.h"
#include "Registry.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/ECS/Components.hpp"

namespace Lavender
{

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Helper functions
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template<typename TComponent>
	void CopyComponent(entt::registry& src, entt::registry& dst, Dict<UUID, entt::entity>& entityMap)
	{
		auto view = src.view<TComponent>();

		for (auto& entity : view)
		{
			UUID uuid = src.get<UUID>(entity);
			auto dstEntity = entityMap[uuid];
			auto component = src.get<TComponent>(entity);
			dst.emplace<TComponent>(dstEntity, component);
		}
	}

	template<typename... TComponents>
	void CopyComponents(ComponentGroup<TComponents...> group, entt::registry& src, entt::registry& dst, Dict<UUID, entt::entity>& entityMap)
	{
		// Note(Jorben): Empty function for when there are no components
	}

	template<typename FirstComponent, typename ... RestComponents>
	void CopyComponents(ComponentGroup<FirstComponent, RestComponents...> group, entt::registry& src, entt::registry& dst, Dict<UUID, entt::entity>& entityMap)
	{
		CopyComponent<FirstComponent>(src, dst, entityMap);
		CopyComponents<RestComponents...>(ComponentGroup<RestComponents...>(), src, dst, entityMap);
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Main functions
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Registry::Registry()
	{
	}

	Registry::~Registry()
	{
		m_Registry.clear();
	}

	UUID Registry::CreateEntity()
	{
		UUID uuid = UUID::Create();
		entt::entity entity = m_Registry.create();

		m_Registry.emplace<UUID>(entity, uuid); // TODO: Maybe only set the UUID component when copying
		m_Entities[uuid] = entity;

		return uuid;
	}

	void Registry::DeleteEntity(UUID entity)
	{
		m_Registry.destroy(m_Entities[entity]);
		m_Entities.erase(entity);
	}

	Ref<Registry> Registry::DeepCopy()
	{
		Ref<Registry> registry = RefHelper::Create<Registry>();

		// Create a new entity for every entity/uuid in the old registry
		for (auto& entity : m_Entities)
		{
			registry->m_Entities[entity.first] = registry->m_Registry.create();
			registry->m_Registry.emplace<UUID>(registry->m_Entities[entity.first], entity.first);
		}

		CopyComponents(AllComponents(), m_Registry, registry->m_Registry, registry->m_Entities);

		return registry;
	}

	Ref<Registry> Registry::Create()
	{
		return RefHelper::Create<Registry>();
	}



	RegistryCollection::RegistryCollection()
		: m_MainRegistry(Registry::Create())
	{
	}

	RegistryCollection::~RegistryCollection()
	{
	}

	UUID RegistryCollection::CreateEntity()
	{
		return m_MainRegistry->CreateEntity();
	}

	void RegistryCollection::DeleteEntity(UUID entity)
	{
		m_MainRegistry->DeleteEntity(entity);
	}

	void RegistryCollection::SwitchRegistry()
	{
		if (m_MainIsMain)
		{
			m_SecondaryRegistry = m_MainRegistry;
			m_MainRegistry = m_MainRegistry->DeepCopy();

			m_MainIsMain = false;
		}
		else
		{
			m_MainRegistry = m_SecondaryRegistry;
			m_SecondaryRegistry = nullptr;

			m_MainIsMain = true;
		}
	}

	Ref<RegistryCollection> RegistryCollection::Create()
	{
		return RefHelper::Create<RegistryCollection>();
	}

}