#pragma once

#include <functional>

#include "Lavender/Core/Core.hpp"

namespace Lavender
{

	typedef std::function<void()> RenderFunction;
	typedef std::function<void()> FreeFunction;

	enum class RenderingAPI : uint8_t
	{
		None = 0, Vulkan
	};
	inline std::string RenderingAPIToString(RenderingAPI api)
	{
		switch (api)
		{
		case RenderingAPI::Vulkan: return "Vulkan";
		
		default:
			APP_LOG_ERROR("Invalid RenderingAPI passed in.");
			break;
		}

		return {};
	}

	struct RendererSpecification
	{
	public:
		enum class BufferMode : uint8_t
		{
			None = 0, Single = 1, Double = 2, Triple = 3
		};
	public:
		inline static constexpr const RenderingAPI API = RenderingAPI::Vulkan;
		inline static constexpr const BufferMode BufferCount = BufferMode::Triple;
	};

	struct RenderData
	{
	public:
		uint32_t DrawCalls = 0;		// During runtime it is x2 since we have a DepthPrepass and FinalShading, so to get the actual number do / 2

	public:
		inline void Reset()
		{
			DrawCalls = 0;
		}
	};

}