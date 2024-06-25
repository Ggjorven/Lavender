#include "Components.hpp"

#include <Lavender/UI/UI.hpp>

#include <Lavender/WorkSpace/Project.hpp>

namespace Lavender::UI
{

	Components::Components(Ref<Entities> entities)
		: m_EntitiesRef(entities)
	{
		InitStyles();
	}

	Components::~Components()
	{
	}

	void Components::RenderUI()
	{
		m_Styles.Push();
		m_Colours.Push();

		UI::BeginWindow("Components", UI::WindowFlags::NoCollapse | UI::WindowFlags::NoDecoration | UI::WindowFlags::NoTitleBar | UI::WindowFlags::NoMove | UI::WindowFlags::NoTabBar);

		if (m_EntitiesRef->m_SelectedEntity == UUID::Empty)
		{
			UI::EndWindow();

			m_Styles.Pop();
			m_Colours.Pop();

			return;
		}

		Entity& entity = Project::Get()->GetScenes().GetActive()->GetRegistry(Project::Get()->GetState()).GetEntity(m_EntitiesRef->m_SelectedEntity);


		UI::EndWindow();

		m_Styles.Pop();
		m_Colours.Pop();
	}

	Ref<Components> Components::Create(Ref<Entities> entities)
	{
		return RefHelper::Create<Components>(entities);
	}

	void Components::InitStyles()
	{
		m_Styles = UI::StyleList({
				{ UI::StyleType::FrameRounding, 4.0f },
				{ UI::StyleType::FramePadding, { 15.0f, 6.0f } }
			});

		m_Colours = UI::StyleList({
			{ UI::StyleColourType::WindowBg, UI::Colours::Background },
			//{ UI::StyleColourType::FrameBg, UI::Colours::AlphaTint },
			//{ UI::StyleColourType::FrameBgHovered, UI::Colours::LightTint },
			//{ UI::StyleColourType::FrameBgActive, UI::Colours::LighterTint },
			//{ UI::StyleColourType::CheckMark, UI::Colours::LightestTint },
		});
	}

}