#pragma once

#include "Lavender/Core/Core.hpp"
#include "Lavender/Utils/Utils.hpp"

#include "Lavender/UI/Colours.hpp"

#include <imgui.h>
#include <imgui_internal.h>

namespace Lavender::UI::Draw
{

	void Underline(bool fullWidth = false, float offsetX = 0.0f, float offsetY = -1.0f);

	enum class OutlineFlags
	{
		None = 0,
		WhenHovered = BIT(1),
		WhenActive = BIT(2),
		WhenInactive = BIT(3),
		HighlightActive = BIT(4),

		NoHighlightActive = WhenHovered | WhenActive | WhenInactive,
		NoOutlineInactive = WhenHovered | WhenActive | HighlightActive,
		All = WhenHovered | WhenActive | WhenInactive | HighlightActive,
	};
	DEFINE_BITWISE_OPS(OutlineFlags)

	void DrawItemActivityOutline(OutlineFlags flags = OutlineFlags::All, uint32_t colourHighlight = UI::Colours::Accent, float rounding = GImGui->Style.FrameRounding);

	bool BufferingBar(const std::string& name, float value, const glm::vec2& sizeArg, const glm::vec4& bgCol, const glm::vec4& fgCol);
	bool Spinner(const std::string& name, float radius, uint32_t thickness, const glm::vec4& colour);

}