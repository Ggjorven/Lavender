#pragma once

#include <Lavender/UI/UI.hpp>
#include <Lavender/UI/Colours.hpp>

namespace Lavender::UI::Draw
{

	inline void Underline(bool fullWidth, float offsetX, float offsetY)
	{
		if (fullWidth)
		{
			if (ImGui::GetCurrentWindow()->DC.CurrentColumns != nullptr)
				ImGui::PushColumnsBackground();
			else if (ImGui::GetCurrentTable() != nullptr)
				ImGui::TablePushBackgroundChannel();
		}

		const float width = fullWidth ? ImGui::GetWindowWidth() : ImGui::GetContentRegionAvail().x;
		const ImVec2 cursor = ImGui::GetCursorScreenPos();
		ImGui::GetWindowDrawList()->AddLine(ImVec2(cursor.x + offsetX, cursor.y + offsetY),
			ImVec2(cursor.x + width, cursor.y + offsetY), UI::Colours::BackgroundDark, 1.0f);

		if (fullWidth)
		{
			if (ImGui::GetCurrentWindow()->DC.CurrentColumns != nullptr)
				ImGui::PopColumnsBackground();
			else if (ImGui::GetCurrentTable() != nullptr)
				ImGui::TablePopBackgroundChannel();
		}
	}

	enum class OutlineFlags
	{
		None = 0,
		WhenHovered = 1 << 1,
		WhenActive = 1 << 2,
		WhenInactive = 1 << 3,
		HighlightActive = 1 << 4,

		NoHighlightActive = WhenHovered | WhenActive | WhenInactive,
		NoOutlineInactive = WhenHovered | WhenActive | HighlightActive,
		All = WhenHovered | WhenActive | WhenInactive | HighlightActive,
	};
	DEFINE_BITWISE_OPS(OutlineFlags)

	inline void DrawItemActivityOutline(OutlineFlags flags = OutlineFlags::All, uint32_t colourHighlight = UI::Colours::Accent, float rounding = GImGui->Style.FrameRounding)
	{
		if (UI::IsItemDisabled())
			return;

		auto* drawList = ImGui::GetWindowDrawList();
		const ImRect rect = UI::RectExpanded(UI::GetItemRect(), 1.0f, 1.0f);
		if ((flags & OutlineFlags::WhenActive) && ImGui::IsItemActive())
		{
			if (flags & OutlineFlags::HighlightActive)
				drawList->AddRect(rect.Min, rect.Max, colourHighlight, rounding, 0, 1.5f);
			else
				drawList->AddRect(rect.Min, rect.Max, ImColor(60, 60, 60), rounding, 0, 1.5f);
		}
		else if ((flags & OutlineFlags::WhenHovered) && ImGui::IsItemHovered() && !ImGui::IsItemActive())
			drawList->AddRect(rect.Min, rect.Max, ImColor(60, 60, 60), rounding, 0, 1.5f);
		else if ((flags & OutlineFlags::WhenInactive) && !ImGui::IsItemHovered() && !ImGui::IsItemActive())
			drawList->AddRect(rect.Min, rect.Max, ImColor(50, 50, 50), rounding, 0, 1.0f);
	}

	bool BufferingBar(const std::string& name, float value, const glm::vec2& sizeArg, const glm::vec4& bgCol, const glm::vec4& fgCol);
	bool Spinner(const std::string& name, float radius, uint32_t thickness, const glm::vec4& colour);

}