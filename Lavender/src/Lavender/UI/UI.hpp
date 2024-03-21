#pragma once

#include <string>

#include "Lavender/Utils/Utils.hpp"

#include <glm/glm.hpp>

#include <imgui.h>
#include <imgui_internal.h>

#include <spdlog/formatter.h>

#include <Lavender/UI/Colours.hpp>

namespace Lavender::UI
{

	void ShiftCursor(float x, float y);
    void ShiftCursorX(float distance);
    void ShiftCursorY(float distance);

	void HelpMarker(const std::string& desc);

	inline bool IsItemDisabled() { return ImGui::GetItemFlags() & ImGuiItemFlags_Disabled; }
	inline ImRect GetItemRect() { return ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()); }

	glm::vec2 GetContentRegionAvail();

	inline ImRect RectExpanded(const ImRect& rect, float x, float y)
	{
		ImRect result = rect;
		result.Min.x -= x;
		result.Min.y -= y;
		result.Max.x += x;
		result.Max.y += y;
		return result;
	}

    enum class WindowFlags : uint32_t
    {
        None = 0,
        NoTitleBar = BIT(0),
        NoResize = BIT(1),
        NoMove = BIT(2),
        NoScrollbar = BIT(3),
        NoScrollWithMouse = BIT(4),
        NoCollapse = BIT(5),
        AlwaysAutoResize = BIT(6),
        NoBackground = BIT(7),
        NoSavedSettings = BIT(8),
        NoMouseInputs = BIT(9),
        MenuBar = BIT(10),
        HorizontalScrollbar = BIT(11),
        NoFocusOnAppearing = BIT(12),
        NoBringToFrontOnFocus = BIT(13),
        AlwaysVerticalScrollbar = BIT(14),
        AlwaysHorizontalScrollbar = BIT(15),
        AlwaysUseWindowPadding = BIT(16),

        NoNavInputs = BIT(18),
        NoNavFocus = BIT(19),
        UnsavedDocument = BIT(20),
        NoDocking = BIT(21),

        NoNav = NoNavInputs | NoNavFocus,
        NoDecoration = NoTitleBar | NoResize | NoScrollbar | NoCollapse,
        NoInputs = NoMouseInputs | NoNavInputs | NoNavFocus
    };
    DEFINE_BITWISE_OPS(WindowFlags)

    void BeginWindow(const std::string& name = "Unnamed Window", WindowFlags flags = WindowFlags::None);
    void EndWindow();

	void BeginChild(const std::string& name, const glm::vec2& size = { 0.0f, 0.0f }, bool border = false, WindowFlags flags = WindowFlags::None);
	void EndChild();

    template<typename ... Args>
    static void Text(const std::string& fmt, Args&& ...args)
    {
        auto fmtStr = fmt::format(fmt.c_str(), std::forward<Args>(args)...);
        ImGui::Text(fmtStr.c_str());
    }

    bool Button(const std::string& name, const glm::vec2& size = { 0.0f, 0.0f });

    enum class TreeNodeFlags : uint16_t
    {
        None = 0,
        Selected = BIT(0),
        Framed = BIT(1),
        AllowItemOverlap = BIT(2),
        NoTreePushOnOpen = BIT(3),
        NoAutoOpenOnLog = BIT(4),
        DefaultOpen = BIT(5),
        OpenOnDoubleClick = BIT(6),
        OpenOnArrow = BIT(7),
        Leaf = BIT(8),
        Bullet = BIT(9),
        FramePadding = BIT(10),
        SpanAvailWidth = BIT(11),
        SpanFullWidth = BIT(12),
        NavLeftJumpsBackHere = BIT(13),

        CollapsingHeader = Framed | NoTreePushOnOpen | NoAutoOpenOnLog
    };
    DEFINE_BITWISE_OPS(TreeNodeFlags)

    bool TreeNode(const std::string& name, TreeNodeFlags flags);
    void TreeNodePop();

    enum class SelectableFlags : uint8_t
    {
        None = 0,
        DontClosePopups = BIT(0),
        SpanAllColumns = BIT(1),
        AllowDoubleClick = BIT(2),
        Disabled = BIT(3),
        AllowItemOverlap = BIT(4)
    };
    DEFINE_BITWISE_OPS(SelectableFlags)

    // Returns true is was pressed (can be used to select and unselect), 'state' is used to highlight or not highlight the selectable and also sets it to true if the selectable has been pressed never back to false
    bool Selectable(const std::string& name, bool* state, const glm::vec2& size = { 0.0f, 0.0f }, SelectableFlags flags = SelectableFlags::None);

    enum class SliderFlags : uint8_t
    {
        None = 0,
        AlwaysClamp = BIT(4),
        Logarithmic = BIT(5),
        NoRoundToFormat = BIT(6),
        NoInput = BIT(7),
    };
	DEFINE_BITWISE_OPS(SliderFlags)

	bool DragFloat2(const std::string& name, glm::vec2& value, float speed = 1.0f, float minValue = 0.0f, float maxValue = 0.0f, const std::string& format = "%.3f", SliderFlags flags = SliderFlags::None);
    bool DragFloat3(const std::string& name, glm::vec3& value, float speed = 1.0f, float minValue = 0.0f, float maxValue = 0.0f, const std::string& format = "%.3f", SliderFlags flags = SliderFlags::None);
	bool DragFloat4(const std::string& name, glm::vec4& value, float speed = 1.0f, float minValue = 0.0f, float maxValue = 0.0f, const std::string& format = "%.3f", SliderFlags flags = SliderFlags::None);

	enum class TableFlags : uint32_t
	{
		None = 0,
		Resizable = BIT(0),
		Reorderable = BIT(1),
		Hideable = BIT(2),
		Sortable = BIT(3),
		NoSavedSettings = BIT(4),
		ContextMenuInBody = BIT(5),
		RowBg = BIT(6),
		BordersInnerH = BIT(7),
		BordersOuterH = BIT(8),
		BordersInnerV = BIT(9),
		BordersOuterV = BIT(10),
		BordersH = BordersInnerH | BordersOuterH,
		BordersV = BordersInnerV | BordersOuterV,
		BordersInner = BordersInnerV | BordersInnerH,
		BordersOuter = BordersOuterV | BordersOuterH,
		Borders = BordersInner | BordersOuter,
		NoBordersInBody = BIT(11),
		NoBordersInBodyUntilResize = BIT(12),

		SizingFixedFit = BIT(13),
		SizingFixedSame = BIT_X(2, 13),
		SizingStretchProp = BIT_X(3, 13),
		SizingStretchSame = BIT_X(4, 13),
		NoHostExtendY = BIT(17),
		NoKeepColumnsVisible = BIT(18),
		PreciseWidths = BIT(19),
		NoClip = BIT(20),
		PadOuterX = BIT(21),
		NoPadOuterX = BIT(22),
		NoPadInnerX = BIT(23),

		ScrollX = BIT(24),
		ScrollY = BIT(25),

		SortMulti = BIT(26),
		SortTristate = BIT(27),
	};
	DEFINE_BITWISE_OPS(TableFlags)

	bool BeginTable(const std::string& name, uint32_t columns = 1, UI::TableFlags flags = UI::TableFlags::None);
	void EndTable();

	void TableSetColumnIndex(uint32_t index = 0);

	enum class TableRowFlags : uint8_t
	{
		None = 0,
		Headers = BIT(0)
	};
	DEFINE_BITWISE_OPS(TableRowFlags)

	void TableNextRow(TableRowFlags flags = TableRowFlags::None, float minHeight = 0.0f);

	enum class TableColumnFlags
	{
		None = 0,
		Disabled = BIT(0),
		DefaultHide = BIT(1),
		DefaultSort = BIT(2),
		WidthStretch = BIT(3),
		WidthFixed = BIT(4),
		NoResize = BIT(5),
		NoReorder = BIT(6),
		NoHide = BIT(7),
		NoClip = BIT(8),
		NoSort = BIT(9),
		NoSortAscending = BIT(10),
		NoSortDescending = BIT(11),
		NoHeaderLabel = BIT(12),
		NoHeaderWidth = BIT(13),
		PreferSortAscending = BIT(14),
		PreferSortDescending = BIT(15),
		IndentEnable = BIT(16),
		IndentDisable = BIT(17),

		IsEnabled = BIT(24),
		IsVisible = BIT(25),
		IsSorted = BIT(26),
		IsHovered = BIT(27)
	};
	DEFINE_BITWISE_OPS(TableColumnFlags)

	void TableSetupColumn(const std::string& name, TableColumnFlags flags = TableColumnFlags::None, float widthOrWeight = 0.0f);
    
	void FullScreenOverlay(const glm::vec4& colour = { 0.0f, 0.0f, 0.0f, 0.2f });

    void BeginPropertyGrid(uint32_t columns);
    void EndPropertyGrid();

	void BeginCustomProperty(const std::string& label, const std::string& helpText = "");
	void EndCustomProperty();

	bool Property(const std::string& label, glm::vec2& value, float delta = 0.1f, float min = 0.0f, float max = 0.0f, const std::string& helpText = "");
	bool Property(const std::string& label, glm::vec3& value, float delta = 0.1f, float min = 0.0f, float max = 0.0f, const std::string& helpText = "");
	bool Property(const std::string& label, glm::vec4& value, float delta = 0.1f, float min = 0.0f, float max = 0.0f, const std::string& helpText = "");

    bool BeginMainMenuBar();
    void EndMainMenuBar();

    bool BeginMenu(const std::string& name);
    bool MenuItem(const std::string& name = "Empty Item", const std::string& shortcut = "");
    void EndMenu();

    void SameLine();
    void Dummy(const glm::vec2& size = { 10.0f, 10.0f });

}

/*
	static bool Property(const std::string& label, std::string& value, const char* helpText = "");
	static bool PropertyMultiline(const std::string& label, std::string& value, const char* helpText = "");
	static void Property(const std::string& label, const std::string& value, const char* helpText = "");
	static void Property(const std::string& label, const char* value, const char* helpText = "");
	static bool Property(const std::string& label, char* value, size_t length, const char* helpText = "");
	static bool Property(const std::string& label, bool& value, const char* helpText = "");
	static bool Property(const std::string& label, int8_t& value, int8_t min = 0, int8_t max = 0, const char* helpText = "");
	static bool Property(const std::string& label, int16_t& value, int16_t min = 0, int16_t max = 0, const char* helpText = "");
	static bool Property(const std::string& label, int32_t& value, int32_t min = 0, int32_t max = 0, const char* helpText = "");
	static bool Property(const std::string& label, int64_t& value, int64_t min = 0, int64_t max = 0, const char* helpText = "");
	static bool Property(const std::string& label, uint8_t& value, uint8_t minValue = 0, uint8_t maxValue = 0, const char* helpText = "");
	static bool Property(const std::string& label, uint16_t& value, uint16_t minValue = 0, uint16_t maxValue = 0, const char* helpText = "");
	static bool Property(const std::string& label, uint32_t& value, uint32_t minValue = 0, uint32_t maxValue = 0, const char* helpText = "");
	static bool Property(const std::string& label, uint64_t& value, uint64_t minValue = 0, uint64_t maxValue = 0, const char* helpText = "");

static bool Property(const std::string& label, std::string& value, const char* helpText = "")
{
	bool modified = false;

	ShiftCursor(10.0f, 9.0f);
	ImGui::Text(label);

	if (std::strlen(helpText) != 0)
	{
		ImGui::SameLine();
		HelpMarker(helpText);
	}

	ImGui::NextColumn();
	ShiftCursorY(4.0f);
	ImGui::PushItemWidth(-1);

	modified = UI::InputText(fmt::format("##{0}", label).c_str(), &value);

	ImGui::PopItemWidth();
	ImGui::NextColumn();
	Draw::Underline();

	return modified;
}

static bool PropertyMultiline(const std::string& label, std::string& value, const char* helpText = "")
{
	bool modified = false;

	ImGui::Text(label);

	if (std::strlen(helpText) != 0)
	{
		ImGui::SameLine();
		HelpMarker(helpText);
	}

	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);

	modified = UI::InputTextMultiline(fmt::format("##{0}", label).c_str(), &value);

	ImGui::PopItemWidth();
	ImGui::NextColumn();

	return modified;
}

static void Property(const std::string& label, const std::string& value, const char* helpText = "")
{
	ShiftCursor(10.0f, 9.0f);
	ImGui::Text(label);

	if (std::strlen(helpText) != 0)
	{
		ImGui::SameLine();
		HelpMarker(helpText);
	}

	ImGui::NextColumn();
	ShiftCursorY(4.0f);
	ImGui::PushItemWidth(-1);
	BeginDisabled();
	UI::InputText(fmt::format("##{0}", label).c_str(), (char*)value.c_str(), value.size(), ImGuiInputTextFlags_ReadOnly);
	EndDisabled();

	ImGui::PopItemWidth();
	ImGui::NextColumn();
	Draw::Underline();
}

static void Property(const std::string& label, const char* value, const char* helpText = "")
{
	ShiftCursor(10.0f, 9.0f);
	ImGui::Text(label);

	if (std::strlen(helpText) != 0)
	{
		ImGui::SameLine();
		HelpMarker(helpText);
	}

	ImGui::NextColumn();
	ShiftCursorY(4.0f);
	ImGui::PushItemWidth(-1);
	BeginDisabled();
	UI::InputText(fmt::format("##{0}", label).c_str(), (char*)value, 256, ImGuiInputTextFlags_ReadOnly);
	EndDisabled();

	ImGui::PopItemWidth();
	ImGui::NextColumn();
	Draw::Underline();
}

static bool Property(const std::string& label, char* value, size_t length, const char* helpText = "")
{
	ShiftCursor(10.0f, 9.0f);
	ImGui::Text(label);

	if (std::strlen(helpText) != 0)
	{
		ImGui::SameLine();
		HelpMarker(helpText);
	}

	ImGui::NextColumn();
	ShiftCursorY(4.0f);
	ImGui::PushItemWidth(-1);

	bool modified = UI::InputText(fmt::format("##{0}", label).c_str(), value, length);

	ImGui::PopItemWidth();
	ImGui::NextColumn();
	Draw::Underline();

	return modified;
}

static bool Property(const std::string& label, bool& value, const char* helpText = "")
{
	bool modified = false;

	ShiftCursor(10.0f, 9.0f);
	ImGui::Text(label);

	if (std::strlen(helpText) != 0)
	{
		ImGui::SameLine();
		HelpMarker(helpText);
	}

	ImGui::NextColumn();
	ShiftCursorY(4.0f);
	ImGui::PushItemWidth(-1);

	modified = UI::Checkbox(fmt::format("##{0}", label).c_str(), &value);

	ImGui::PopItemWidth();
	ImGui::NextColumn();
	Draw::Underline();

	return modified;
}

static bool Property(const std::string& label, int8_t& value, int8_t min = 0, int8_t max = 0, const char* helpText = "")
{
	ShiftCursor(10.0f, 9.0f);
	ImGui::Text(label);

	if (std::strlen(helpText) != 0)
	{
		ImGui::SameLine();
		HelpMarker(helpText);
	}

	ImGui::NextColumn();
	ShiftCursorY(4.0f);
	ImGui::PushItemWidth(-1);

	bool modified = UI::DragInt8(fmt::format("##{0}", label).c_str(), &value, 1.0f, min, max);

	ImGui::PopItemWidth();
	ImGui::NextColumn();
	Draw::Underline();

	return modified;
}

static bool Property(const std::string& label, int16_t& value, int16_t min = 0, int16_t max = 0, const char* helpText = "")
{
	ShiftCursor(10.0f, 9.0f);
	ImGui::Text(label);

	if (std::strlen(helpText) != 0)
	{
		ImGui::SameLine();
		HelpMarker(helpText);
	}

	ImGui::NextColumn();
	ShiftCursorY(4.0f);
	ImGui::PushItemWidth(-1);

	bool modified = UI::DragInt16(fmt::format("##{0}", label).c_str(), &value, 1.0f, min, max);

	ImGui::PopItemWidth();
	ImGui::NextColumn();
	Draw::Underline();

	return modified;
}

static bool Property(const std::string& label, int32_t& value, int32_t min = 0, int32_t max = 0, const char* helpText = "")
{
	ShiftCursor(10.0f, 9.0f);
	ImGui::Text(label);

	if (std::strlen(helpText) != 0)
	{
		ImGui::SameLine();
		HelpMarker(helpText);
	}

	ImGui::NextColumn();
	ShiftCursorY(4.0f);
	ImGui::PushItemWidth(-1);

	bool modified = UI::DragInt32(fmt::format("##{0}", label).c_str(), &value, 1.0f, min, max);

	ImGui::PopItemWidth();
	ImGui::NextColumn();
	Draw::Underline();

	return modified;
}

static bool Property(const std::string& label, int64_t& value, int64_t min = 0, int64_t max = 0, const char* helpText = "")
{
	ShiftCursor(10.0f, 9.0f);
	ImGui::Text(label);

	if (std::strlen(helpText) != 0)
	{
		ImGui::SameLine();
		HelpMarker(helpText);
	}

	ImGui::NextColumn();
	ShiftCursorY(4.0f);
	ImGui::PushItemWidth(-1);

	bool modified = UI::DragInt64(fmt::format("##{0}", label).c_str(), &value, 1.0f, min, max);

	ImGui::PopItemWidth();
	ImGui::NextColumn();
	Draw::Underline();

	return modified;
}

static bool Property(const std::string& label, uint8_t& value, uint8_t minValue = 0, uint8_t maxValue = 0, const char* helpText = "")
{
	ShiftCursor(10.0f, 9.0f);
	ImGui::Text(label);

	if (std::strlen(helpText) != 0)
	{
		ImGui::SameLine();
		HelpMarker(helpText);
	}

	ImGui::NextColumn();
	ShiftCursorY(4.0f);
	ImGui::PushItemWidth(-1);

	bool modified = UI::DragUInt8(fmt::format("##{0}", label).c_str(), &value, 1.0f, minValue, maxValue);

	ImGui::PopItemWidth();
	ImGui::NextColumn();
	Draw::Underline();

	return modified;
}

static bool Property(const std::string& label, uint16_t& value, uint16_t minValue = 0, uint16_t maxValue = 0, const char* helpText = "")
{
	ShiftCursor(10.0f, 9.0f);
	ImGui::Text(label);

	if (std::strlen(helpText) != 0)
	{
		ImGui::SameLine();
		HelpMarker(helpText);
	}

	ImGui::NextColumn();
	ShiftCursorY(4.0f);
	ImGui::PushItemWidth(-1);

	bool modified = UI::DragUInt16(fmt::format("##{0}", label).c_str(), &value, 1.0f, minValue, maxValue);

	ImGui::PopItemWidth();
	ImGui::NextColumn();
	Draw::Underline();

	return modified;
}

static bool Property(const std::string& label, uint32_t& value, uint32_t minValue = 0, uint32_t maxValue = 0, const char* helpText = "")
{
	ShiftCursor(10.0f, 9.0f);
	ImGui::Text(label);

	if (std::strlen(helpText) != 0)
	{
		ImGui::SameLine();
		HelpMarker(helpText);
	}

	ImGui::NextColumn();
	ShiftCursorY(4.0f);
	ImGui::PushItemWidth(-1);

	bool modified = UI::DragUInt32(fmt::format("##{0}", label).c_str(), &value, 1.0f, minValue, maxValue);

	ImGui::PopItemWidth();
	ImGui::NextColumn();
	Draw::Underline();

	return modified;
}

static bool Property(const std::string& label, uint64_t& value, uint64_t minValue = 0, uint64_t maxValue = 0, const char* helpText = "")
{
	ShiftCursor(10.0f, 9.0f);
	ImGui::Text(label);

	if (std::strlen(helpText) != 0)
	{
		ImGui::SameLine();
		HelpMarker(helpText);
	}

	ImGui::NextColumn();
	ShiftCursorY(4.0f);
	ImGui::PushItemWidth(-1);

	bool modified = UI::DragUInt64(fmt::format("##{0}", label).c_str(), &value, 1.0f, minValue, maxValue);

	ImGui::PopItemWidth();
	ImGui::NextColumn();
	Draw::Underline();

	return modified;
}

static bool Property(const char* label, glm::vec2& value, float delta = 0.1f, float min = 0.0f, float max = 0.0f, const char* helpText = "")
{
	ShiftCursor(10.0f, 9.0f);
	ImGui::Text(label);

	if (std::strlen(helpText) != 0)
	{
		ImGui::SameLine();
		HelpMarker(helpText);
	}

	ImGui::NextColumn();
	ShiftCursorY(4.0f);
	ImGui::PushItemWidth(-1);

	bool modified = UI::DragFloat2(fmt::format("##{0}", label).c_str(), glm::value_ptr(value), delta, min, max);

	ImGui::PopItemWidth();
	ImGui::NextColumn();
	Draw::Underline();

	return modified;
}

static bool Property(const char* label, glm::vec3& value, float delta = 0.1f, float min = 0.0f, float max = 0.0f, const char* helpText = "")
{
	ShiftCursor(10.0f, 9.0f);
	ImGui::Text(label);

	if (std::strlen(helpText) != 0)
	{
		ImGui::SameLine();
		HelpMarker(helpText);
	}

	ImGui::NextColumn();
	ShiftCursorY(4.0f);
	ImGui::PushItemWidth(-1);

	bool modified = UI::DragFloat3(fmt::format("##{0}", label).c_str(), glm::value_ptr(value), delta, min, max);

	ImGui::PopItemWidth();
	ImGui::NextColumn();
	Draw::Underline();

	return modified;
}

static bool Property(const char* label, glm::vec4& value, float delta = 0.1f, float min = 0.0f, float max = 0.0f, const char* helpText = "")
{
	ShiftCursor(10.0f, 9.0f);
	ImGui::Text(label);

	if (std::strlen(helpText) != 0)
	{
		ImGui::SameLine();
		HelpMarker(helpText);
	}

	ImGui::NextColumn();
	ShiftCursorY(4.0f);
	ImGui::PushItemWidth(-1);

	bool modified = UI::DragFloat4(fmt::format("##{0}", label).c_str(), glm::value_ptr(value), delta, min, max);

	ImGui::PopItemWidth();
	ImGui::NextColumn();
	Draw::Underline();

	return modified;
}
*/