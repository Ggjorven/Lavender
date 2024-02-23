#pragma once

#include <stdint.h>

namespace Lavender
{

	enum class RenderingAPI
	{
		None = 0, Vulkan
	};

	// TODO: Add more functionality
	class Renderer
	{
	public:
		static void Init();
		static void Destroy();

		static void OnResize(uint32_t width, uint32_t height);

		inline static RenderingAPI GetAPI() { return s_API; }
		
	private:
		constexpr static const RenderingAPI s_API = RenderingAPI::Vulkan;
	};
}