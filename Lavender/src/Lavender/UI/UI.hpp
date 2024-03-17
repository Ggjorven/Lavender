#pragma once

#include <string>

#include "Lavender/Utils/Utils.hpp"

#include <glm/glm.hpp>

#include <imgui.h>
#include <imgui_internal.h>

#include <spdlog/formatter.h>

namespace Lavender::UI
{

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

    enum class SelectableFlags
    {
        None = 0,
        DontClosePopups = BIT(0),
        SpanAllColumns = BIT(1),
        AllowDoubleClick = BIT(2),
        Disabled = BIT(3),
        AllowItemOverlap = BIT(4)
    };
    DEFINE_BITWISE_OPS(SelectableFlags)

    void Selectable(const std::string& name, bool* selected, const glm::vec2& size = { 0.0f, 0.0f }, SelectableFlags flags = SelectableFlags::None);

}