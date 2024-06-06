#pragma once

#include <Lavender/Utils/UUID.hpp>
#include <Lavender/ECS/Components.hpp>

#include <Lavender/Scripting/C++/Mutual/Functions.hpp>

#include <Insight/Insight.hpp> // Make sure the dll is linked against Insight

namespace Lavender
{

	class ScriptableEntity
	{
	public:
		ScriptableEntity() = default;
		virtual ~ScriptableEntity() = default;

		virtual void OnCreate() = 0;
		virtual void OnUpdate(float deltaTime) = 0;

		// TODO: Component functions

		inline UUID& GetUUID() { return m_UUID; }

	protected:
		UUID m_UUID = UUID::Empty;
	};

	typedef void (*OnCreateFn)(ScriptableEntity*);
	typedef void (*OnUpdateFn)(ScriptableEntity*, float);
	typedef UUID* (*GetUUIDFn)(ScriptableEntity*);

#define LavenderEntity(name) \
InsightClass(name)

#if defined(LAVENDER_DLL)
	extern "C" inline __declspec(dllexport) void Lavender_ScriptableEntityOnCreate(Lavender::ScriptableEntity* entity)
	{
		entity->OnCreate();
	}

	extern "C" inline __declspec(dllexport) void Lavender_ScriptableEntityOnUpdate(Lavender::ScriptableEntity* entity, float deltaTime)
	{
		entity->OnUpdate(deltaTime);
	}

	extern "C" inline __declspec(dllexport) UUID* Lavender_ScriptableEntityGetUUID(Lavender::ScriptableEntity* entity)
	{
		return &entity->GetUUID();
	}
#endif

// TODO: ^

	/*
	Example: 

	class Spider
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