#include "Debug.hpp"

#include <Lavender/Core/Application.hpp>

#include <Lavender/Renderer/RendererConfig.hpp>

#include <Lavender/UI/UI.hpp>

#include <Lavender/WorkSpace/Project.hpp>
#include <Lavender/WorkSpace/EngineTracker.hpp>

namespace Lavender::UI
{

	Debug::Debug()
	{
		InitStyles();
	}

	Debug::~Debug()
	{
	}

	void Debug::RenderUI()
	{
		m_Styles.Push();
		m_Colours.Push();

		UI::BeginWindow("Debug", UI::WindowFlags::NoCollapse | UI::WindowFlags::NoDecoration | UI::WindowFlags::NoTitleBar | UI::WindowFlags::NoMove | UI::WindowFlags::NoTabBar);
		
		// Update Window Data
		{
			auto imWindow = ImGui::GetCurrentWindow();
			auto& position = imWindow->Pos;
			auto& size = imWindow->Size;

			m_Width = (uint32_t)size.x;
			m_Height = (uint32_t)size.y;

			auto& mainWindow = Application::Get().GetWindow();
			m_Position.x = (uint32_t)position.x - mainWindow.GetPositionX();
			m_Position.y = (uint32_t)position.y - mainWindow.GetPositionY();
		}

		if (UI::Tree("Debug"))
		{
			UI::BeginPropertyGrid("##DebugGrid", 2);
			
			UI::Property("FPS", "", "{0}", Track::Frame::FPS);
			UI::Property("Frametime", "", "{0:.3f} ms", Track::Frame::FrameTime);
			UI::Property("Memory", "Not comparable with Task Manager", "{0:.1f} MB", (float)((double)Utils::ToolKit::GetMemoryUsage() / (1024.0 * 1024.0)));
			UI::Property("Config", "Make sure you built your script in the same configuration.", "{0}", ConfigurationToString(Track::Lavender::Config));

			UI::EndPropertyGrid("##DebugGrid");
		}

		if (UI::Tree("Scripting"))
		{
			UI::BeginPropertyGrid("##ScriptGrid", 2);

			UI::Property("Loaded", "", "{0}", (Project::Get()->GetScript() ? "True" : "False"));
			UI::Property("Reload", "Reload", []() 
			{
				if (Project::Get()->GetScript())
					Project::Get()->GetScript()->Reload();
			});

			UI::EndPropertyGrid("##ScriptGrid");
		}

		if (UI::Tree("Rendering"))
		{
			UI::BeginPropertyGrid("##RenderGrid", 2);

			UI::Property("API", "", "{0}", RenderingAPIToString(RendererSpecification::API));
			if (RendererSpecification::API == RenderingAPI::Vulkan)
				UI::Property("Validation", "Vulkan validation layers state.", "{0}", ((Track::Lavender::Config == WorkSpace::Configuration::Dist) ? "Disabled" : "Enabled"));

			UI::Property("DrawCalls", "", "{0}", Renderer::GetRenderData().DrawCalls / 2u);

			UI::EndPropertyGrid("##RenderGrid");
		}

		if (UI::Tree("EditorCamera"))
		{
			auto camera = Scene::Get()->GetEditorCamera();

			UI::BeginPropertyGrid("##CameraGrid", 2);

			UI::Combo state = {};
			std::pair<std::string, UI::Combo::SelectionFunc> arcBall = std::make_pair("ArcBall", [this, camera]() { camera->SwitchState(); });
			std::pair<std::string, UI::Combo::SelectionFunc> flyCam = std::make_pair("FlyCam", [this, camera]() { camera->SwitchState(); });
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
				APP_LOG_ERROR("Invalid EditorCamera State selected");
				break;
			}

			UI::Property("FOV", camera->GetFOV(), 1.0f);

			UI::EndPropertyGrid("##CameraGrid");

			UI::ShiftCursorY(-4.0f);

			// Reset camera
			float spacing = 5.0f;
			auto avail = ImGui::GetContentRegionAvail();
			UI::ShiftCursorX(spacing);
			if (UI::Button("Reset Camera", { avail.x - (spacing * 2.0f), 0.0f}))
				camera->Reset();
		}
		
		UI::EndWindow();

		m_Styles.Pop();
		m_Colours.Pop();
	}

	Ref<Debug> Debug::Create()
	{
		return RefHelper::Create<Debug>();
	}

	void Debug::InitStyles()
	{
		m_Styles = UI::StyleList({
			{ UI::StyleType::FrameRounding, 4.0f },
			{ UI::StyleType::FramePadding, { 15.0f, 6.0f } }
		});

		m_Colours = UI::StyleList(/*{
			//{ UI::StyleColourType::WindowBg, UI::Colours::BackgroundBitDarker },
			//{ UI::StyleColourType::Header, UI::Colours::BackgroundPopup },
			//{ UI::StyleColourType::HeaderHovered, UI::Colours::LightTint },
			//{ UI::StyleColourType::HeaderActive, UI::Colours::LighterTint },
			//{ UI::StyleColourType::FrameBg, UI::Colours::AlphaTint },
			//{ UI::StyleColourType::FrameBgHovered, UI::Colours::LightTint },
			//{ UI::StyleColourType::FrameBgActive, UI::Colours::LighterTint },
			//{ UI::StyleColourType::Button, UI::Colours::DarkTint },
			//{ UI::StyleColourType::ButtonHovered, UI::Colours::DarkTint },
			//{ UI::StyleColourType::ButtonActive, UI::Colours::DarkTint },
		}*/);
	}

}