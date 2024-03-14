#pragma once

#include "Lavender/Utils/Utils.hpp"
#include "Lavender/Utils/UUID.hpp"

#include "Lavender/ECS/Components.hpp"
#include "Lavender/ECS/Registry.hpp"

#include "Lavender/Scripting/ScriptLoader.hpp"

#ifdef LV_PLATFORM_WINDOWS
#include "Lavender/Platforms/Windows/WindowsRegistryInterface.hpp"
#endif

namespace Lavender
{

	typedef void* (*AddComponentFn)(Component, uint64_t, void*);
	typedef void (*SetAddComponentFn)(AddComponentFn);

	typedef void* (*AddOrReplaceComponentFn)(Component, uint64_t, void*);
	typedef void (*SetAddOrReplaceComponentFn)(AddOrReplaceComponentFn);

	typedef bool (*HasComponentFn)(Component, uint64_t);
	typedef void (*SetHasComponentFn)(HasComponentFn);

	typedef void* (*GetComponentFn)(Component, uint64_t);
	typedef void (*SetGetComponentFn)(GetComponentFn);

	typedef void (*RemoveComponentFn)(Component, uint64_t);
	typedef void (*SetRemoveComponentFn)(RemoveComponentFn);

	// Note(Jorben): The functions with the Native_ are wrappers around member functions
	class RegistryInterface
	{
	public:
		RegistryInterface(Ref<RegistryCollection> collection, Ref<ScriptLoader> loader);
		virtual ~RegistryInterface();

		void Reload();

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// AddComponent
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		template<typename TComponent>
		TComponent* AddComponent(uint64_t entity, TComponent* component)
		{
			return m_Interface->AddComponent<TComponent>(entity, *component);
		}

		static void* Native_AddComponent(Component type, uint64_t entity, void* component)
		{
			void* data = nullptr;

			switch (type)
			{
			case Component::Tag:
			{
				TagComponent* tagComponent = (TagComponent*)component;
				data = (void*)s_Instance->AddComponent<TagComponent>(entity, tagComponent);
				break;
			}
			case Component::Transform:
			{
				TransformComponent* transformComponent = (TransformComponent*)component;
				data = (void*)s_Instance->AddComponent<TransformComponent>(entity, transformComponent);
				break;
			}
			default:
				// TODO: Implement logging (issue is with linking to script)
				break;
			}

			return data;
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// AddOrReplaceComponent
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		template<typename TComponent>
		TComponent* AddOrReplaceComponent(uint64_t entity, TComponent* component)
		{
			return m_Interface->AddOrReplaceComponent<TComponent>(entity, *component);
		}

		static void* Native_AddOrReplaceComponent(Component type, uint64_t entity, void* component)
		{
			void* data = nullptr;

			switch (type)
			{
			case Component::Tag:
			{
				TagComponent* tagComponent = (TagComponent*)component;
				data = (void*)s_Instance->AddOrReplaceComponent<TagComponent>(entity, tagComponent);
				break;
			}
			case Component::Transform:
			{
				TransformComponent* transformComponent = (TransformComponent*)component;
				data = (void*)s_Instance->AddOrReplaceComponent<TransformComponent>(entity, transformComponent);
				break;
			}
			default:
				// TODO: Implement logging (issue is with linking to script)
				break;
			}

			return data;
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// HasComponent
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		template<typename TComponent>
		bool HasComponent(uint64_t entity)
		{
			return m_Interface->HasComponent<TComponent>(entity);
		}

		static bool Native_HasComponent(Component type, uint64_t entity)
		{
			switch (type)
			{
			case Component::Tag:
				return s_Instance->HasComponent<TagComponent>(entity);

			case Component::Transform:
				return s_Instance->HasComponent<TransformComponent>(entity);

			default:
				// TODO: Implement logging (issue is with linking to script)
				break;
			}

			return false;
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// GetComponent
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		template<typename TComponent>
		TComponent* GetComponent(uint64_t entity)
		{
			return m_Interface->GetComponent<TComponent>(entity);
		}

		static void* Native_GetComponent(Component type, uint64_t entity)
		{
			void* data = nullptr;

			switch (type)
			{
			case Component::Tag:
				data = (void*)s_Instance->GetComponent<TagComponent>(entity);
				break;

			case Component::Transform:
				data = (void*)s_Instance->GetComponent<TransformComponent>(entity);
				break;

			default:
				// TODO: Implement logging (issue is with linking to script)
				break;
			}

			return data;
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// RemoveComponent
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		template<typename TComponent>
		void RemoveComponent(uint64_t entity)
		{
			m_Interface->RemoveComponent<TComponent>(entity);
		}

		static void Native_RemoveComponent(Component type, uint64_t entity)
		{
			switch (type)
			{
			case Component::Tag:
				s_Instance->RemoveComponent<TagComponent>(entity);
				break;

			case Component::Transform:
				s_Instance->RemoveComponent<TransformComponent>(entity);
				break;

			default:
				// TODO: Implement logging (issue is with linking to script)
				break;
			}
		}

		static RegistryInterface* Get() { return s_Instance; }

		static Ref<RegistryInterface> Create(Ref<RegistryCollection> collection, Ref<ScriptLoader> loader);

	private:
		#ifdef LV_PLATFORM_WINDOWS
		Ref<WindowsRegistryInterface> m_Interface = nullptr;
		#endif

		static RegistryInterface* s_Instance;
	};

}