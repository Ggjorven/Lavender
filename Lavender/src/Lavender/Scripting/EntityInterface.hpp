#pragma once

#include <string>
#include <filesystem>

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/ECS/Entity.hpp"
#include "Lavender/Scripting/ScriptableEntity.hpp"
#include "Lavender/Scripting/ScriptLoader.hpp"

namespace Lavender
{

	typedef ScriptableEntity* (*CreateFn)();
	typedef void (*DestroyFn)(ScriptableEntity*);

	typedef void (*OnCreateFn)(ScriptableEntity*);
	typedef void (*OnUpdateFn)(ScriptableEntity*, float);

	typedef VariableList* (*GetVariableListFn)();

	typedef uint64_t (*GetUUIDFn)(ScriptableEntity*);
	typedef void (*SetUUIDFn)(ScriptableEntity*, uint64_t);

	typedef float (*GetFloatFn)(ScriptableEntity*);
	typedef void (*SetFloatFn)(ScriptableEntity*, float);
	// TODO: Add more variable types

	struct ScriptableEntityFunctions
	{
	public:
		CreateFn Create = nullptr;
		DestroyFn Destroy = nullptr;

		OnCreateFn OnCreate = nullptr;
		OnUpdateFn OnUpdate = nullptr;

		GetVariableListFn GetVariableList = nullptr;

		GetUUIDFn GetUUID = nullptr;
		SetUUIDFn SetUUID = nullptr;

		// TODO: Variable list with function pointers

	public:
		bool Validate() const;
		void Clear();
	};

	class EntityInterface
	{
	public:
		EntityInterface() = default;
		virtual ~EntityInterface() = default;

		virtual void Reload() = 0;

		virtual void InvokeOnCreate() = 0;
		virtual void InvokeOnUpdate(float deltaTime) = 0;

		static Ref<EntityInterface> Create(Entity& entity, Ref<ScriptLoader> loader, const std::string& classname);
	};

}