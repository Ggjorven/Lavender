#pragma once

#include "Lavender/Core/Events.hpp"
#include "Lavender/Core/Layer.hpp"

#include "Lavender/Core/Window.hpp"

#include "Lavender/ImGui/BaseImGuiLayer.hpp"

#include "Lavender/Renderer/RenderConfig.hpp"

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
		RendererSpecification RenderSpecs = { };

		ApplicationSpecification() = default;
	};

	class Application
	{
	public:
		Application(const ApplicationSpecification& appInfo);
		virtual ~Application();

		void OnEvent(Ref<Event>& e);

		void Run();
		inline void Close() { m_Running = false; }

		void AddLayer(Layer* layer);
		void AddOverlay(Layer* layer);

		inline Window& GetWindow() { return *m_Window; }
		inline BaseImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		inline static Application& Get() { return *s_Instance; }

		inline bool IsMinimized() const { return m_Minimized; }

		void HandleEvents();
	
	private:
		void Init(const ApplicationSpecification& appInfo);

		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	private:
		ApplicationSpecification m_AppInfo;

		std::unique_ptr<Window> m_Window = nullptr;
		bool m_Running = true;
		bool m_Minimized = false;

		std::queue<Ref<Event>> m_EventQueue = { };

		LayerStack m_LayerStack;
		BaseImGuiLayer* m_ImGuiLayer = nullptr;

	private:
		static Application* s_Instance;

	};


	// Implemented by USER/CLIENT
	Application* CreateApplication(int argc, char* argv[]);

}