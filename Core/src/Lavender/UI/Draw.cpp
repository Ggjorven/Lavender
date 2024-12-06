#include "lvpch.h"
#include "Style.hpp"

#include "Lavender/Core/Logging.hpp"
#include "Draw.hpp"

#include "Lavender/UI/UI.hpp"

namespace Lavender::UI::Draw
{

	void Underline(bool fullWidth, float offsetX, float offsetY)
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

	void DrawItemActivityOutline(OutlineFlags flags, uint32_t colourHighlight, float rounding)
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

	// from https://github.com/ocornut/imgui/issues/1901 @zfedoran
	bool BufferingBar(const std::string& name, float value, const glm::vec2& sizeArg, const glm::vec4& bgCol, const glm::vec4& fgCol)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(name.c_str());

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size = ImVec2(sizeArg.x, sizeArg.y);
		size.x -= style.FramePadding.x * 2;

		const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
		ImGui::ItemSize(bb, style.FramePadding.y);
		if (!ImGui::ItemAdd(bb, id))
			return false;

		ImU32 bg = ImGui::GetColorU32(ImVec4(bgCol.r, bgCol.g, bgCol.b, bgCol.a));
		ImU32 fg = ImGui::GetColorU32(ImVec4(fgCol.r, fgCol.g, fgCol.b, fgCol.a));

		const float circleStart = size.x * 0.7f;
		const float circleEnd = size.x;
		const float circleWidth = circleEnd - circleStart;

		window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart, bb.Max.y), bg);
		window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart * value, bb.Max.y), fg);

		const float t = (float)g.Time;
		const float r = size.y / 2;
		const float speed = 1.5f;

		const float a = speed * 0;
		const float b = speed * 0.333f;
		const float c = speed * 0.666f;

		const float o1 = (circleWidth + r) * (t + a - speed * (int)((t + a) / speed)) / speed;
		const float o2 = (circleWidth + r) * (t + b - speed * (int)((t + b) / speed)) / speed;
		const float o3 = (circleWidth + r) * (t + c - speed * (int)((t + c) / speed)) / speed;

		window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o1, bb.Min.y + r), r, bg);
		window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o2, bb.Min.y + r), r, bg);
		window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o3, bb.Min.y + r), r, bg);

		return true;
	}

	// from https://github.com/ocornut/imgui/issues/1901 @zfedoran
	bool Spinner(const std::string& name, float radius, uint32_t thickness, const glm::vec4& colour)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(name.c_str());

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size((radius) * 2, (radius + style.FramePadding.y) * 2);

		const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
		ImGui::ItemSize(bb, style.FramePadding.y);
		if (!ImGui::ItemAdd(bb, id))
			return false;

		// Render
		window->DrawList->PathClear();

		int num_segments = 30;
		int start = (int)abs(ImSin((float)g.Time * 1.8f) * (num_segments - 5));

		const float a_min = IM_PI * 2.0f * ((float)start) / (float)num_segments;
		const float a_max = IM_PI * 2.0f * ((float)num_segments - 3) / (float)num_segments;

		const ImVec2 centre = ImVec2(pos.x + radius, pos.y + radius + style.FramePadding.y);

		for (int i = 0; i < num_segments; i++)
		{
			const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
			window->DrawList->PathLineTo(ImVec2(centre.x + ImCos(a + (float)g.Time * 8) * radius, centre.y + ImSin(a + (float)g.Time * 8) * radius));
		}

		window->DrawList->PathStroke(ImGui::GetColorU32(ImVec4(colour.r, colour.g, colour.b, colour.a)), false, (float)thickness);
		return true;
	}

}