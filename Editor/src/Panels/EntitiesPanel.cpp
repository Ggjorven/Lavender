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
				{ UI::StyleColourType::WindowBg, UI::Colours::BackgroundDark },
				{ UI::StyleColourType::Header, UI::Colours::BackgroundPopup },
				{ UI::StyleColourType::HeaderHovered, UI::Colours::LightTint },
				{ UI::StyleColourType::HeaderActive, UI::Colours::LighterTint }
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
					static Dict<UUID, bool> uniformSize = { };

					// TODO: Look into ImGui tables, so the user doesn't have to move the column line every time.
					UI::BeginPropertyGrid(2);

					UI::ScopedStyleList colours = UI::StyleColourList({
						{ UI::StyleColourType::FrameBgHovered, UI::Colours::LightTint },
						{ UI::StyleColourType::FrameBgActive, UI::Colours::LighterTint },
						{ UI::StyleColourType::FrameBg, UI::Colours::NearBlack }
					});
					UI::Property("Position", transform.Position);
					UI::UniformProperty("Size", transform.Size, uniformSize[m_SelectedUUID], 0.1f, 0.0f, 0.0f, "%.2f", "Uniform Scaling");
					UI::Property("Rotation", transform.Rotation);

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
					UI::ScopedStyleList colours = {{
						{ UI::StyleColourType::Header, UI::Colours::BackgroundPopup },
						{ UI::StyleColourType::HeaderHovered, UI::Colours::LightTint },
						{ UI::StyleColourType::HeaderActive, UI::Colours::LighterTint },
						{ UI::StyleColourType::FrameBg, UI::Colours::AlphaTint },
						{ UI::StyleColourType::FrameBgHovered, UI::Colours::LightTint },
						{ UI::StyleColourType::FrameBgActive, UI::Colours::LighterTint },
						{ UI::StyleColourType::Button, UI::Colours::DarkTint },
						{ UI::StyleColourType::ButtonHovered, UI::Colours::DarkTint },
						{ UI::StyleColourType::ButtonActive, UI::Colours::DarkTint }
					}};

					UI::BeginPropertyGrid(2);
					
					{
						UI::Combo meshCombo = {};
						for (auto& asset : m_Project->GetSceneCollection().GetActive()->GetAssetManager()->GetAssets())
						{
							if (asset.second->GetStaticType() != AssetType::MeshAsset)
								continue;

							auto path = asset.second->GetAssetPath();
							std::string name = path.filename().replace_extension().string();
							std::pair<std::string, UI::Combo::SelectionFunc> item = std::make_pair(name, [this, asset]()
							{
								auto registry = m_Project->GetSceneCollection().GetActive()->GetCollection()->GetMainRegistry();
								MeshComponent& mesh = registry->GetComponent<MeshComponent>(m_SelectedUUID);
								mesh.MeshObject = RefHelper::RefAs<MeshAsset>(m_Project->GetSceneCollection().GetActive()->GetAssetManager()->GetAsset(asset.first));
							});

							meshCombo.Items.push_back(item);
							if (mesh.MeshObject->GetHandle() == asset.first)
							{
								meshCombo.Selected = item.first;
								meshCombo.Preview = item.first;
							}
						}
						UI::Property("Mesh", meshCombo);
					}
					{
						UI::Combo materialCombo = {};
						for (auto& asset : m_Project->GetSceneCollection().GetActive()->GetAssetManager()->GetAssets())
						{
							if (asset.second->GetStaticType() != AssetType::MaterialAsset)
								continue;

							auto path = asset.second->GetAssetPath();
							std::string name = path.filename().replace_extension().string();
							std::pair<std::string, UI::Combo::SelectionFunc> item = std::make_pair(name, [this, asset]()
							{
								auto registry = m_Project->GetSceneCollection().GetActive()->GetCollection()->GetMainRegistry();
								MeshComponent& mesh = registry->GetComponent<MeshComponent>(m_SelectedUUID);
								mesh.Material = RefHelper::RefAs<MaterialAsset>(m_Project->GetSceneCollection().GetActive()->GetAssetManager()->GetAsset(asset.first));
							});

							materialCombo.Items.push_back(item);
							if (mesh.Material->GetHandle() == asset.first)
							{
								materialCombo.Selected = item.first;
								materialCombo.Preview = item.first;
							}

						}
						UI::Property("Material", materialCombo);
					}

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