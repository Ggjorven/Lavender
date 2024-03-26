#include "DebugPanel.hpp"

#include <Lavender/Core/Logging.hpp>
#include <Lavender/Core/Application.hpp>

#include <Lavender/UI/UI.hpp>
#include <Lavender/UI/Colours.hpp>
#include <Lavender/UI/Style.hpp>

#include <Lavender/Renderer/Renderer.hpp>

#include <imgui.h>

namespace Lavender
{

	DebugPanel::DebugPanel(Ref<Project> project)
		: m_Project(project)
	{
	}

	DebugPanel::~DebugPanel()
	{
	}

	void DebugPanel::OnUpdate(float deltaTime)
	{
		static float timer = 0.0f;
		timer += deltaTime;
		float threshold = 0.05f;

		ImGuiIO& io = ImGui::GetIO();

		if (timer > threshold)
		{
			m_FPS = (uint32_t)io.Framerate;
			m_Frametime = 1.0f / io.Framerate * 1000.0f; // in ms

			timer = 0.0f;
		}
	}

	void DebugPanel::RenderUI()
	{
		// To remove the tab bar.
		ImGuiWindowClass window = {};
		window.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
		ImGui::SetNextWindowClass(&window);

		UI::ScopedStyleList styles = {{
			{ UI::StyleType::FrameRounding, 1.0f },
			{ UI::StyleType::FramePadding, { 10.0f, 4.0f } },
			{ UI::StyleType::FrameBorderSize, 1.0f }
		}};

		UI::ScopedStyleList colours = {{
			{ UI::StyleColourType::Header, UI::Colours::BackgroundPopup },
			{ UI::StyleColourType::Border, UI::Colours::BackgroundDark },
		}};

		UI::BeginWindow("Debug", UI::WindowFlags::NoCollapse | UI::WindowFlags::NoDecoration | UI::WindowFlags::NoTitleBar | UI::WindowFlags::NoMove);

		UI::TreeNodeFlags treenodeFlags = UI::TreeNodeFlags::Framed | UI::TreeNodeFlags::SpanAvailWidth | UI::TreeNodeFlags::FramePadding | UI::TreeNodeFlags::DefaultOpen;
			
		if (UI::TreeNode("Debug", treenodeFlags))
		{
			UI::Text("FPS: {0}", m_FPS);
			UI::Text("Frametime: {0:.2f} ms", m_Frametime);
			UI::TreeNodePop();
		}

		UI::Dummy({ 0.0f, 2.0f });

		if (UI::TreeNode("Rendering", treenodeFlags))
		{
			UI::Text("Draw calls: {0}", Renderer::GetRenderData().DrawCalls);

			auto camera = m_Project->GetSceneCollection().GetActive()->GetEditorCamera();
			UI::Text("Camera: ");
			UI::SameLine();
			if (UI::Button("Switch State"))
			{
				camera->SwitchState();
			}

			UI::TreeNodePop();
		}

		UI::EndWindow();
	}

	Ref<DebugPanel> DebugPanel::Create(Ref<Project> project)
	{
		return RefHelper::Create<DebugPanel>(project);
	}

}