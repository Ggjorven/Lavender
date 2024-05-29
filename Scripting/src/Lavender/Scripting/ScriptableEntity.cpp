#include "ScriptableEntity.hpp"

namespace Lavender
{

    void ClassList::Add(const std::string& name)
    {
        Classes.push_back(name);
    }
    
    void VariableList::Add(VariableType type, const std::string& name)
    {
        Variables.push_back({ type, name });
    }

}

// Non-user exported functions
extern "C"
{

    EXPORT void Script_SetAddComponentFP(Lavender::AddComponentFn fn)
    {
        Lavender::EntityFunctions::AddComponentFP = fn;
    }

    EXPORT void Script_SetAddOrReplaceComponentFP(Lavender::AddOrReplaceComponentFn fn)
    {
        Lavender::EntityFunctions::AddOrReplaceComponentFP = fn;
    }

    EXPORT void Script_SetHasComponentFP(Lavender::HasComponentFn fn)
    {
        Lavender::EntityFunctions::HasComponentFP = fn;
    }

    EXPORT void Script_SetGetComponentFP(Lavender::GetComponentFn fn)
    {
        Lavender::EntityFunctions::GetComponentFP = fn;
    }

    EXPORT void Script_SetRemoveComponentFP(Lavender::RemoveComponentFn fn)
    {
        Lavender::EntityFunctions::RemoveComponentFP = fn;
    }

}