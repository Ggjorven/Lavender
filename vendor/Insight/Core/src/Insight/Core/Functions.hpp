#pragma once

#include <string>

#include <Insight/Core/Fields.hpp>
#include <Insight/Core/Variables.hpp>
#include <Insight/Core/Attributes.hpp>

#define INSIGHT_FUNC_DEF(name, returntype, ...) typedef returntype (*Insight##name##Def)(__VA_ARGS__)


// This can't export them as DLL functions
// It only stores the function metadata
// If no parameters use 'void' as the ... arg
#define ExportMemFunc(access, cls, name, returntype, ...) \
Insight::Empty INSIGHT_CONC(InsightExportFuncAttr, __COUNTER__) = {}; \
RUN_FUNCTION_NN(INSIGHT_CONC(name, cls), Insight::Internal::ClassSymbols.AddFunction, #cls, Insight::Internal::FunctionInfo(#name, std::string(#returntype) + '(' + std::string(#__VA_ARGS__) + ')')) \
access: \
[[



// Used to wrap class functions in wrapper functions
#define INSIGHT_FUNC_EXPORT extern "C" EXPORT