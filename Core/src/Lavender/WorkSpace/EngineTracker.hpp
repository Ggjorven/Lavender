#pragma once

#include "Lavender/Core/Core.hpp"
#include "Lavender/Utils/Utils.hpp"

#include <glm/glm.hpp>

// A namespace for keeping track of engine side stats
namespace Lavender::Track
{

	struct Frame
	{
	public:
		inline static float DeltaTime = 0.0f;

		inline static constexpr const float UpdateInterval = 1.0f;
		// Stats below get updated every 'UpdateInterval'
		inline static uint32_t FPS = 0u;
		inline static float FrameTime = 0.0f;
	}; 

	struct Viewport
	{
	public:
		inline static uint32_t Width = 0u;
		inline static uint32_t Height = 0u;

		inline static glm::uvec2 Position = { 0.0f, 0.0f };
	};

}