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

			auto registry = m_Project->GetSceneCollection().GetActive()->GetCollection()->GetMainRegistry();
			for (auto& entity : registry->GetEntityMap())
			{
				// Break on faulty UUID's
				if (entity.first == 0)
					break;

				if (!registry->HasComponent<TagComponent>(entity.first))
					registry->AddComponent<TagComponent>(entity.first);
				TagComponent& tag = registry->GetComponent<TagComponent>(entity.first);

				bool wasSelected = entity.first == m_SelectedUUID;
				bool newSelected = entity.first == m_SelectedUUID;
				bool pressed = UI::Selectable(tag.Tag + std::string("##") + std::to_string(entity.first.Get()), &newSelected);

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
					// TODO: Look into ImGui tables, so the user doesn't have to move the column line every time.
					UI::BeginPropertyGrid(2);

					UI::ScopedStyle colour = UI::StyleColour(UI::StyleColourType::FrameBg, UI::Colours::NearBlack);
					{
						UI::ScopedStyleList red = {{
							{ UI::StyleColourType::FrameBgHovered, LV_U32_COLOUR(211, 0, 0, 123) },
							{ UI::StyleColourType::FrameBgActive, LV_U32_COLOUR(255, 0, 0, 172) }
						}};
						UI::Property("Position", transform.Position);
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

					UI::EndPropertyGrid();
				}
			}

			// MeshComponent
			if (registry->HasComponent<MeshComponent>(m_SelectedUUID))
			{
				MeshComponent& mesh = registry->GetComponent<MeshComponent>(m_SelectedUUID);

				ComponentUsage usage = BeginECSComponent<MeshComponent>();
				if (usage & ComponentUsage::Opened)
				{
					{
						UI::Text("Mesh: ");
						UI::SameLine();

						if (UI::BeginCombo("##MeshAsset_LavenderUI", "Select Mesh"))
						{
							// TODO: Improve system
							for (auto& asset : m_Project->GetSceneCollection().GetActive()->GetAssetManager()->GetAssets())
							{
								if (asset.second->GetStaticType() != AssetType::MeshAsset)
									continue;

								bool selected = false;
								if (mesh.MeshObject)
									selected = asset.second->GetAssetPath().filename() == mesh.MeshObject->GetAssetPath().filename();

								if (UI::Selectable(asset.second->GetAssetPath().filename().string(), &selected))
								{
									mesh.MeshObject = RefHelper::RefAs<MeshAsset>(asset.second);
								}
							}
							UI::EndCombo();
						}
					}
					{
						UI::Text("Material: ");
						UI::SameLine();

						if (UI::BeginCombo("##MaterialAsset_LavenderUI", "Select Material"))
						{
							// TODO: Improve system
							for (auto& asset : m_Project->GetSceneCollection().GetActive()->GetAssetManager()->GetAssets())
							{
								if (asset.second->GetStaticType() != AssetType::MaterialAsset)
									continue;

								bool selected = false;
								if (mesh.Material)
									selected = asset.second->GetAssetPath().filename() == mesh.Material->GetAssetPath().filename();

								if (UI::Selectable(asset.second->GetAssetPath().filename().string(), &selected))
								{
									mesh.Material = RefHelper::RefAs<MaterialAsset>(asset.second);
								}
							}
							UI::EndCombo();
						}
					}
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