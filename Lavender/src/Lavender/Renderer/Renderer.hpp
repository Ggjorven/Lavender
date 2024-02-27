#pragma once

#include <stdint.h>
#include <memory>

#include "Lavender/Core/Application.hpp"

#include "Lavender/Renderer/RenderConfig.hpp"

namespace Lavender
{

	class RenderCommandBuffer;
	class RenderInstance;

	// TODO: Add more functionality
	class Renderer
	{
	public:
		static void Init(const RendererSpecification& specs);
		static void Destroy();

		static void BeginFrame();
		static void EndFrame();

		static void Submit(RenderFunction function);
		static void WaitFor(Ref<RenderCommandBuffer> commandBuffer); // TODO: Remove this and replace with something else

		static void OnResize(uint32_t width, uint32_t height);

		inline static Ref<RenderingContext> GetContext() { return Application::Get().GetWindow().GetRenderingContext(); }
		static RenderInstance* GetInstance();

		inline constexpr static const RenderingAPI GetAPI() { return s_API; }
		inline static RendererSpecification GetSpecification() { return s_Specifications; }
		
	private:
		inline constexpr static const RenderingAPI s_API = RenderingAPI::Vulkan;
		static RendererSpecification s_Specifications;
	};

}