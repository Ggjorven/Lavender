#pragma once

#include "Lavender/Core/Core.hpp"
#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Utils/UUID.hpp"

#include "Lavender/ECS/Entity.hpp"

#include <entt/entt.hpp>

namespace Lavender
{

	class Registry
	{
	public:
		Registry() = default;
		virtual ~Registry() = default;

		void AddEntity(const UUID& uuid, const Entity& entity);
		void RemoveEntity(const UUID& uuid);

		UUID CreateEntity(const UUID& uuid = UUID());
		void CopyRegistry(Registry& registry); // Copies 'registry' to this

		void Clear();

		inline Entity& GetEntity(const UUID& uuid) { return m_Entities[uuid]; }
		inline entt::registry& GetRegistry() { return m_Registry; }
		inline SortedDict<UUID, Entity>& GetDict() { return m_Entities; }

	private:
		entt::registry m_Registry = {};
		SortedDict<UUID, Entity> m_Entities = {};
	};

}