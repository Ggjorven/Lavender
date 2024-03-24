#pragma once

#include "Lavender/Utils/Utils.hpp"
#include "Lavender/Utils/UUID.hpp"

#include <unordered_map>

#include <entt/entt.hpp>

namespace Lavender
{

	class Registry
	{
	public:
		Registry();
		virtual ~Registry();

		UUID CreateEntity();
		void DeleteEntity(UUID entity);

		template<typename TComponent>
		TComponent& AddComponent(UUID entity, TComponent component = TComponent())
		{
			if (HasComponent<TComponent>(entity))
			{
				LV_LOG_WARN("Tried to add component using AddComponent<T>, when the T component already exists. If you meant to overwrite existing components use AddOrReplaceComponent<T>.");
				return m_Registry.get<TComponent>(m_Entities[entity]);
			}

			m_Registry.emplace<TComponent>(m_Entities[entity], component);
			return m_Registry.get<TComponent>(m_Entities[entity]);
		}

		template<typename TComponent>
		TComponent& AddOrReplaceComponent(UUID entity, TComponent component = TComponent())
		{
			m_Registry.emplace_or_replace<TComponent>(m_Entities[entity], component);
			return m_Registry.get<TComponent>(m_Entities[entity]);
		}

		template<typename TComponent>
		bool HasComponent(UUID entity)
		{
			return m_Registry.view<TComponent>().contains(m_Entities[entity]);
		}

		template<typename TComponent>
		TComponent& GetComponent(UUID entity)
		{
			return m_Registry.get<TComponent>(m_Entities[entity]);
		}

		template<typename TComponent>
		void RemoveComponent(UUID entity)
		{
			m_Registry.remove<TComponent>(m_Entities[entity]);
		}

		inline entt::registry& GetRegistry() { return m_Registry; }
		inline Dict<UUID, entt::entity>& GetEntityMap() { return m_Entities; }

		Ref<Registry> DeepCopy();

		static Ref<Registry> Create();

	private:
		entt::registry m_Registry = {};

		Dict<UUID, entt::entity> m_Entities = { };
	};


	class RegistryCollection
	{
	public:
		RegistryCollection();
		virtual ~RegistryCollection();

		UUID CreateEntity();
		void DeleteEntity(UUID entity);

		template<typename TComponent>
		TComponent& AddComponent(UUID entity, TComponent component = TComponent())
		{
			return m_MainRegistry->AddComponent<TComponent>(entity, component);
		}

		template<typename TComponent>
		TComponent& AddOrReplaceComponent(UUID entity, TComponent component = TComponent())
		{
			return m_MainRegistry->AddOrReplaceComponent<TComponent>(entity, component);
		}

		template<typename TComponent>
		bool HasComponent(UUID entity)
		{
			return m_MainRegistry->HasComponent<TComponent>(entity);
		}

		template<typename TComponent>
		TComponent& GetComponent(UUID entity)
		{
			return m_MainRegistry->GetComponent<TComponent>(entity);
		}

		template<typename TComponent>
		void RemoveComponent(UUID entity)
		{
			m_MainRegistry->RemoveComponent<TComponent>(entity);
		}

		void SwitchRegistry();

		inline Ref<Registry> GetMainRegistry() { return m_MainRegistry; }
		inline Ref<Registry> GetSecondRegistry() { return m_SecondaryRegistry; }

		// Note(Jorben): I know, confusing name.
		inline bool IsMainMain() const { return m_MainIsMain; }

		static Ref<RegistryCollection> Create();

	private:
		// Note(Jorben): I know, confusing name.
		bool m_MainIsMain = true;

		Ref<Registry> m_MainRegistry = nullptr;
		Ref<Registry> m_SecondaryRegistry = nullptr;
	};

}