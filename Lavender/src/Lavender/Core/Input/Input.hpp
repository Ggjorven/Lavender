#pragma once

#include <string>
#include <memory>

#include <glm/glm.hpp>

#include "Lavender/Core/Input/MouseCodes.hpp"
#include "Lavender/Core/Input/KeyCodes.hpp"

namespace Lavender
{

	enum class CursorMode : uint32_t
	{
		Shown = 0x00034001,
		Hidden = 0x00034002,
		Disabled = 0x00034003,
		Captured = 0x00034004
	};

	class Input
	{
	public:
		inline static bool IsKeyPressed(Key keycode) { return s_Instance->IsKeyPressedImplementation(keycode); }
		inline static bool IsMousePressed(MouseButton button) { return s_Instance->IsMousePressedImplementation(button); }

		inline static glm::vec2 GetMousePosition() { return s_Instance->GetMousePositionImplementation(); }

		inline static void SetCursorPosition(glm::vec2 position) { s_Instance->SetCursorPositionImplementation(position); }
		inline static void SetCursorMode(CursorMode mode) { s_Instance->SetCursorModeImplementation(mode); }

	protected:
		//Implementation functions dependent on platform
		virtual bool IsKeyPressedImplementation(Key keycode) = 0;
		virtual bool IsMousePressedImplementation(MouseButton button) = 0;

		virtual glm::vec2 GetMousePositionImplementation() = 0;

		virtual void SetCursorPositionImplementation(glm::vec2 position) = 0;
		virtual void SetCursorModeImplementation(CursorMode mode) = 0;

	private:
		static std::unique_ptr<Input> s_Instance;
	};

}