#include "lvpch.h"
#include "Application.hpp"

#include <GLFW/glfw3.h>

#include "Lavender/Core/Logging.hpp"
#include "Lavender/Renderer/Renderer.hpp"

#include "Lavender/Utils/Profiler.hpp"

namespace Lavender
{

	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationSpecification& appInfo)
		: m_AppInfo(appInfo)
	{
		Init(appInfo);
	}

	Application::~Application()
	{
		for (Layer* layer : m_LayerStack)
		{
			layer->OnDetach();
			delete layer;
		}

		Renderer::Destroy();
		m_Window->Shutdown();
	}

	void Application::OnEvent(std::shared_ptr<Event>& e)
	{
		m_EventQueue.push(e);
	}

	void Application::Run()
	{
		while (m_Running)
		{
			// Delta Time
			float currentTime = (float)glfwGetTime();
			static float lastTime = 0.0f;

			float deltaTime = currentTime - lastTime;
			lastTime = currentTime;

			// Update & Render
			m_Window->OnUpdate();
			HandleEvents();

			Renderer::BeginFrame();
			{
				LV_PROFILE_SCOPE("Update & Render");
				for (Layer* layer : m_LayerStack)
				{
					layer->OnUpdate(deltaTime);
					layer->OnRender();
				}
			}

			{
				LV_PROFILE_SCOPE("ImGui Submission");
				{
					LV_PROFILE_SCOPE("ImGui::Begin");
					m_ImGuiLayer->Begin();
				}
				for (Layer* layer : m_LayerStack)
					layer->OnImGuiRender();
				{
					LV_PROFILE_SCOPE("ImGui::End");
					m_ImGuiLayer->End();
				}
			}

			Renderer::EndFrame();
			m_Window->OnRender();
		}
	}

	void Application::AddLayer(Layer* layer)
	{
		m_LayerStack.AddLayer(layer);
	}

	void Application::AddOverlay(Layer* layer)
	{
		m_LayerStack.AddOverlay(layer);
	}

	void Application::Init(const ApplicationSpecification& appInfo)
	{
		s_Instance = this;

		Log::Init();
		
		m_Window = Window::Create();
		m_Window->Init(appInfo.WindowSpecs);
		m_Window->SetEventCallBack(LV_BIND_EVENT_FN(Application::OnEvent));

		Renderer::Init(appInfo.RenderSpecs);

		m_ImGuiLayer = BaseImGuiLayer::Create();
		AddOverlay(m_ImGuiLayer);
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return false;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return true;
		}

		Renderer::OnResize(e.GetWidth(), e.GetHeight());
		m_Minimized = false;
		return false;
	}

	void Application::HandleEvents()
	{
		while (!m_EventQueue.empty())
		{
			std::shared_ptr<Event> rawEvent = m_EventQueue.front();
			Event& e = *rawEvent;

			EventHandler handler(e);

			handler.Handle<WindowCloseEvent>(LV_BIND_EVENT_FN(Application::OnWindowClose));
			handler.Handle<WindowResizeEvent>(LV_BIND_EVENT_FN(Application::OnWindowResize));

			for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
			{
				if (e.Handled)
					break;
				(*it)->OnEvent(e);
			}

			m_EventQueue.pop();
		}
	}

}