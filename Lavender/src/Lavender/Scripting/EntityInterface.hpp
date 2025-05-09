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

	//////////////////////////////////////////////////////////////////////////////
	// Variable functions
	//////////////////////////////////////////////////////////////////////////////
	typedef char (*GetCharFn)(ScriptableEntity*);
	typedef void (*SetCharFn)(ScriptableEntity*, char);

	typedef signed char (*GetSCharFn)(ScriptableEntity*);
	typedef void (*SetSCharFn)(ScriptableEntity*, signed char);

	typedef unsigned char (*GetUCharFn)(ScriptableEntity*);
	typedef void (*SetUCharFn)(ScriptableEntity*, unsigned char);

	typedef short (*GetShortFn)(ScriptableEntity*);
	typedef void (*SetShortFn)(ScriptableEntity*, short);

	typedef unsigned short (*GetUShortFn)(ScriptableEntity*);
	typedef void (*SetUShortFn)(ScriptableEntity*, unsigned short);

	typedef int (*GetIntFn)(ScriptableEntity*);
	typedef void (*SetIntFn)(ScriptableEntity*, int);

	typedef unsigned int (*GetUIntFn)(ScriptableEntity*);
	typedef void (*SetUIntFn)(ScriptableEntity*, unsigned int);

	typedef long (*GetLongFn)(ScriptableEntity*);
	typedef void (*SetLongFn)(ScriptableEntity*, long);

	typedef unsigned long (*GetULongFn)(ScriptableEntity*);
	typedef void (*SetULongFn)(ScriptableEntity*, unsigned long);

	typedef long long (*GetLongLongFn)(ScriptableEntity*);
	typedef void (*SetLongLongFn)(ScriptableEntity*, long long);

	typedef unsigned long long (*GetULongLongFn)(ScriptableEntity*);
	typedef void (*SetULongLongFn)(ScriptableEntity*, unsigned long long);

	typedef float (*GetFloatFn)(ScriptableEntity*);
	typedef void (*SetFloatFn)(ScriptableEntity*, float);

	typedef double (*GetDoubleFn)(ScriptableEntity*);
	typedef void (*SetDoubleFn)(ScriptableEntity*, double);

	typedef long double (*GetLongDoubleFn)(ScriptableEntity*);
	typedef void (*SetLongDoubleFn)(ScriptableEntity*, long double);

	typedef bool (*GetBoolFn)(ScriptableEntity*);
	typedef void (*SetBoolFn)(ScriptableEntity*, bool);
	//////////////////////////////////////////////////////////////////////////////

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

	public:
		bool Validate() const;
		void Clear();
	};

	// Note(Jorben): The functions are void* so they can be used for any type, but under the hood they are only for one type
	struct EntityVariableFunctions
	{
	public:
		void* Get = nullptr;
		void* Set = nullptr;
	};

	class EntityInterface
	{
	public:
		EntityInterface() = default;
		virtual ~EntityInterface() = default;

		virtual void Reload() = 0;

		virtual void InvokeOnCreate() = 0;
		virtual void InvokeOnUpdate(float deltaTime) = 0;

		virtual VariableList GetVariables() = 0;
		virtual bool HasVariable(const std::string& name) = 0;

		virtual Entity& GetEntity() = 0;

		static Ref<EntityInterface> Create(const UUID& uuid, Ref<ScriptLoader> loader, const std::string& classname);
		static Ref<EntityInterface> Create(Entity& entity, Ref<ScriptLoader> loader, const std::string& classname);
	};

}