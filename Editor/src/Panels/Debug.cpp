#include "Debug.hpp"

#include <Lavender/UI/UI.hpp>

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
		
		if (UI::Tree("Debug"))
		{
			UI::BeginPropertyGrid(2);

			UI::Property("FPS", "", "{0}", Track::Frame::FPS);
			UI::Property("Frametime", "", "{0:.3f} ms", Track::Frame::FrameTime);
			UI::Property("Memory", "Can differ from taskmanager.", "{0:.1f} MB", (float)((double)Utils::ToolKit::GetMemoryUsage() / (1024.0 * 1024.0))/* - 67.0f*/);

			UI::EndPropertyGrid();
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