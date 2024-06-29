#include "Viewport.hpp"

#include "Lavender/Utils/Profiler.hpp"
#include "Lavender/Core/Application.hpp"

#include "Lavender/Renderer/Renderer.hpp"
#include "Lavender/Renderer/CommandBuffer.hpp"

#include "Lavender/WorkSpace/Scene.hpp"
#include "Lavender/WorkSpace/Project.hpp"
#include "Lavender/WorkSpace/EngineTracker.hpp"

#include "Lavender/UI/UI.hpp"
#include "Lavender/UI/Style.hpp"

#include <imgui.h>
#include <imgui_internal.h>

namespace Lavender::UI
{

	Viewport::Viewport()
	{
		InitStyles();
		InitResources();
	}

	Viewport::~Viewport()
	{
	}

	void Viewport::RenderUI()
	{
		APP_PROFILE_SCOPE("Viewport::UI");

		UI::ScopedStyleList styles = UI::ScopedStyleList({
			{ UI::StyleType::WindowPadding, { 0.0f, 0.0f } }
		});

		UI::BeginWindow("Viewport", UI::WindowFlags::NoCollapse | UI::WindowFlags::NoDecoration | UI::WindowFlags::NoBackground | UI::WindowFlags::NoMove | UI::WindowFlags::NoTitleBar | UI::WindowFlags::NoTabBar);
		{
			m_TopLeftCursorPos = { ImGui::GetCursorPos().x, ImGui::GetCursorPos().y };
			
			auto imWindow = ImGui::GetCurrentWindow();
			auto& position = imWindow->Pos;
			auto& size = imWindow->Size;
			if ((uint32_t)size.x != Track::Viewport::Width || (uint32_t)size.y != Track::Viewport::Height)
				Resize((uint32_t)size.x, (uint32_t)size.y);

			auto& mainWindow = Application::Get().GetWindow();
			Track::Viewport::Position.x = (uint32_t)position.x - mainWindow.GetPositionX();
			Track::Viewport::Position.y = (uint32_t)position.y - mainWindow.GetPositionY();
		}
		{
			auto region = ImGui::GetContentRegionAvail();
			ImGui::Image(Scene::Get()->GetRenderer()->GetResources().Shading.Attachment->GetTextureID(), { region.x, region.y }, { 0.0f, 0.0f }, { 1.0f, 1.0f });
		}
		{
			RenderTopUI();
		}
		ImGui::End();
	}

	void Viewport::Resize(uint32_t width, uint32_t height)
	{
		// Set size
		Track::Viewport::Width = width;
		Track::Viewport::Height = height;

		// Resize
		Scene::Get()->GetRenderer()->Resize(width, height);
	}

	Ref<Viewport> Viewport::Create()
	{
		return RefHelper::Create<Viewport>();
	}

	void Viewport::InitStyles()
	{
		m_Styles = UI::StyleList({
			{ UI::StyleType::FrameRounding, 4.0f }
		});

		m_Colours = UI::StyleList({
			{ UI::StyleColourType::WindowBg, UI::Colours::BackgroundDark },
			//{ UI::StyleColourType::Header, UI::Colours::BackgroundPopup },
			//{ UI::StyleColourType::HeaderHovered, UI::Colours::LightTint },
			//{ UI::StyleColourType::HeaderActive, UI::Colours::LighterTint },
		});
	}

	void Viewport::InitResources()
	{
		ImageSpecification specs = {};
		specs.Usage = ImageUsage::File;
		specs.Layout = ImageLayout::ShaderRead;
		specs.Flags = ImageUsageFlags::Sampled | ImageUsageFlags::Colour;
		specs.CreateUIImage = true;

		specs.Path = Track::Lavender::Directory / "Editor/Resources/Images/Play.png";
		m_PlayButton = Image2D::Create(specs);

		specs.Path = Track::Lavender::Directory / "Editor/Resources/Images/Stop.png";
		m_StopButton = Image2D::Create(specs);
	}

	void Viewport::RenderTopUI()
	{
		m_Styles.Push();
		m_Colours.Push();

		UI::SetCursorPos(m_TopLeftCursorPos);

		Ref<Image2D> playButton = ((Project::Get()->GetState() == WorkSpace::State::Editor) ? m_PlayButton: m_StopButton);
		UI::ShiftCursorX(((float)Track::Viewport::Width / 2.0f) - ((32.0f / 2.0f)));
		UI::ShiftCursorY(1.0f);

		if (ImGui::ImageButton((ImTextureID)playButton->GetTextureID(), { 32.0f, 32.0f }))
			Project::Get()->SwitchState();

		m_Styles.Pop();
		m_Colours.Pop();
	}

}