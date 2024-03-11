#pragma once

//Includes
#include <iostream>
#include <fstream>
#include <memory>
#include <utility>
#include <limits>
#include <algorithm>
#include <functional>
#include <numeric>
#include <thread>
#include <chrono>

#include <cstdint>
#include <cstdlib>
#include <stdexcept>

#include <string>
#include <sstream>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>

#if defined(LV_PLATFORM_WINDOWS)
	#include <Windows.h>
#endif

// TODO: Change the way we do this?
#if defined(LV_DIST)
	#define LV_DISABLE_IMGUI
#endif