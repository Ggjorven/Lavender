#pragma once

#include "Lavender/Utils/Utils.hpp"
#include "Lavender/Utils/UUID.hpp"

#include "Lavender/ECS/Registry.hpp"

#include <entt/entt.hpp>

namespace Lavender
{

	class Entity
	{
	public:
		Entity() = default;
		Entity(Ref<RegistryCollection> registry, UUID uuid);
		Entity(const Entity& entity) = default;
		virtual ~Entity();

		void Destroy();

		template<typename TComponent>
		TComponent& AddComponent(TComponent component = TComponent())
		{
			return m_Registry->AddComponent<TComponent>(m_UUID, component);
		}

		template<typename TComponent>
		TComponent& AddOrReplaceComponent(TComponent component = TComponent())
		{
			return m_Registry->AddOrReplaceComponent<TComponent>(m_UUID, component);
		}

		template<typename TComponent>
		bool HasComponent()
		{
			return m_Registry->HasComponent<TComponent>(m_UUID);
		}

		template<typename TComponent>
		TComponent& GetComponent()
		{
			return m_Registry->GetComponent<TComponent>(m_UUID);
		}

		template<typename TComponent>
		void RemoveComponent()
		{
			m_Registry->RemoveComponent<TComponent>(m_UUID);
		}

		UUID GetUUID() const { return m_UUID; }

		static Entity Create(Ref<RegistryCollection> registry);

	private:
		Ref<RegistryCollection> m_Registry = nullptr;
		UUID m_UUID = {};
	};

}