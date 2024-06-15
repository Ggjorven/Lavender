#pragma once

#include "Lavender/Utils/UUID.hpp"

#include "Lavender/Scripting/C++/Mutual/Core/Functions.hpp"

namespace Lavender::Script
{

	class Entity
	{
	public:
		Entity() = default;
		virtual ~Entity() = default;

		virtual void OnCreate() = 0;
		virtual void OnUpdate(float deltaTime) = 0;

		template<typename TComponent>
		TComponent& AddComponent(TComponent component = TComponent())
		{
			TComponent tempComp = component;
			TComponent* newComp = (TComponent*)ComponentFunctions::Add(TComponent::GetStaticType(), m_UUID, (void*)&tempComp);
			return *newComp;
		}

		template<typename TComponent>
		TComponent& AddOrReplaceComponent(TComponent component = TComponent())
		{
			TComponent tempComp = component;
			TComponent* newComp = (TComponent*)ComponentFunctions::AddOrReplace(TComponent::GetStaticType(), m_UUID, (void*)&tempComp);
			return *newComp;
		}

		template<typename TComponent>
		bool HasComponent()
		{
			return ComponentFunctions::Has(TComponent::GetStaticType(), m_UUID);
		}

		template<typename TComponent>
		TComponent& GetComponent()
		{
			TComponent* newComp = (TComponent*)ComponentFunctions::Get(TComponent::GetStaticType(), m_UUID);
			return *newComp;
		}

		template<typename TComponent>
		void RemoveComponent()
		{
			ComponentFunctions::Remove(TComponent::GetStaticType(), m_UUID);
		}

		inline UUID& GetUUID() { return m_UUID; }

	protected:
		UUID m_UUID = UUID::Empty;
	};

	typedef void (*OnCreateFn)(Entity*);
	typedef void (*OnUpdateFn)(Entity*, float);
	typedef UUID* (*GetUUIDFn)(Entity*);

#define LavenderEntity(name) \
InsightClass(name)

#if defined(LAVENDER_DLL)
	extern "C" inline __declspec(dllexport) void Lavender_ScriptEntityOnCreate(Lavender::Script::Entity* entity)
	{
		entity->OnCreate();
	}

	extern "C" inline __declspec(dllexport) void Lavender_ScriptEntityOnUpdate(Lavender::Script::Entity* entity, float deltaTime)
	{
		entity->OnUpdate(deltaTime);
	}

	extern "C" inline __declspec(dllexport) UUID* Lavender_ScriptEntityGetUUID(Lavender::Script::Entity* entity)
	{
		return &entity->GetUUID();
	}
#endif

// TODO: ^

	/*
	Example: 

	class Spider : public Lavender::Script::Entity
	{
	public:
		void OnCreate()
		{
			
		}

		void OnUpdate(float deltaTime)
		{

		}

	} LavenderEntity(Spider);
	*/

}