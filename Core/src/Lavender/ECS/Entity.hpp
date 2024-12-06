#pragma once

#include "Lavender/Core/Core.hpp"
#include "Lavender/Utils/Utils.hpp"

#include <entt/entt.hpp>

namespace Lavender
{

	// Pretty much opaque entity handle
	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::registry& registry);
		Entity(entt::registry& registry, const entt::entity& entity);
		virtual ~Entity() = default;

		template<typename TComponent>
		TComponent& AddComponent(const TComponent& component = TComponent())
		{
			if (HasComponent<TComponent>())
			{
				APP_LOG_WARN("Tried to AddComponent when component already exists. Use AddOrReplaceComponent for this.");
			}
			else
			{
				m_RegistryRef->emplace<TComponent>(m_Entity, component);
			}
			return m_RegistryRef->get<TComponent>(m_Entity);
		}

		template<typename TComponent>
		TComponent& AddOrReplaceComponent(const TComponent& component = TComponent())
		{
			m_RegistryRef->emplace_or_replace<TComponent>(m_Entity, component);
			return m_RegistryRef->get<TComponent>(m_Entity);
		}

		template<typename TComponent>
		bool HasComponent()
		{
			return m_RegistryRef->view<TComponent>().contains(m_Entity);
		}

		template<typename TComponent>
		TComponent& GetComponent()
		{
			return m_RegistryRef->get<TComponent>(m_Entity);
		}

		template<typename TComponent>
		void RemoveComponent()
		{
			m_RegistryRef->remove<TComponent>(m_Entity);
		}

		inline entt::entity GetHandle() const { return m_Entity; }

		// Operators
		Entity& operator == (const Entity& entity)
		{
			m_RegistryRef = entity.m_RegistryRef;
			m_Entity = entity.m_Entity;

			return *this;
		}

	private:
		entt::registry* m_RegistryRef = nullptr;
		entt::entity m_Entity = {};
	};

}