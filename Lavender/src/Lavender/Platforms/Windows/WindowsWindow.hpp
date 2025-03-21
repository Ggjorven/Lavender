#pragma once

#include "Lavender/Core/Window.hpp"

#include <GLFW/glfw3.h>

namespace Lavender
{

	class WindowsWindow : public Window
	{
	public:
		WindowsWindow();
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

		bool Init(const WindowSpecification& properties) override;
		void Shutdown() override;

		Ref<RenderingContext> GetRenderingContext() override { return m_RenderingContext; }

	private:
		static void ErrorCallBack(int errorCode, const char* description);

	private:
		static bool s_GLFWinitialized;
		static uint32_t s_Instances;

		GLFWwindow* m_Window = nullptr;
		Ref<RenderingContext> m_RenderingContext = nullptr;
		WindowData m_Data = {};

	};

}