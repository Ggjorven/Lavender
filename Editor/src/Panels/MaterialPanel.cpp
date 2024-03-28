#include "MaterialPanel.hpp"

#include <Lavender/Core/Logging.hpp>

#include <Lavender/Workspace/Assets/Asset.hpp>

#include <Lavender/UI/UI.hpp>
#include <Lavender/UI/Style.hpp>
#include <Lavender/UI/Colours.hpp>

#include <imgui.h>

namespace Lavender
{

	MaterialPanel::MaterialPanel(Ref<Project> project)
		: m_Project(project)
	{
	}

	MaterialPanel::~MaterialPanel()
	{
	}

	void MaterialPanel::RenderUI()
	{
		// To remove the tab bar.
		ImGuiWindowClass window = {};
		window.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
		ImGui::SetNextWindowClass(&window);

		UI::ScopedStyleList styles = { {
			{ UI::StyleType::FrameRounding, 1.0f },
			{ UI::StyleType::FrameBorderSize, 1.0f }
		} };
		//UI::Style windowPadding = { UI::StyleType::FramePadding, { 10.0f, 4.0f } };
		//UI::Style treePadding = { UI::StyleType::WindowPadding, { 0.0f, 0.0f } };

		UI::ScopedStyleList colours = { {
			{ UI::StyleColourType::Header, UI::Colours::BackgroundPopup },
			{ UI::StyleColourType::Border, UI::Colours::BackgroundDark },
		} };

		UI::BeginWindow("Material"/*, UI::WindowFlags::NoCollapse | UI::WindowFlags::NoDecoration | UI::WindowFlags::NoTitleBar | UI::WindowFlags::NoMove*/);



		UI::EndWindow();
	}

}