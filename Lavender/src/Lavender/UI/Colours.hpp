#pragma once

#include <stdint.h>

#include <imgui.h>
#include <imgui_internal.h>

#include <glm/glm.hpp>

namespace Lavender::UI::Colours
{

	#define LV_U32_COLOUR(R, G, B, A) (((uint32_t)(A) << 24) | ((uint32_t)(B) << 16) | ((uint32_t)(G) << 8) | ((uint32_t)(R) << 0))
	
	constexpr uint32_t Accent =					LV_U32_COLOUR(236, 158, 36, 255);
	constexpr uint32_t Highlight =				LV_U32_COLOUR(39, 185, 242, 255);
	constexpr uint32_t NiceBlue =				LV_U32_COLOUR(83, 232, 254, 255);
	constexpr uint32_t Compliment =				LV_U32_COLOUR(78, 151, 166, 255);
	constexpr uint32_t Background =				LV_U32_COLOUR(36, 36, 36, 255);
	constexpr uint32_t BackgroundBitDarker =	LV_U32_COLOUR(36, 36, 36, 210);
	constexpr uint32_t BackgroundDark =			LV_U32_COLOUR(26, 26, 26, 255);
	constexpr uint32_t Titlebar =				LV_U32_COLOUR(21, 21, 21, 255);
	constexpr uint32_t PropertyField =			LV_U32_COLOUR(15, 15, 15, 255);
	constexpr uint32_t Text =					LV_U32_COLOUR(192, 192, 192, 255);
	constexpr uint32_t TextBrighter =			LV_U32_COLOUR(210, 210, 210, 255);
	constexpr uint32_t TextDarker =				LV_U32_COLOUR(128, 128, 128, 255);
	constexpr uint32_t TextError =				LV_U32_COLOUR(230, 51, 51, 255);
	constexpr uint32_t Muted =					LV_U32_COLOUR(77, 77, 77, 255);
	constexpr uint32_t GroupHeader =			LV_U32_COLOUR(47, 47, 47, 255);
	constexpr uint32_t Selection =				LV_U32_COLOUR(237, 192, 119, 255);
	constexpr uint32_t SelectionMuted =			LV_U32_COLOUR(237, 201, 142, 23);
	constexpr uint32_t BackgroundPopup =		LV_U32_COLOUR(50, 50, 50, 255);
	constexpr uint32_t NearBlack =				LV_U32_COLOUR(18, 18, 18, 138);

	constexpr uint32_t AlphaTint =				LV_U32_COLOUR(110, 65, 138, 68);
	constexpr uint32_t DarkTint =				LV_U32_COLOUR(112, 52, 145, 102);
	constexpr uint32_t LightTint =				LV_U32_COLOUR(110, 65, 138, 150);
	constexpr uint32_t LighterTint =			LV_U32_COLOUR(110, 65, 138, 160);
	constexpr uint32_t LightestTint =			LV_U32_COLOUR(219, 199, 234, 255);

	inline static glm::vec4 ConvertU32Colour(uint32_t colour)
	{
		float a = ((colour >> 24) & 0xFF) / 255.0f;
		float b = ((colour >> 16) & 0xFF) / 255.0f;
		float g = ((colour >> 8) & 0xFF) / 255.0f;
		float r = ((colour >> 0) & 0xFF) / 255.0f;

		return glm::vec4(r, g, b, a);
	}

}