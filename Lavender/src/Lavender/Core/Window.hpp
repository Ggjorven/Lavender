#pragma once

#include <string>
#include <functional>
#include <memory>

#include "Lavender/Core/Events.hpp"

#include "Lavender/Renderer/RenderingContext.hpp"

namespace Lavender
{

	typedef std::function<void(Ref<Event>&)> EventCallBackFunction;

	struct WindowSpecification
	{
		std::string Name;
		uint32_t Width;
		uint32_t Height;

		//bool Titlebar = true; // TODO(Jorben): Implement using an updated GLFW branch
		bool VSync = true;

		bool CustomPos = false;
		uint32_t X = 0u;
		uint32_t Y = 0u;

		WindowSpecification(std::string name = "Lavender Window", uint32_t width = 1280u, uint32_t height = 720u)
			: Name(name), Width(width), Height(height)
		{
		}
	};

	struct WindowData
	{
		std::string Name;
		uint32_t Width;
		uint32_t Height;

		bool Vsync = false;
		EventCallBackFunction CallBack;

		WindowData(std::string name = "Lavender Window", uint32_t width = 1280, uint32_t height = 720)
			: Name(name), Width(width), Height(height)
		{
		}

		WindowData operator = (WindowSpecification const& properties)
		{
			WindowData newData;
			newData.Name = properties.Name;
			newData.Width = properties.Width;
			newData.Height = properties.Height;
			newData.Vsync = properties.VSync;
			return newData;
		}
	};

	class Window
	{
	public:
		virtual ~Window() = default;

		virtual void SetEventCallBack(EventCallBackFunction func) = 0;

		virtual void OnUpdate() = 0;
		virtual void OnRender() = 0;

		// Actual window size
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		// Extra
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void SetTitle(const std::string& title) = 0;

		virtual void* GetNativeWindow() const = 0;

		virtual bool Init(const WindowSpecification& properties = WindowSpecification()) = 0; 
		virtual void Shutdown() = 0;

		virtual Ref<RenderingContext> GetRenderingContext() = 0;

		static std::unique_ptr<Window> Create();
	};

}