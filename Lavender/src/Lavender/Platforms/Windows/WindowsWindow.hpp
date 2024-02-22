#pragma once

#include "Lavender/Core/Window.hpp"

//#include "Lavender/Renderer/GraphicsContext.hpp"

#include <GLFW/glfw3.h>

namespace Lavender
{

	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProperties properties);
		virtual ~WindowsWindow();

		void SetEventCallBack(EventCallBackFunction func) override { m_Data.CallBack = func; }

		void OnUpdate() override;
		void OnRender() override;

		uint32_t GetWidth() const override { return m_Data.Width; }
		uint32_t GetHeight() const override { return m_Data.Height; }

		void SetVSync(bool enabled) override;
		bool IsVSync() const override { return m_Data.Vsync; }

		void SetTitle(const std::string& title) override;

		void* GetNativeWindow() const override { return (void*)m_Window; }

	private:
		bool Init(WindowProperties properties);
		void Shutdown();

		static void ErrorCallBack(int errorCode, const char* description);

	private:
		static bool s_GLFWinitialized;
		static uint32_t s_Instances;

		GLFWwindow* m_Window = nullptr;
		//std::unique_ptr<GraphicsContext> m_GraphicsContext = nullptr;
		WindowData m_Data = {};

	};

}