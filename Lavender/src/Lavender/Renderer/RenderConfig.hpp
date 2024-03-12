#pragma once

#include <memory>
#include <functional>

namespace Lavender
{

	enum class RenderingAPI : uint8_t
	{
		None = 0, Vulkan
	};

	struct RendererSpecification
	{
		uint32_t FramesInFlight = 3;

		// TODO: Add more 
	};

	struct RenderData
	{
	public:
		uint32_t DrawCalls = 0;
		// TODO: Add more...

	public:
		void Reset()
		{
			DrawCalls = 0;
			// TODO: Add more...
		}
	};

	// TODO: Maybe change?
	typedef std::function<void()> RenderFunction;

}