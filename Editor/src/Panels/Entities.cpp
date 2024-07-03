#include "Entities.hpp"

#include <Lavender/Core/Input/Input.hpp>

#include <Lavender/UI/UI.hpp>

#include <Lavender/WorkSpace/Project.hpp>

namespace Lavender::UI
{

	Entities::Entities()
	{
		InitStyles();

		// Select the first entity as the selected on startup
		{
			auto& entities = Scene::Get()->GetRegistry(Project::Get()->GetState()).GetDict();
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

		// Check for pop-up
		static UUID entityHovered = UUID::Empty;
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			glm::vec2 windowPos = { ImGui::GetCurrentWindow()->Pos.x - ImGui::GetMainViewport()->Pos.x, ImGui::GetCurrentWindow()->Pos.y - ImGui::GetMainViewport()->Pos.y };
			glm::vec2 windowSize = { ImGui::GetCurrentWindow()->Size.x, ImGui::GetCurrentWindow()->Size.y };

			if (Utils::ToolKit::PositionInRect(Input::GetCursorPosition(), windowPos, windowSize))
			{
				ImGui::OpenPopup("New Object");
				entityHovered = UUID::Empty;
			}
		}

		// All Entities
		auto& entities = Scene::Get()->GetRegistry(Project::Get()->GetState()).GetDict();
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
			bool wasSelected = selected;
			bool pressed = UI::Selectable(fmt::format("{0}##{1}", tag.Tag, uuid.String()), selected);

			if (pressed && wasSelected) m_SelectedEntity = UUID::Empty;
			else if (selected) m_SelectedEntity = uuid;

			// Extra data for popup
			if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
				entityHovered = uuid;
		}

		// Actual pop-up
		if (ImGui::BeginPopupContextItem("New Object"))
		{
			if (ImGui::BeginMenu(" New        "))
			{
				// Entity creation
				if (ImGui::MenuItem("Entity"))
				{
					m_SelectedEntity = Scene::Get()->GetRegistry(Project::Get()->GetState()).CreateEntity();

					Entity& entity = Scene::Get()->GetRegistry(Project::Get()->GetState()).GetEntity(m_SelectedEntity);
					entity.AddComponent<TagComponent>();
					entity.AddComponent<TransformComponent>();
				}

				ImGui::EndMenu();
			}

			if (entityHovered != UUID::Empty && ImGui::MenuItem(" Delete"))
			{
				Scene::Get()->GetRegistry(Project::Get()->GetState()).RemoveEntity(entityHovered);
				
				// Select the first entity as the selected
				{
					auto& entities = Scene::Get()->GetRegistry(Project::Get()->GetState()).GetDict();
					for (auto& [uuid, entity] : entities)
					{
						m_SelectedEntity = uuid;
						break;
					}
				}
			}

			ImGui::EndMenu();
		}

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