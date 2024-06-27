#include "Entities.hpp"

#include <Lavender/UI/UI.hpp>

#include <Lavender/WorkSpace/Project.hpp>

namespace Lavender::UI
{

	Entities::Entities()
	{
		InitStyles();

		// Select the first entity as the selected on startup
		{
			auto& entities = Project::Get()->GetScenes().GetActive()->GetRegistry(Project::Get()->GetState()).GetDict();
			for (auto& [uuid, entity] : entities)
			{
				m_SelectedEntity = uuid;
				break;
			}
		}
	}

	Entities::~Entities()
	{
	}

	void Entities::RenderUI()
	{
		m_Styles.Push();
		m_Colours.Push();

		UI::BeginWindow("Entities", UI::WindowFlags::NoCollapse | UI::WindowFlags::NoDecoration | UI::WindowFlags::NoTitleBar | UI::WindowFlags::NoMove | UI::WindowFlags::NoTabBar);

		auto& entities = Project::Get()->GetScenes().GetActive()->GetRegistry(Project::Get()->GetState()).GetDict();
		for (auto& [uuid, entity] : entities)
		{
			if (uuid == UUID::Empty) break;

			if (!entity.HasComponent<TagComponent>())
			{
				APP_LOG_WARN("[Entities-UI] Entity by UUID: {0} doesn't have a TagComponent.", (uint64_t)uuid);
				entity.AddComponent<TagComponent>();
			}
			TagComponent& tag = entity.GetComponent<TagComponent>();

			bool selected = (uuid == m_SelectedEntity);
			UI::Selectable(fmt::format("{0}##{1}", tag.Tag, uuid.String()), &selected);

			if (selected) m_SelectedEntity = uuid;
		}

		// TODO: Popup

		UI::EndWindow();

		m_Styles.Pop();
		m_Colours.Pop();
	}

	Ref<Entities> Entities::Create()
	{
		return RefHelper::Create<Entities>();
	}

	void Entities::InitStyles()
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

}