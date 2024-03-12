#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <typeinfo>
#include <type_traits>

#define EXPORT __declspec(dllexport)

namespace Lavender
{

    class ScriptableEntity
    {
    public:
        virtual void OnCreate() = 0;
        virtual void OnUpdate(float deltaTime) = 0;
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
}

// Note(Jorben): At the moment the variable has to be public and this is declared outside the class
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