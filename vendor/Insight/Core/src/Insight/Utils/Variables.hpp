#pragma once

#include <Insight/Core/Class.hpp>
#include <Insight/Core/Fields.hpp>

namespace Insight
{

    #define INSIGHT_TYPE(underlyingType, newType) \
    typedef underlyingType newType; \
    using Get##newType##Fn = newType (*)(void* instance); \
    using Set##newType##Fn = void (*)(void* instance, newType value)

    INSIGHT_TYPE(void*, Default);
    INSIGHT_TYPE(char, Char);
    INSIGHT_TYPE(signed char, SChar);
    INSIGHT_TYPE(unsigned char, UChar);
    INSIGHT_TYPE(short, Short);
    INSIGHT_TYPE(unsigned short, UShort);
    INSIGHT_TYPE(int, Int);
    INSIGHT_TYPE(unsigned int, UInt);
    INSIGHT_TYPE(long, Long);
    INSIGHT_TYPE(unsigned long, ULong);
    INSIGHT_TYPE(long long, LongLong);
    INSIGHT_TYPE(unsigned long long, ULongLong);
    INSIGHT_TYPE(float, Float);
    INSIGHT_TYPE(double, Double);
    INSIGHT_TYPE(long double, LongDouble);
    INSIGHT_TYPE(bool, Bool);



    struct OpaqueVariable
    {
    public:
        OpaqueVariable() = default;
        OpaqueVariable(const Internal::VariableInfo& var, void* instance, SetDefaultFn setter, GetDefaultFn getter)
            : m_Variable(var), m_Instance(instance), m_Setter(setter), m_Getter(getter)
        {
        }
        virtual ~OpaqueVariable() = default;

        inline Internal::VariableInfo& GetVariableInfo() { return m_Variable; }
        inline void*& GetInstance() { return m_Instance; }
        inline SetDefaultFn& GetSetter() { return m_Setter; }
        inline GetDefaultFn& GetGetter() { return m_Getter; }

    private:
        Internal::VariableInfo m_Variable = {};
        void* m_Instance = nullptr;
        
        SetDefaultFn m_Setter = nullptr;
        GetDefaultFn m_Getter = nullptr;
    };

    template <typename T>
    struct Variable
    {
    public:
        Variable() = default;
        Variable(const Internal::VariableInfo& var, void* instance, void (*setter)(void*, T), T (*getter)(void*))
            : m_Variable(var), m_Instance(instance), m_Setter(setter), m_Getter(getter)
        {
        }
        virtual ~Variable() = default;

        Variable<T>& operator = (const T& newValue)
        {
            m_Setter(m_Instance, newValue);
            return *this;
        }

        operator T() const
        {
            return m_Getter(m_Instance);
        }

        inline Internal::VariableInfo& GetVariableInfo() { return m_Variable; }

    private:
        Internal::VariableInfo m_Variable = {};
        void* m_Instance = nullptr;

        void (*m_Setter)(void*, T) = nullptr;
        T(*m_Getter)(void*) = nullptr;
    };

    #define INSIGHT_VARIABLE_DEFINE(type) \
    namespace Insight \
    { \
    template <> \
    struct Variable<type> \
    { \
    public: \
        Variable() = default; \
        Variable(const Internal::VariableInfo& var, void* instance, Set##type##Fn setter, Get##type##Fn getter) \
            : m_Variable(var), m_Instance(instance), m_Setter(setter), m_Getter(getter) \
        { \
        } \
        virtual ~Variable() = default; \
        \
        Variable<type>& operator = (const type& newValue) \
        { \
            m_Setter(m_Instance, newValue); \
            return *this; \
        } \
        \
        operator type() const \
        { \
            return m_Getter(m_Instance); \
        } \
        \
        inline Internal::VariableInfo& GetVariableInfo() { return m_Variable; } \
        \
    private: \
        Internal::VariableInfo m_Variable = {}; \
        void* m_Instance = nullptr; \
        \
        Set##type##Fn m_Setter = nullptr; \
        Get##type##Fn m_Getter = nullptr; \
    }; \
    } \
    REQUIRE_SEMICOLON(INSIGHT_CONC(type, __COUNTER__))

    #define INSIGHT_VARIABLE_DEFINE_NEW(type) \
    namespace Insight \
    { \
    template <> \
    struct Variable<type> \
    { \
    public: \
        Variable() = default; \
        Variable(const Internal::VariableInfo& var, void* instance, void (*setter)(void*, type), type (*getter)(void*)) \
            : m_Variable(var), m_Instance(instance), m_Setter(setter), m_Getter(getter) \
        { \
        } \
        virtual ~Variable() = default; \
        \
        Variable<type>& operator = (const type& newValue) \
        { \
            m_Setter(m_Instance, newValue); \
            return *this; \
        } \
        \
        operator type() const \
        { \
            return Getter(m_Instance); \
        } \
        \
        inline Internal::VariableInfo& GetVariableInfo() { return m_Variable; } \
        \
    private: \
        Internal::VariableInfo m_Variable = {}; \
        void* m_Instance = nullptr; \
        \
        void (*m_Setter)(void*, type) = nullptr; \
        type (*m_Getter)(void*) = nullptr; \
    } \
    }; \
    REQUIRE_SEMICOLON(INSIGHT_CONC(type, __COUNTER__))



    #define INSIGHT_DECL_VARIABLE(type) \
    std::vector<Variable<type>> type##s = { }

    struct DefaultVariableCollection
    {
    public:
        DefaultVariableCollection() = default;
        DefaultVariableCollection(const std::vector<OpaqueVariable>& vars);
        virtual ~DefaultVariableCollection() = default;

        void Add(OpaqueVariable var);
        void Add(const std::vector<OpaqueVariable> vars);

        void PrintInfo();

        INSIGHT_DECL_VARIABLE(Char);
        INSIGHT_DECL_VARIABLE(SChar);
        INSIGHT_DECL_VARIABLE(UChar);
        INSIGHT_DECL_VARIABLE(Short);
        INSIGHT_DECL_VARIABLE(UShort);
        INSIGHT_DECL_VARIABLE(Int);
        INSIGHT_DECL_VARIABLE(UInt);
        INSIGHT_DECL_VARIABLE(Long);
        INSIGHT_DECL_VARIABLE(ULong);
        INSIGHT_DECL_VARIABLE(LongLong);
        INSIGHT_DECL_VARIABLE(ULongLong);
        INSIGHT_DECL_VARIABLE(Float);
        INSIGHT_DECL_VARIABLE(Double);
        INSIGHT_DECL_VARIABLE(LongDouble);
        INSIGHT_DECL_VARIABLE(Bool);
    };

}

using namespace Insight;

INSIGHT_VARIABLE_DEFINE(Char);
INSIGHT_VARIABLE_DEFINE(SChar);
INSIGHT_VARIABLE_DEFINE(UChar);
INSIGHT_VARIABLE_DEFINE(Short);
INSIGHT_VARIABLE_DEFINE(UShort);
INSIGHT_VARIABLE_DEFINE(Int);
INSIGHT_VARIABLE_DEFINE(UInt);
INSIGHT_VARIABLE_DEFINE(Long);
INSIGHT_VARIABLE_DEFINE(ULong);
INSIGHT_VARIABLE_DEFINE(LongLong);
INSIGHT_VARIABLE_DEFINE(ULongLong);
INSIGHT_VARIABLE_DEFINE(Float);
INSIGHT_VARIABLE_DEFINE(Double);
INSIGHT_VARIABLE_DEFINE(LongDouble);
INSIGHT_VARIABLE_DEFINE(Bool);