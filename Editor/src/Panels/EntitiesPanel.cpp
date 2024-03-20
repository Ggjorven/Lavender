#include "EntitiesPanel.hpp"

#include <Lavender/Core/Logging.hpp>

#include <Lavender/UI/UI.hpp>
#include <Lavender/UI/Style.hpp>
#include <Lavender/UI/Draw.hpp>
#include <Lavender/UI/Colours.hpp>

namespace Lavender
{

	EntitiesPanel::EntitiesPanel(Ref<Project> project)
		: m_Project(project)
	{
	}

	EntitiesPanel::~EntitiesPanel()
	{
	}

	void EntitiesPanel::RenderUI()
	{
		{
			// To remove the tab bar.
			ImGuiWindowClass window = {};
			window.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
			ImGui::SetNextWindowClass(&window);

			UI::ScopedStyleList styles = UI::StyleList({
				{ UI::StyleType::FrameRounding, 4.0f }
			});

			UI::ScopedStyleList colours = UI::StyleColourList({
				{ UI::StyleColourType::WindowBg, UI::Colours::BackgroundDark }
			});

			UI::BeginWindow("Entities", UI::WindowFlags::NoCollapse | UI::WindowFlags::NoDecoration | UI::WindowFlags::NoTitleBar | UI::WindowFlags::NoMove);

			for (auto& entity : m_Project->GetSceneCollection().GetActive()->GetCollection()->GetMainRegistry()->GetEntityMap())
			{
				// Break on faulty UUID's
				if (entity.first == 0)
					break;

				bool wasSelected = entity.first == m_SelectedUUID;
				bool newSelected = entity.first == m_SelectedUUID;
				bool pressed = UI::Selectable(std::to_string(entity.first.Get()), &newSelected);

				if (pressed && !wasSelected)
					m_SelectedUUID = entity.first;
				else if (pressed && wasSelected)
					m_SelectedUUID = 0;
			}

			UI::EndWindow();
		}
		// -----------------------------------------------------------------------
		{
			// To remove the tab bar.
			ImGuiWindowClass window = {};
			window.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
			ImGui::SetNextWindowClass(&window);

			UI::ScopedStyleList styles = UI::StyleList({
				{ UI::StyleType::FrameRounding, 4.0f }
			});

			UI::ScopedStyleList colours = UI::StyleColourList({
				{ UI::StyleColourType::WindowBg, UI::Colours::Background }
			});

			UI::BeginWindow("Components", UI::WindowFlags::NoCollapse | UI::WindowFlags::NoDecoration | UI::WindowFlags::NoTitleBar | UI::WindowFlags::NoMove);

			// Check if no entity is selected and end prematurely.
			if (m_SelectedUUID == 0)
			{
				UI::EndWindow();
				return;
			}

			auto registry = m_Project->GetSceneCollection().GetActive()->GetCollection()->GetMainRegistry();

			// TagComponent
			/*
			if (registry->HasComponent<TagComponent>(m_SelectedUUID))
			{
				TagComponent& tag = registry->GetComponent<TagComponent>(m_SelectedUUID);

				ComponentUsage usage = BeginECSComponent<TagComponent>();
				if (usage & ComponentUsage::Opened)
				{
					UI::Text("Tag: {}", tag.Tag);
				}
			}
			*/
			// TransformComponent
			if (registry->HasComponent<TransformComponent>(m_SelectedUUID))
			{
				TransformComponent& transform = registry->GetComponent<TransformComponent>(m_SelectedUUID);

				ComponentUsage usage = BeginECSComponent<TransformComponent>();
				if (usage & ComponentUsage::Opened)
				{
					//UI::BeginTable("##LavenderUI", 2, UI::TableFlags::SizingFixedFit | UI::TableFlags::BordersInnerV | UI::TableFlags::NoClip);
					//UI::TableSetupColumn("Label", UI::TableColumnFlags::None, 100.0f);
					//UI::TableSetupColumn("Value", UI::TableColumnFlags::IndentEnable | UI::TableColumnFlags::NoClip, UI::GetContentRegionAvail().x - 100.0f);

					UI::BeginPropertyGrid(2);

					UI::ScopedStyle colour = UI::StyleColour(UI::StyleColourType::FrameBg, UI::Colours::NearBlack);
					{
						UI::ScopedStyleList red = {{
							{ UI::StyleColourType::FrameBgHovered, LV_U32_COLOUR(211, 0, 0, 123) },
							{ UI::StyleColourType::FrameBgActive, LV_U32_COLOUR(255, 0, 0, 172) }
						}};
						UI::Property("Position", transform.Position);
						//UI::TableNextRow();
						//
						//UI::BeginChild("PositionChild", {});
						//
						//UI::TableSetColumnIndex(0);
						//UI::ShiftCursor(17.0f, 7.0f);
						//
						//UI::Text("Position");
						//UI::Draw::Underline(false, 0.0f, 2.0f);
						//
						//UI::TableSetColumnIndex(1);
						//UI::ShiftCursor(7.0f, 0.0f);
						//
						//UI::DragFloat3("##Position_LavenderUI", transform.Position);
						//
						//UI::EndChild();
					}
					{
						UI::ScopedStyleList blue = { {
							{ UI::StyleColourType::FrameBgHovered, LV_U32_COLOUR(0, 171, 35, 123) },
							{ UI::StyleColourType::FrameBgActive, LV_U32_COLOUR(0, 211, 35, 172) }
						} };
						UI::Property("Size", transform.Size);
					}
					{
						UI::ScopedStyleList green = { {
							{ UI::StyleColourType::FrameBgHovered, LV_U32_COLOUR(0, 80, 211, 123) },
							{ UI::StyleColourType::FrameBgActive, LV_U32_COLOUR(0, 80, 255, 172) }
						} };
						UI::Property("Rotation", transform.Rotation);
					}

					//UI::EndTable();
					UI::EndPropertyGrid();
				}
			}

			UI::EndWindow();
		}
	}

	Ref<EntitiesPanel> EntitiesPanel::Create(Ref<Project> project)
	{
		return RefHelper::Create<EntitiesPanel>(project);
	}

}