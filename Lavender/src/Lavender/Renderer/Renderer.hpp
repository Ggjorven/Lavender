#pragma once

#include <stdint.h>
#include <memory>

#include "Lavender/Core/Application.hpp"

#include "Lavender/Renderer/RenderConfig.hpp"

namespace Lavender
{

	// TODO: Add more functionality
	class Renderer
	{
	public:
		static void Init(const RendererSpecification& specs);
		static void Destroy();

		static void Display();

		static void OnResize(uint32_t width, uint32_t height);

		inline static std::shared_ptr<RenderingContext> GetContext() { return Application::Get().GetWindow().GetRenderingContext(); }

		inline constexpr static const RenderingAPI GetAPI() { return s_API; }
		
	private:
		inline constexpr static const RenderingAPI s_API = RenderingAPI::Vulkan;
	};
}