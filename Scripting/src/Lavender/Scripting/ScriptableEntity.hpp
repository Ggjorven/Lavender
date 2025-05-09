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

    struct ClassList
    {
    public:
        std::vector<std::string> Classes = { };

        void Add(const std::string& name);
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

        inline static VariableType GetVariableType(std::type_info const& type)
        {
            if (type == typeid(float))
                return VariableType::Float;
            else if (type == typeid(double))
                return VariableType::Double;
            else if (type == typeid(int))
                return VariableType::Int;
            else if (type == typeid(unsigned int))
                return VariableType::UInt;
            else if (type == typeid(bool))
                return VariableType::Bool;
            else if (type == typeid(char))
                return VariableType::Char;
            else if (type == typeid(signed char))
                return VariableType::SChar;
            else if (type == typeid(unsigned char))
                return VariableType::UChar;
            else if (type == typeid(short))
                return VariableType::Short;
            else if (type == typeid(unsigned short))
                return VariableType::UShort;
            else if (type == typeid(long))
                return VariableType::Long;
            else if (type == typeid(unsigned long))
                return VariableType::ULong;
            else if (type == typeid(long long))
                return VariableType::LongLong;
            else if (type == typeid(unsigned long long))
                return VariableType::ULongLong;
            else if (type == typeid(long double))
                return VariableType::LongDouble;

            return VariableType::None;
        }

        inline static std::string VariableTypeToString(VariableType type)
        {
            switch (type)
            {
            case VariableType::None:
                return "None";
            case VariableType::Float:
                return "Float";
            case VariableType::Double:
                return "Double";
            case VariableType::Int:
                return "Int";
            case VariableType::UInt:
                return "UInt";
            case VariableType::Bool:
                return "Bool";
            case VariableType::Char:
                return "Char";
            case VariableType::SChar:
                return "SChar";
            case VariableType::UChar:
                return "UChar";
            case VariableType::Short:
                return "Short";
            case VariableType::UShort:
                return "UShort";
            case VariableType::Long:
                return "Long";
            case VariableType::ULong:
                return "ULong";
            case VariableType::LongLong:
                return "LongLong";
            case VariableType::ULongLong:
                return "ULongLong";
            case VariableType::LongDouble:
                return "LongDouble";

            default:
                return "Unknown";
            }
        }
    };

    struct VariableList
    {
    public:
        std::vector<VariableInfo> Variables = { };

        void Add(VariableType type, const std::string& name);
    };
}

// Public scope class list
#ifdef LV_CLASS_EXPORT
inline Lavender::ClassList DefinedClasses = {};

#ifndef LV_CLASSES_EXPORTED
#define LV_CLASSES_EXPORTED
extern "C"
{
EXPORT inline Lavender::ClassList* Script_GetDefinedClasses()
{ 
    return &DefinedClasses;
}
}
#endif
#endif

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
} \
namespace \
{ \
inline void Add##name##ToDefinedClasses() \
{ \
    DefinedClasses.Add(#name); \
} \
\
static int dummy##name = (Add##name##ToDefinedClasses(), 0); \
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
inline void Add##varname##ToVariableList##classname() \
{ \
    classname##Variables.Add(Lavender::VariableInfo::GetVariableType(typeid(type)), #varname); \
} \
\
static int dummy##varname##classname = (Add##varname##ToVariableList##classname(), 0); \
}

// Note(Jorben): The private variables are not recommended. Personal opinion.
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
inline void Add##varname##ToVariableList##classname() \
{ \
    classname##Variables.Add(Lavender::VariableInfo::GetVariableType(typeid(type)), #varname); \
} \
\
static int dummy##varname##classname = (Add##varname##ToVariableList##classname(), 0); \
}