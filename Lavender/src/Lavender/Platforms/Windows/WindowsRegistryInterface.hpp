#pragma once

#include "Lavender/Utils/Utils.hpp"
#include "Lavender/Utils/UUID.hpp"

#include "Lavender/ECS/Registry.hpp"

#include "Lavender/Scripting/ScriptLoader.hpp"
#include "Lavender/Platforms/Windows/WindowsScriptLoader.hpp"

namespace Lavender
{



	class WindowsRegistryInterface
	{
	public:
		WindowsRegistryInterface(Ref<RegistryCollection> collection, Ref<ScriptLoader> loader);
		virtual ~WindowsRegistryInterface();

		void Initialize();

		template<typename TComponent>
		TComponent* AddComponent(uint64_t entity, TComponent component = TComponent())
		{
			TComponent& comp = m_Collection->AddComponent<TComponent>(UUID(entity), component);
			return &comp;
		}

		template<typename TComponent>
		TComponent* AddOrReplaceComponent(uint64_t entity, TComponent component = TComponent())
		{
			TComponent& comp = m_Collection->AddOrReplaceComponent<TComponent>(UUID(entity), component);
			return &comp;
		}

		template<typename TComponent>
		bool HasComponent(uint64_t entity)
		{
			return m_Collection->HasComponent<TComponent>(UUID(entity));
		}

		template<typename TComponent>
		TComponent* GetComponent(uint64_t entity)
		{
			TComponent& comp = m_Collection->GetComponent<TComponent>(UUID(entity));
			return &comp;
		}

		template<typename TComponent>
		void RemoveComponent(uint64_t entity)
		{
			m_Collection->RemoveComponent<TComponent>(UUID(entity));
		}

	private:
		Ref<RegistryCollection> m_Collection = nullptr;
		Ref<WindowsScriptLoader> m_Loader = nullptr;
	};

}