#pragma once

#include <string>
#include <memory>

#include <glm/glm.hpp>

#include "Lavender/Core/Input/MouseCodes.hpp"
#include "Lavender/Core/Input/KeyCodes.hpp"

namespace Lavender
{

	class Input
	{
	public:
		static void Init();
		static void Destroy();

		inline static bool IsKeyPressed(Key keycode) { return s_Instance->IsKeyPressedImplementation(keycode); }
		inline static bool IsMousePressed(MouseButton button) { return s_Instance->IsMousePressedImplementation(button); }

		inline static glm::vec2 GetCursorPosition() { return s_Instance->GetCursorPositionImplementation(); }
		inline static void SetCursorPosition(glm::vec2 position) { s_Instance->SetCursorPositionImplementation(position); }

		inline static void SetCursorMode(CursorMode mode) { s_Instance->SetCursorModeImplementation(mode); }

	protected:
		//Implementation functions dependant on platform
		virtual bool IsKeyPressedImplementation(Key keycode) = 0;
		virtual bool IsMousePressedImplementation(MouseButton button) = 0;

		virtual glm::vec2 GetCursorPositionImplementation() = 0;
		virtual void SetCursorPositionImplementation(glm::vec2 position) = 0;

		virtual void SetCursorModeImplementation(CursorMode mode) = 0;

	private:
		static Input* s_Instance;
	};


}