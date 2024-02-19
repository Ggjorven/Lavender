#pragma once

#include "Lavender/Core/Events.hpp"
#include "Lavender/Core/Layer.hpp"

#include "Lavender/Core/Window.hpp"

#include "Lavender/ImGui/BaseImGuiLayer.hpp"

#include "Lavender/Renderer/RenderingAPI.hpp"

#include <vector>
#include <memory>
#include <filesystem>

namespace Lavender
{

	struct AppInfo
	{
	public:
		WindowProperties WindowProperties = {};
		mutable APISpecifications APISpecs = {};
		int ArgCount = 0;
		char** Args = nullptr;

		AppInfo() = default;
		AppInfo(int argc, char** argv)
			: ArgCount(argc), Args(argv) {}
	};

	class Application
	{
	public:
		Application(const AppInfo& appInfo);
		virtual ~Application();

		void OnEvent(Event& e);

		void Run();
		inline void Close() { m_Running = false; }

		void AddLayer(Layer* layer);
		void AddOverlay(Layer* layer);

		inline Window& GetWindow() { return *m_Window; }

		template<typename TEvent>
		inline void DispatchEvent(TEvent e = TEvent()) { static_assert(std::is_base_of<Event, TEvent>::value); OnEvent(e); }

		inline static Application& Get() { return *s_Instance; }
		inline static std::filesystem::path GetWorkingDirectory() { return std::filesystem::path(s_Instance->m_AppInfo.Args[0]).parent_path(); }

		inline bool IsMinimized() const { return m_Minimized; }

	private:
		void Init(const AppInfo& appInfo);

		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	private:
		AppInfo m_AppInfo;

		std::unique_ptr<Window> m_Window = nullptr;
		bool m_Running = true;
		bool m_Minimized = false;

		LayerStack m_LayerStack;
		BaseImGuiLayer* m_ImGuiLayer = nullptr;

	private:
		static Application* s_Instance;

	};


	// Implemented by USER/CLIENT
	Application* CreateApplication(int argc, char* argv[]);

}