#pragma once

#include <Insight/Core/If.hpp>
#include <Insight/Core/Help.hpp>
#include <Insight/Core/Class.hpp>
#include <Insight/Core/Variables.hpp>
#include <Insight/Core/Attributes.hpp>

#define INSIGHT_private 1
#define INSIGHT_public 0

// Used for all field functions
#define INSIGHT_FIELD_GENERIC(cls, type, name) \
private: \
RUN_FUNCTION_NN(INSIGHT_CONC(name, cls), Insight::Internal::ClassSymbols.AddVariable, #cls, Insight::Internal::VariableInfo::Create<type>(#name))

#define INSIGHT_PUBLIC_FIELD(cls, type, name) \
INSIGHT_FIELD_GENERIC(cls, type, name) \
public: 

#define INSIGHT_PRIVATE_FIELD(cls, type, name) \
INSIGHT_FIELD_GENERIC(cls, type, name) \
public: \
type Get##name() \
{ \
	return name;\
}\
void Set##name(type val) \
{ \
	name = val; \
} \
private:


// Note(Jorben): Only supports public/private vars
#define ExportField(access, cls, type, name) Insight::Empty INSIGHT_CONC(InsightExportVarAttr, __COUNTER__) = {}; \
INSIGHT_IF(INSIGHT_CONC(INSIGHT, access), INSIGHT_PUBLIC_FIELD(cls, type, name), INSIGHT_PRIVATE_FIELD(cls, type, name)) \
[[





#define INSIGHT_PUBLIC_FIELD_EXPORT(cls, type, name) \
extern "C" \
{ \
EXPORT type Insight_Get##name##cls(cls* instance) \
{ \
	return instance->name; \
} \
EXPORT void Insight_Set##name##cls(cls* instance, type val) \
{ \
	instance->name = val; \
} \
}

#define INSIGHT_PRIVATE_FIELD_EXPORT(cls, type, name) \
extern "C" \
{ \
EXPORT type Insight_Get##name##cls(cls* instance) \
{ \
	return instance->Get##name(); \
} \
EXPORT void Insight_Set##name##cls(cls* instance, type val) \
{ \
	instance->Set##name(val); \
} \
}

#define InsightExportField(access, cls, type, name) \
INSIGHT_IF(INSIGHT_CONC(INSIGHT, access), INSIGHT_PUBLIC_FIELD_EXPORT(cls, type, name), INSIGHT_PRIVATE_FIELD_EXPORT(cls, type, name)) \
REQUIRE_SEMICOLON(INSIGHT_CONC(cls, name))