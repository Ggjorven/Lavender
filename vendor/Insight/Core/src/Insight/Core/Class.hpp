#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include <Insight/Core/If.hpp>
#include <Insight/Core/Help.hpp>
#include <Insight/Core/Variables.hpp>

namespace Insight::Internal
{
	
    ///////////////////////////////////////////////////////////////////////////////////
    // Variable Info
    ///////////////////////////////////////////////////////////////////////////////////
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

    template<typename T>
    inline constexpr static VariableType GetVariableType()
    {
        if constexpr (std::is_same_v<T, char>)
            return VariableType::Char;
        else if constexpr (std::is_same_v<T, signed char>)
            return VariableType::SChar;
        else if constexpr (std::is_same_v<T, unsigned char>)
            return VariableType::UChar;
        else if constexpr (std::is_same_v<T, short>)
            return VariableType::Short;
        else if constexpr (std::is_same_v<T, unsigned short>)
            return VariableType::UShort;
        else if constexpr (std::is_same_v<T, int>)
            return VariableType::Int;
        else if constexpr (std::is_same_v<T, unsigned int>)
            return VariableType::UInt;
        else if constexpr (std::is_same_v<T, long>)
            return VariableType::Long;
        else if constexpr (std::is_same_v<T, unsigned long>)
            return VariableType::ULong;
        else if constexpr (std::is_same_v<T, long long>)
            return VariableType::LongLong;
        else if constexpr (std::is_same_v<T, unsigned long long>)
            return VariableType::ULongLong;
        else if constexpr (std::is_same_v<T, float>)
            return VariableType::Float;
        else if constexpr (std::is_same_v<T, double>)
            return VariableType::Double;
        else if constexpr (std::is_same_v<T, long double>)
            return VariableType::LongDouble;
        else if constexpr (std::is_same_v<T, bool>)
            return VariableType::Bool;

        return VariableType::None;
    }

    struct VariableInfo
    {
    public:
        std::string Name = {};
        VariableType Type = VariableType::None;

    public:
        VariableInfo() = default;
        VariableInfo(const std::string& name, VariableType type)
            : Name(name), Type(type)
        {
        }
        virtual ~VariableInfo() = default;

        template<typename T>
        inline constexpr static VariableInfo Create(const std::string& name) { return VariableInfo(name, GetVariableType<T>()); }
    };
    ///////////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////////
    // Functions Info
    ///////////////////////////////////////////////////////////////////////////////////
    struct FunctionInfo
    {
    public:
        std::string Name = {};
        std::string Signature = {};

    public:
        FunctionInfo() = default;
        FunctionInfo(const std::string& name, const std::string& signature)
            : Name(name), Signature(signature)
        {
        }
        virtual ~FunctionInfo() = default;
    };
    ///////////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////////
    // Class Info
    ///////////////////////////////////////////////////////////////////////////////////
    struct ClassInfo
    {
    public:
        std::vector<VariableInfo> Variables = { };
        std::vector<FunctionInfo> Functions = { };
    };

	class Classes
	{
	public:
        Classes() = default;
        virtual ~Classes() = default;

        inline void AddClass(const std::string& name)
        {
            if (m_Classes.find(name) == m_Classes.end())
            {
                m_Classes[name] = {};
                return;
            }
        }
        inline void AddVariable(const std::string& name, const VariableInfo& var)
        {
            m_Classes[name].Variables.push_back(var);
        }
        inline void AddFunction(const std::string& name, const FunctionInfo& func)
        {
            m_Classes[name].Functions.push_back(func);
        }

        inline std::unordered_map<std::string, ClassInfo>& GetClasses()
        {
            return m_Classes;
        }
        inline std::vector<VariableInfo>& GetVariables(const std::string& name)
        {
            return m_Classes[name].Variables;
        }
        inline std::vector<FunctionInfo>& GetFunctions(const std::string& name)
        {
            return m_Classes[name].Functions;
        }

    private:
        std::unordered_map<std::string, ClassInfo> m_Classes = { };
	};


    ///////////////////////////////////////////////////////////////////////////////////
    // Exports
    ///////////////////////////////////////////////////////////////////////////////////
    inline Classes ClassSymbols = {};
    
    extern "C"
    {
        inline EXPORT Insight::Internal::Classes* Insight_GetClasses()
        {
            return &Insight::Internal::ClassSymbols;
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////

}

// Note(Jorben): Used for both function definitions
#define INSIGHT_CLASS_GENERIC(name) \
; \
RUN_FUNCTION(INSIGHT_CONC(name, __COUNTER__), Insight::Internal::ClassSymbols.AddClass, #name)

// Export functions 
#if INSIGHT_EXPORT_CLASSES
#define InsightClass(name) \
INSIGHT_CLASS_GENERIC(name) \
extern "C" \
{ \
inline EXPORT name* Insight_Create##name() \
{ \
    return new name(); \
} \
inline EXPORT void Insight_Delete##name(name* instance) \
{ \
    delete instance; \
} \
} \
REQUIRE_SEMICOLON(INSIGHT_CONC(name, __COUNTER__))

// Regular functions 
#else
#define INSIGHT_CLASS(name) \
INSIGHT_CLASS_GENERIC(name) \
REQUIRE_SEMICOLON(INSIGHT_CONC(name, __COUNTER__))

#endif