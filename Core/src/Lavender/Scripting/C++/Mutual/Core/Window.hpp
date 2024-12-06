#pragma once

#include <cstdint>
#include <string>

#include "Lavender/Scripting/C++/Mutual/Core/Functions.hpp"

namespace Lavender::Script
{

	class Window
	{
	public:
		// Size
		static uint32_t GetWidth()
		{
			return WindowFunctions::GetWidth();
		}

		static uint32_t GetHeight()
		{
			return WindowFunctions::GetHeight();
		}

		static UVec2 GetSize()
		{
			return { GetWidth(), GetHeight() };
		}

		// Positions
		static uint32_t GetXPosition()
		{
			return WindowFunctions::GetXPosition();
		}

		static uint32_t GetYPosition()
		{
			return WindowFunctions::GetYPosition();
		}
		
		static UVec2 GetPosition()
		{
			return { GetXPosition(), GetYPosition() };
		}

		// Utils
		static uint32_t GetMonitorWidth()
		{
			return WindowFunctions::GetMonitorWidth();
		}

		static uint32_t GetMonitorHeight()
		{
			return WindowFunctions::GetMonitorHeight();
		}

		static void SetVSync(bool enabled = true)
		{
			WindowFunctions::SetVSync(enabled);
		}

		static bool IsVSync()
		{
			return WindowFunctions::IsVSync();
		}

		static void SetTitle(const std::string& title)
		{
			WindowFunctions::SetTitle(title.c_str());
		}

	};

}