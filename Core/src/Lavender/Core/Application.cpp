#include "lvpch.h"
#include "Application.hpp"

#include <GLFW/glfw3.h>

#include "Lavender/Core/Logging.hpp"
#include "Lavender/Core/Input/Input.hpp"

#include "Lavender/Renderer/Renderer.hpp"

#include "Lavender/Utils/Profiler.hpp"

#include "Lavender/WorkSpace/Project.hpp" // TODO: Remove
#include "Lavender/WorkSpace/WorkSpace.hpp"
#include "Lavender/WorkSpace/EngineTracker.hpp"

namespace Lavender
{

	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationSpecification& appInfo)
		: m_AppInfo(appInfo)
	{
		s_Instance = this;

		if constexpr (Track::Lavender::Config == WorkSpace::Configuration::Dist)
		{
			// TODO: Change based on whether it's Dist or not
		}
		else
			Track::Lavender::Directory = std::filesystem::path(__argv[0]).parent_path().parent_path().parent_path().parent_path();
	
		Input::Init();
		Log::Init();

		m_Window = Window::Create(appInfo.WindowSpecs);
		m_Window->SetEventCallBack(APP_BIND_EVENT_FN(Application::OnEvent));

		Renderer::Init();
		Track::Viewport::Width = appInfo.WindowSpecs.Width;
		Track::Viewport::Height = appInfo.WindowSpecs.Height;

		m_ImGuiLayer = BaseImGuiLayer::Create();
		m_LayerStack.AddOverlay((Layer*)m_ImGuiLayer);

		WorkSpace::Init();
	}

	Application::~Application()
	{
		Input::Destroy();

		Renderer::Wait();

		for (Layer* layer : m_LayerStack)
		{
			layer->OnDetach();
			delete layer;
		}

		WorkSpace::Destroy();

		Renderer::Destroy();
		m_Window.reset();
	}

	void Application::OnEvent(Event& e)
	{
		EventHandler handler(e);

		handler.Handle<WindowCloseEvent>(APP_BIND_EVENT_FN(Application::OnWindowClose));
		handler.Handle<WindowResizeEvent>(APP_BIND_EVENT_FN(Application::OnWindowResize));

		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
		{
			if (e.Handled)
				break;
			(*it)->OnEvent(e);
		}
	}

	void Application::Run()
	{
		while (m_Running)
		{
			// Delta Time
			float currentTime = (float)Utils::ToolKit::GetTime();
			static float lastTime = 0.0f;

			float deltaTime = currentTime - lastTime;
			lastTime = currentTime;

			Track::Frame::DeltaTime = deltaTime;
			{
				static float timer = 0.0f;
				static uint32_t tempFPS = 0;
				timer += deltaTime;
				tempFPS += 1u;

				if (timer >= Track::Frame::UpdateInterval)
				{
					Track::Frame::FPS = (uint32_t)((float)tempFPS / timer);
					Track::Frame::FrameTime = timer / (float)Track::Frame::FPS * 1000.0f;
					timer = 0.0f;
					tempFPS = 0u;
				}
			}

			// Update & Render
			m_Window->OnUpdate();
			{
				APP_PROFILE_SCOPE("Renderer::Begin");
				Renderer::BeginFrame();
			}
			{
				APP_PROFILE_SCOPE("Update & Render Submit");
				for (Layer* layer : m_LayerStack)
				{
					layer->OnUpdate(deltaTime);
					if (!m_Minimized) layer->OnRender();
				}
			}

			// ImGui 
			if (m_AppInfo.EnableUI && !m_Minimized)
			{
				APP_PROFILE_SCOPE("ImGui Submit");
				Renderer::Submit([this]() 
				{
					APP_PROFILE_SCOPE("ImGui Pass");
					m_ImGuiLayer->Begin();
					for (Layer* layer : m_LayerStack)
					{
						layer->OnUIRender();
					}
					m_ImGuiLayer->End();
				});
			}

			{
				APP_PROFILE_SCOPE("Renderer::End");
				Renderer::EndFrame();
				m_Window->OnRender();
			}
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

		Lavender::Track::Viewport::Width = e.GetWidth();
		Lavender::Track::Viewport::Height = e.GetHeight();

		Renderer::OnResize(e.GetWidth(), e.GetHeight());
		m_ImGuiLayer->Resize(e.GetWidth(), e.GetHeight());

		m_Minimized = false;
		return false;
	}

}