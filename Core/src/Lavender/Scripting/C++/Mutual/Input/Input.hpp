#pragma once

#include "Lavender/Scripting/C++/Mutual/Core/Functions.hpp"

#include "Lavender/Scripting/C++/Mutual/Input/KeyCodes.hpp"
#include "Lavender/Scripting/C++/Mutual/Input/MouseCodes.hpp"

#include <glm/glm.hpp>

namespace Lavender::Script
{

	class Input
	{
	public:
		inline static bool IsKeyPressed(Key key)
		{
			return InputFunctions::IsKeyPressed(key);
		}

		inline static bool IsMousePressed(MouseButton button)
		{
			return InputFunctions::IsMousePressed(button);
		}

		inline static glm::vec2 GetCursorPosition()
		{
			return *InputFunctions::GetCursorPosition();
		}

		inline static void SetCursorPosition(const glm::vec2& position)
		{
			static glm::vec2 lastPosition = {};
			lastPosition = position;

			InputFunctions::SetCursorPosition(&lastPosition);
		}

		inline static void SetCursorMode(CursorMode cursorMode)
		{
			InputFunctions::SetCursorMode(cursorMode);
		}

	};

}