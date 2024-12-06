#pragma once

#include <stdint.h>

#include <imgui.h>
#include <imgui_internal.h>

#include <glm/glm.hpp>

namespace Lavender::UI::Colours
{

	#define LV_U32_COLOUR(R, G, B, A) (((uint32_t)(A) << 24) | ((uint32_t)(B) << 16) | ((uint32_t)(G) << 8) | ((uint32_t)(R) << 0))
	
	// TODO: Colours
	inline constexpr const uint32_t Accent =					LV_U32_COLOUR(236, 158, 36, 255);

	inline constexpr const uint32_t Background =				LV_U32_COLOUR(36, 36, 36, 255);
	inline constexpr const uint32_t BackgroundDark =			LV_U32_COLOUR(26, 26, 26, 255);

	inline constexpr static glm::vec4 ConvertU32Colour(uint32_t colour)
	{
		float a = ((colour >> 24) & 0xFF) / 255.0f;
		float b = ((colour >> 16) & 0xFF) / 255.0f;
		float g = ((colour >> 8) & 0xFF) / 255.0f;
		float r = ((colour >> 0) & 0xFF) / 255.0f;

		return glm::vec4(r, g, b, a);
	}

}