#include "DebugPanel.hpp"

#include <Lavender/Core/Logging.hpp>
#include <Lavender/Core/Application.hpp>

#include <Lavender/Utils/Profiler.hpp>

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
			m_MemoryUsage = (float)((double)Utils::ToolKit::GetMemoryUsage() / (1024.0 * 1024.0));

			timer = 0.0f;
		}
	}

	void DebugPanel::RenderUI()
	{
		LV_PROFILE_SCOPE("DebugPanel::RenderUI");

		// To remove the tab bar.
		ImGuiWindowClass window = {};
		window.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
		ImGui::SetNextWindowClass(&window);

		UI::ScopedStyleList styles = {{
			{ UI::StyleType::FrameRounding, 4.0f },
			{ UI::StyleType::FramePadding, { 15.0f, 6.0f } }
		}};

		UI::ScopedStyleList colours = {{
			{ UI::StyleColourType::WindowBg, UI::Colours::BackgroundBitDarker },
			{ UI::StyleColourType::Header, UI::Colours::BackgroundPopup },
			{ UI::StyleColourType::HeaderHovered, UI::Colours::LightTint },
			{ UI::StyleColourType::HeaderActive, UI::Colours::LighterTint },
			{ UI::StyleColourType::FrameBg, UI::Colours::AlphaTint },
			{ UI::StyleColourType::FrameBgHovered, UI::Colours::LightTint },
			{ UI::StyleColourType::FrameBgActive, UI::Colours::LighterTint },
			{ UI::StyleColourType::Button, UI::Colours::DarkTint },
			{ UI::StyleColourType::ButtonHovered, UI::Colours::DarkTint },
			{ UI::StyleColourType::ButtonActive, UI::Colours::DarkTint },
		}};

		UI::BeginWindow("Debug", UI::WindowFlags::NoCollapse | UI::WindowFlags::NoDecoration | UI::WindowFlags::NoTitleBar | UI::WindowFlags::NoMove);

		if (CustomTree("Debug"))
		{
			UI::BeginPropertyGrid(2);

			UI::Property("FPS", "", "{0}", m_FPS);
			UI::Property("Frametime", "", "{0:.2f} ms", m_Frametime);
			UI::Property("Memory", "Not comparable with task manager", "{0:.1f} MB", m_MemoryUsage);

			UI::EndPropertyGrid();
		}

		if (CustomTree("Rendering"))
		{
			UI::BeginPropertyGrid(2);

			std::string api = RenderingAPIToString(Renderer::GetAPI());
			UI::Property("API", "", "{0}", api);
			uint32_t drawCalls = Renderer::GetRenderData().DrawCalls;
			UI::Property("DrawCalls", "", "{0}", drawCalls);

			UI::EndPropertyGrid();
		}

		if (CustomTree("EditorCamera"))
		{	
			auto camera = m_Project->GetSceneCollection().GetActive()->GetEditorCamera();
			
			UI::BeginPropertyGrid(2);

			UI::Combo state = {};
			std::pair<std::string, UI::Combo::SelectionFunc> arcBall = std::make_pair("ArcBall", [this, camera]() { camera->SwitchState(); });
			std::pair<std::string, UI::Combo::SelectionFunc> flyCam = std::make_pair("FlyCam", [this, camera](){ camera->SwitchState(); });
			state.Items = { arcBall, flyCam };
			state.Selected = EditorCamera::StateToString(camera->GetState());
			state.Preview = EditorCamera::StateToString(camera->GetState());

			UI::Property("State", state);

			switch (camera->GetState())
			{
			case EditorCamera::State::ArcBall:
				UI::Property("Speed", camera->GetArcBallSpeed(), 0.0004f, 0.0f, 0.0f, "%.3f", "Changes based on camera state.");
				break;

			case EditorCamera::State::FlyCam:
				UI::Property("Speed", camera->GetFlyCamSpeed(), 0.1f, 0.0f, 0.0f, "%.2f", "Changes based on camera state.");
				break;

			default:
				LV_LOG_ERROR("Invalid EditorCamera State selected");
				break;
			}

			UI::Property("FOV", camera->GetFOV(), 1.0f);

			UI::EndPropertyGrid();
		}

		UI::EndWindow();
	}

	Ref<DebugPanel> DebugPanel::Create(Ref<Project> project)
	{
		return RefHelper::Create<DebugPanel>(project);
	}

	bool DebugPanel::CustomTree(const std::string& label)
	{
		bool open = false;

		UI::ScopedStyleList styles = {{
			{ UI::StyleType::FrameBorderSize, 1.0f }
		}};

		UI::ScopedStyleList colours = {{
			{ UI::StyleColourType::Border, UI::Colours::BackgroundDark },
			{ UI::StyleColourType::Header, UI::Colours::BackgroundPopup },
			{ UI::StyleColourType::HeaderHovered, UI::Colours::LightTint },
			{ UI::StyleColourType::HeaderActive, UI::Colours::LighterTint },
		}};

		UI::TreeNodeFlags treenodeFlags = UI::TreeNodeFlags::Framed | UI::TreeNodeFlags::SpanAvailWidth | UI::TreeNodeFlags::FramePadding | UI::TreeNodeFlags::DefaultOpen;
		if (UI::TreeNode(label.c_str(), treenodeFlags))
		{
			open = true;
			UI::TreeNodePop();
		}
		UI::ShiftCursorY(-1.0f);

		return open;
	}

}