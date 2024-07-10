#pragma once

#include "Lavender/Core/Events.hpp"
#include "Lavender/Core/Layer.hpp"

#include "Lavender/Core/Window.hpp"

#include "Lavender/UI/BaseImGuiLayer.hpp"

#include <vector>
#include <memory>
#include <queue>
#include <filesystem>

namespace Lavender
{

	struct ApplicationSpecification
	{
	public:
		WindowSpecification WindowSpecs = { };
		bool EnableUI = true;

	public:
		ApplicationSpecification() = default;
	};

	class Application
	{
	public:
		Application(const ApplicationSpecification& appInfo);
		virtual ~Application();

		void OnEvent(Event& e);

		void Run();
		inline void Close() { m_Running = false; }

		void AddLayer(Layer* layer);
		void AddOverlay(Layer* layer);

		inline ApplicationSpecification GetSpecification() const { return m_AppInfo; }
		inline Window& GetWindow() { return *m_Window; }

		inline bool IsMinimized() const { return m_Minimized; }

		inline static Application& Get() { return *s_Instance; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	private:
		ApplicationSpecification m_AppInfo = {};

		Unique<Window> m_Window = nullptr;
		bool m_Running = true;
		bool m_Minimized = false;

		LayerStack m_LayerStack = {};
		BaseImGuiLayer* m_ImGuiLayer = nullptr;

	private:
		static Application* s_Instance;

	};



	// Implemented by USER/CLIENT
	Application* CreateApplication(int argc, char* argv[]);

}