#pragma once

#include <string>
#include <utility>
#include <functional>

#include "Lavender/Utils/Utils.hpp"
#include "Lavender/Utils/UUID.hpp"

#include <glm/glm.hpp>

#include <imgui.h>
#include <imgui_internal.h>

#include <spdlog/formatter.h>

#include "Lavender/UI/Colours.hpp"

#include "Lavender/Renderer/Image.hpp"

namespace Lavender::UI
{

	namespace Draw
	{
		void Underline(bool fullWidth = false, float offsetX = 0.0f, float offsetY = -1.0f);
	}

	void PushID(uint32_t id = LV_MAX_UINT32);
	void PopID();
	void ResetIDs();

	void ShiftCursor(float x, float y);
    void ShiftCursorX(float distance);
    void ShiftCursorY(float distance);

	void HelpMarker(const std::string& desc);

	bool Checkbox(const std::string& label, bool& value);

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

    bool TreeNode(const std::string& name, TreeNodeFlags flags = TreeNodeFlags::None);
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

	bool DragFloat(const std::string& name, float& value, float speed = 1.0f, float minValue = 0.0f, float maxValue = 0.0f, const std::string& format = "%.2f", SliderFlags flags = SliderFlags::None);
	bool DragFloat2(const std::string& name, glm::vec2& value, float speed = 1.0f, float minValue = 0.0f, float maxValue = 0.0f, const std::string& format = "%.2f", SliderFlags flags = SliderFlags::None);
    bool DragFloat3(const std::string& name, glm::vec3& value, float speed = 1.0f, float minValue = 0.0f, float maxValue = 0.0f, const std::string& format = "%.2f", SliderFlags flags = SliderFlags::None);
	bool DragFloat4(const std::string& name, glm::vec4& value, float speed = 1.0f, float minValue = 0.0f, float maxValue = 0.0f, const std::string& format = "%.2f", SliderFlags flags = SliderFlags::None);

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
    
	enum class ComboFlags : uint8_t
	{
		None = 0,
		PopupAlignLeft = BIT(0),
		HeightSmall = BIT(1),
		HeightRegular = BIT(2),
		HeightLarge = BIT(3),
		HeightLargest = BIT(4),
		NoArrowButton = BIT(5),
		NoPreview = BIT(6),

		HeightMask = ImGuiComboFlags_HeightSmall | ImGuiComboFlags_HeightRegular | ImGuiComboFlags_HeightLarge | ImGuiComboFlags_HeightLargest
	};
	DEFINE_BITWISE_OPS(ComboFlags)

	bool BeginCombo(const std::string& name, const std::string& previewValue, ComboFlags flags = ComboFlags::None);
	void EndCombo();

	struct Combo
	{
	public:
		typedef std::function<void()> SelectionFunc;
	public:
		std::string Preview = {};
		std::string Selected = {};
		std::vector<std::pair<std::string, SelectionFunc>> Items = { };

	public:
		void Render(const std::string& label, UI::ComboFlags flags = UI::ComboFlags::None);
	};

	enum class InputTextFlags : uint32_t
	{
		None = 0,
		CharsDecimal = BIT(0),
		CharsHexadecimal = BIT(1),
		CharsUppercase = BIT(2),
		CharsNoBlank = BIT(3),
		AutoSelectAll = BIT(4),
		EnterReturnsTrue = BIT(5),
		CallbackCompletion = BIT(6),
		CallbackHistory = BIT(7),
		CallbackAlways = BIT(8),
		CallbackCharFilter = BIT(9),
		AllowTabInput = BIT(10),
		CtrlEnterForNewLine = BIT(11),
		NoHorizontalScroll = BIT(12),
		AlwaysOverwrite = BIT(13),
		ReadOnly = BIT(14),
		Password = BIT(15),
		NoUndoRedo = BIT(16),
		CharsScientific = BIT(17),
		CallbackResize = BIT(18),
		CallbackEdit = BIT(19)
	};

	bool InputText(const std::string& label, char* buffer, size_t size, InputTextFlags flags = InputTextFlags::None);

	typedef std::function<void()> ClickableImageFn;
	struct ClickAbleImage
	{
	public:
		Ref<Image2D> Image = nullptr;
		glm::vec2 Size = { 0.0f, 0.0f };
		ClickableImageFn Action = nullptr;

	public:
		void Render();
	};

    void BeginPropertyGrid(uint32_t columns = 2);
    void EndPropertyGrid();

	void BeginCustomProperty(const std::string& label, const std::string& helpText = "");
	void EndCustomProperty();

	template<typename ... Args>
	bool Property(const std::string& label, const std::string& helpText = "", const std::string& fmt = "", Args&& ...args);

	bool Property(const std::string& label, float& value, float delta = 0.1f, float min = 0.0f, float max = 0.0f, const std::string& format = "%.2f", const std::string& helpText = "");
	bool Property(const std::string& label, glm::vec2& value, float delta = 0.1f, float min = 0.0f, float max = 0.0f, const std::string& format = "%.2f", const std::string& helpText = "");
	bool Property(const std::string& label, glm::vec3& value, float delta = 0.1f, float min = 0.0f, float max = 0.0f, const std::string& format = "%.2f", const std::string& helpText = "");
	bool Property(const std::string& label, glm::vec4& value, float delta = 0.1f, float min = 0.0f, float max = 0.0f, const std::string& format = "%.2f", const std::string& helpText = "");

	bool Property(const std::string& label, Combo& value, const std::string& helpText = "");
	bool Property(const std::string& label, ClickAbleImage& value, const std::string& helpText = "");

	// Custom property for entity size
	bool UniformProperty(const std::string& label, glm::vec3& value, bool& uniform, float delta = 0.1f, float min = 0.0f, float max = 0.0f, const std::string& format = "%.2f", const std::string& hoverText = "");

    bool BeginMainMenuBar();
    void EndMainMenuBar();

    bool BeginMenu(const std::string& name);
    bool MenuItem(const std::string& name = "Empty Item", const std::string& shortcut = "");
    void EndMenu();

    void SameLine();
    void Dummy(const glm::vec2& size = { 10.0f, 10.0f });

	void FullScreenOverlay(const glm::vec4& colour = { 0.0f, 0.0f, 0.0f, 0.2f });

	// Templated functions
	template<typename ... Args>
	bool Property(const std::string& label, const std::string& helpText, const std::string& fmt, Args&& ...args)
	{
		UI::ShiftCursor(10.0f, 9.0f);
		UI::Text(label);

		if (std::strlen(helpText.c_str()) != 0)
		{
			UI::SameLine();
			UI::HelpMarker(helpText);
		}

		ImGui::NextColumn();
		UI::ShiftCursor(4.0f, 8.0f);
		ImGui::PushItemWidth(-1);

		UI::Text<Args...>(fmt, args...);

		ImGui::PopItemWidth();
		ImGui::NextColumn();
		UI::Draw::Underline();

		return false;
	}

}