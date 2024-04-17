#include "ScriptableEntity.hpp"

namespace Lavender
{

    VariableType VariableInfo::GetVariableType(std::type_info const& type)
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

    std::string VariableInfo::VariableTypeToString(VariableType type)
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