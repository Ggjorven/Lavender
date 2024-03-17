#pragma once

#include <string>

#include "Lavender/Utils/Utils.hpp"

#include <glm/glm.hpp>

#include <imgui.h>
#include <imgui_internal.h>

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
		ImGui::Text(fmt.c_str(), std::forward<Args>(args)...);
	}

}