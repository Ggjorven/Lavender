#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <typeinfo>
#include <type_traits>

#include "Lavender/Scripting/Core.hpp"
#include "Lavender/Scripting/Components.hpp"

namespace Lavender
{

    typedef void* (*AddComponentFn)(Component, uint64_t, void*);
    typedef void* (*AddOrReplaceComponentFn)(Component, uint64_t, void*);
    typedef bool (*HasComponentFn)(Component, uint64_t);
    typedef void* (*GetComponentFn)(Component, uint64_t);
    typedef void (*RemoveComponentFn)(Component, uint64_t);

    struct EntityFunctions
    {
    public:
        inline static AddComponentFn AddComponentFP = nullptr;
        inline static AddOrReplaceComponentFn AddOrReplaceComponentFP = nullptr;
        inline static HasComponentFn HasComponentFP = nullptr;
        inline static GetComponentFn GetComponentFP = nullptr;
        inline static RemoveComponentFn RemoveComponentFP = nullptr;
    };

    class ScriptableEntity
    {
    public:
        virtual void OnCreate() = 0;
        virtual void OnUpdate(float deltaTime) = 0;

        template<typename TComponent>
        TComponent& AddComponent(TComponent component = TComponent())
        {
            TComponent tempComp = component;
            TComponent* newComp = (TComponent*)EntityFunctions::AddComponentFP(GetComponentType<TComponent>(), m_UUID, (void*)&tempComp);
            return *newComp;
        }

        template<typename TComponent>
        TComponent& AddOrReplaceComponent(TComponent component = TComponent())
        {
            TComponent tempComp = component;
            TComponent* newComp = (TComponent*)EntityFunctions::AddOrReplaceComponentFP(GetComponentType<TComponent>(), m_UUID, (void*)&tempComp);
            return *newComp;
        }

        template<typename TComponent>
        bool HasComponent()
        {
            return EntityFunctions::HasComponentFP(GetComponentType<TComponent>(), m_UUID);
        }

        template<typename TComponent>
        TComponent& GetComponent()
        {
            TComponent* newComp = (TComponent*)EntityFunctions::GetComponentFP(GetComponentType<TComponent>(), m_UUID);
            return *newComp;
        }

        template<typename TComponent>
        void RemoveComponent()
        {
            EntityFunctions::RemoveComponentFP(GetComponentType<TComponent>(), m_UUID);
        }

        inline void SetUUID(uint64_t uuid) { m_UUID = uuid; }
        inline uint64_t GetUUID() const { return m_UUID; }

    protected:
        uint64_t m_UUID = 0ull;
    };

    enum class VariableType
    {
        None = 0,
        Char,
        SChar,
        UChar,
        Short,
        UShort,
        Int,
        UInt,
        Long,
        ULong,
        LongLong,
        ULongLong,
        Float,
        Double,
        LongDouble,
        Bool
    };

    struct VariableInfo
    {
    public:
        VariableType Type = VariableType::None;
        std::string Name = "Empty";

        static VariableType GetVariableType(std::type_info const& type);

        static std::string VariableTypeToString(VariableType type);
    };

    struct VariableList
    {
    public:
        std::vector<VariableInfo> Variables = { };

        void Add(VariableType type, const std::string& name);
    };
}

#define LV_ENTITY(name) \
static Lavender::VariableList name##Variables = {}; \
extern "C" \
{ \
EXPORT name* Script_CreateEntity##name() \
{ \
    return new name(); \
} \
EXPORT void Script_DestroyEntity##name(name* var) \
{ \
    delete var; \
} \
EXPORT void Script_OnCreateEntity##name(name* instance) \
{ \
    instance->OnCreate(); \
} \
EXPORT void Script_OnUpdateEntity##name(name* instance, float deltaTime) \
{ \
    instance->OnUpdate(deltaTime); \
} \
EXPORT Lavender::VariableList* Script_GetVariables##name() \
{ \
    return &name##Variables; \
} \
EXPORT uint64_t Script_GetUUID##name(name* instance) \
{ \
    return instance->GetUUID(); \
} \
EXPORT void Script_SetUUID##name(name* instance, uint64_t uuid) \
{ \
    return instance->SetUUID(uuid); \
} \
}

// Note(Jorben): This is only for public variables use LV_DEFINE_PRIVATE_VARIABLE in a public scope and use LV_PRIVATE_VARIABLE for a private variable.
#define LV_VARIABLE(classname, type, varname) \
extern "C" \
{ \
EXPORT void Script_SetValueOf##varname##classname(classname* instance, type value) \
{ \
	instance->varname = value; \
} \
EXPORT type Script_GetValueOf##varname##classname(classname* instance) \
{ \
	return instance->varname; \
} \
} \
namespace \
{ \
inline void Add##varname##ToVariableList() \
{ \
    classname##Variables.Add(Lavender::VariableInfo::GetVariableType(typeid(type)), #varname); \
} \
\
static int dummy##varname = (Add##varname##ToVariableList(), 0); \
}

// Note(Jorben): This macro has to be used in a public scope
#define LV_DEFINE_PRIVATE_VARIABLE(type, varname) \
void Script_SetVar##varname##Value(type value) \
{ \
    varname = value; \
} \
type Script_GetVar##varname##Value() \
{ \
    return varname; \
} 

#define LV_PRIVATE_VARIABLE(classname, type, varname) \
extern "C" \
{ \
EXPORT void Script_SetValueOf##varname##classname(classname* instance, type value) \
{ \
	instance->Script_SetVar##varname##Value(value); \
} \
EXPORT type Script_GetValueOf##varname##classname(classname* instance) \
{ \
	return instance->Script_GetVar##varname##Value(); \
} \
} \
namespace \
{ \
inline void Add##varname##ToVariableList() \
{ \
    classname##Variables.Add(Lavender::VariableInfo::GetVariableType(typeid(type)), #varname); \
} \
\
static int dummy##varname = (Add##varname##ToVariableList(), 0); \
}