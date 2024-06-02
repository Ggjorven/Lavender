#pragma once

#include <Insight/Core/Help.hpp>

// Main attribute starter
#define InsightAttr ]] \
inline static Insight::Empty INSIGHT_CONC(dummy, __COUNTER__); \
private: