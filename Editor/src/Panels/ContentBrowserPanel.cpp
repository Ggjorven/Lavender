#include "ContentBrowserPanel.hpp"

#include <Lavender/Core/Logging.hpp>

#include <Lavender/Workspace/Assets/Asset.hpp>

#include <Lavender/UI/UI.hpp>
#include <Lavender/UI/Style.hpp>
#include <Lavender/UI/Colours.hpp>

#include <imgui.h>

namespace Lavender
{

	ContentBrowserPanel::ContentBrowserPanel(Ref<Project> project)
		: m_Project(project)
	{
	}

	ContentBrowserPanel::~ContentBrowserPanel()
	{
	}

	void ContentBrowserPanel::OnUpdate(float deltaTime)
	{
	}

	void ContentBrowserPanel::RenderUI()
	{
		// To remove the tab bar.
		ImGuiWindowClass window = {};
		window.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
		ImGui::SetNextWindowClass(&window);

		UI::ScopedStyleList styles = { {
			{ UI::StyleType::FrameRounding, 1.0f },
			{ UI::StyleType::FramePadding, { 10.0f, 4.0f } },
			{ UI::StyleType::FrameBorderSize, 1.0f },
			{ UI::StyleType::WindowPadding, { 0.0f, 0.0f } },
		} };

		UI::ScopedStyleList colours = { {
			{ UI::StyleColourType::Header, UI::Colours::BackgroundPopup },
			{ UI::StyleColourType::Border, UI::Colours::BackgroundDark },
			{ UI::StyleColourType::WindowBg, UI::Colours::BackgroundDark },
			{ UI::StyleColourType::ChildBg, UI::Colours::Background }
		} };

		UI::BeginWindow("ContentBrowser", UI::WindowFlags::NoCollapse | UI::WindowFlags::NoDecoration | UI::WindowFlags::NoTitleBar | UI::WindowFlags::NoMove);
		auto size = ImGui::GetCurrentWindow()->Size;
		m_Width = (uint32_t)size.x;
		m_Height = (uint32_t)size.y;

		UI::EndWindow();
	}

	Ref<ContentBrowserPanel> ContentBrowserPanel::Create(Ref<Project> project)
	{
		return RefHelper::Create<ContentBrowserPanel>(project);
	}

}