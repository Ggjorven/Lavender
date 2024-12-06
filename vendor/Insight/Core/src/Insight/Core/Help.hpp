#pragma once

// We use a helper function incase 'id' is some kind of function that generates something
// Now it has to be expanded.
#define RUN_FUNCTION_HELPER(id, functionName, ...) \
namespace \
{ \
inline void Run##id() \
{ \
	functionName(__VA_ARGS__); \
} \
\
static int dummy##id = (Run##id(), 0); \
}

#define RUN_FUNCTION_NN_HELPER(id, functionName, ...) \
inline static void Run##id() \
{ \
    functionName(__VA_ARGS__); \
} \
inline static const int dummy##id = (Run##id(), 0);

// Note(Jorben): This runs the function outside of a scope
// The 'id' parameter is just to ensure you can run the same function multiple times 
// 'id' can be an int or whatever as long as it is unique
#define RUN_FUNCTION(id, functionName, ...) RUN_FUNCTION_HELPER(id, functionName, __VA_ARGS__)
// Note(Jorben): NN stands for No Namespace
#define RUN_FUNCTION_NN(id, functionName, ...) RUN_FUNCTION_NN_HELPER(id, functionName, __VA_ARGS__)



// Macros to access individual arguments
/*
#define GET_NTH_ARG_1(ARG1, ...) ARG1
#define GET_NTH_ARG_2(ARG1, ARG2, ...) ARG2
#define GET_NTH_ARG_3(ARG1, ARG2, ARG3, ...) ARG3
#define GET_NTH_ARG_4(ARG1, ARG2, ARG3, ARG4, ...) ARG4
#define GET_NTH_ARG_5(ARG1, ARG2, ARG3, ARG4, ARG5, ...) ARG5
#define GET_NTH_ARG_6(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ...) ARG6
#define GET_NTH_ARG_7(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ...) ARG7
#define GET_NTH_ARG_8(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8, ...) ARG8
#define GET_NTH_ARG_9(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8, ARG9, ...) ARG9

#define INSIGHT_GET_NTH_ARG(N, ...) GET_NTH_ARG_##N(__VA_ARGS__)
*/



/*
#define GET_NTH_ARG_1(ARG1, ...) ARG1
#define GET_NTH_ARG_2(ARG1, ARG2, ...) ARG2
#define GET_NTH_ARG_3(ARG1, ARG2, ARG3, ...) ARG3

#define GET_NTH_ARG_HELPER(N, FIRST_ARG, ...) GET_NTH_ARG_##N(FIRST_ARG)
#define GET_NTH_ARG(N, ...) GET_NTH_ARG_HELPER(N, __VA_ARGS__)
*/



#define REQUIRE_SEMICOLON(id) \
struct id \
{} 

#define STR_HELPER(str) #str
#define STRINGIFY(str) STR_HELPER(str)

namespace Insight
{
	typedef char Empty;
}