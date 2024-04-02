#pragma once

#include <string>
#include <memory>
#include <functional>

namespace Lavender
{

	enum class RenderingAPI : uint8_t
	{
		None = 0, Vulkan
	};

	inline static std::string RenderingAPIToString(RenderingAPI api)
	{
		switch (api)
		{
		case RenderingAPI::Vulkan:
			return "Vulkan";

		default:
			break;
		}

		return "Unrecognized API";
	}

	struct RendererSpecification
	{
		uint32_t FramesInFlight = 3;
	};

	struct RenderData
	{
	public:
		uint32_t DrawCalls = 0;

	public:
		void Reset()
		{
			DrawCalls = 0;
		}
	};

	// TODO: Maybe change?
	typedef std::function<void()> RenderFunction;
	typedef std::function<void()> FreeFunction;

}