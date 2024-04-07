#include "EntitiesPanel.hpp"

#include <Lavender/Core/Logging.hpp>
#include <Lavender/Core/Input/Input.hpp>

#include <Lavender/Utils/Profiler.hpp>

#include <Lavender/UI/UI.hpp>
#include <Lavender/UI/Style.hpp>
#include <Lavender/UI/Draw.hpp>
#include <Lavender/UI/Colours.hpp>

namespace Lavender
{

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Util functions
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template<typename TComponent>
	void AddComponentUI(Entity& selected)
	{
		if (!selected.HasComponent<TComponent>() 
			&& ImGui::MenuItem(ComponentToString<TComponent>().c_str()))
		{
			selected.AddComponent<TComponent>();
		}
	}

	template<typename... TComponents>
	void AddComponentsUI(ComponentGroup<TComponents...> group, Entity& selected)
	{
		// Note(Jorben): Empty function for when there are no components left
	}

	template<typename FirstComponent, typename ... RestComponents>
	void AddComponentsUI(ComponentGroup<FirstComponent, RestComponents...> group, Entity& selected)
	{
		AddComponentUI<FirstComponent>(selected);
		AddComponentsUI<RestComponents...>(ComponentGroup<RestComponents...>(), selected);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Main Functions
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	EntitiesPanel::EntitiesPanel(Ref<Project> project)
		: m_Project(project)
	{
		ImageSpecification specs = {};
		specs.Usage = ImageSpecification::ImageUsage::File;
		specs.Flags = ImageSpecification::ImageUsageFlags::Sampled;
		specs.Path = std::filesystem::path(Utils::ToolKit::GetEnv("LAVENDER_DIR") + "\\Editor\\assets\\images\\grey-plus.png");
		m_PlusIcon = Image2D::Create(specs);
	}

	EntitiesPanel::~EntitiesPanel()
	{
	}

	void EntitiesPanel::RenderUI()
	{
		LV_PROFILE_SCOPE("EntitiesPanel::RenderUI");
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
				{ UI::StyleColourType::HeaderActive, UI::Colours::LighterTint },
			});

			UI::BeginWindow("Entities", UI::WindowFlags::NoCollapse | UI::WindowFlags::NoDecoration | UI::WindowFlags::NoTitleBar | UI::WindowFlags::NoMove);

			// Popup for new objects
			static UUID entityToDelete = UUID::Empty;
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			{
				ImVec2 windowPos = ImGui::GetCurrentWindow()->Pos;
				ImVec2 mainViewportPos = ImGui::GetMainViewport()->Pos;
				ImVec2 relativePos = { windowPos.x - mainViewportPos.x, windowPos.y - mainViewportPos.y };

				ImVec2 windowSize = ImGui::GetCurrentWindow()->Size;

				glm::vec2 pos = Input::GetMousePosition();
				if (pos.x >= relativePos.x && pos.x <= relativePos.x + windowSize.x &&
					pos.y >= relativePos.y && pos.y <= relativePos.y + windowSize.y)
				{
					ImGui::OpenPopup("New Object");
					entityToDelete = UUID::Empty;
				}
			}

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

				if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
					entityToDelete = entity.first;
			}

			//Actual pop-up
			{
				UI::ScopedStyleList popupStyles = UI::StyleList({
					{ UI::StyleType::PopupRounding, 6.0f }
				});

				UI::ScopedStyleList popupColours = UI::StyleColourList({
					{ UI::StyleColourType::Header, UI::Colours::DarkTint },
					{ UI::StyleColourType::HeaderHovered, UI::Colours::LightTint },
					{ UI::StyleColourType::HeaderActive, UI::Colours::LighterTint }
				});

				if (ImGui::BeginPopupContextItem("New Object"))
				{
					if (ImGui::BeginMenu(" New        "))
					{
						// Entity
						if (ImGui::MenuItem("Entity"))
						{
							Entity e = m_Project->GetSceneCollection().GetActive()->CreateEntity();
							m_SelectedUUID = e.GetUUID();
						}

						ImGui::EndMenu();
					}

					if (entityToDelete != UUID::Empty && ImGui::MenuItem(" Delete"))
					{
						if (m_SelectedUUID == entityToDelete)
							m_SelectedUUID = 0;

						m_Project->GetSceneCollection().GetActive()->DeleteEntity(entityToDelete);
					}

					ImGui::EndMenu();
				}
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
				{ UI::StyleType::FrameRounding, 4.0f },
				{ UI::StyleType::FramePadding, { 15.0f, 6.0f } }
			});

			UI::ScopedStyleList colours = UI::StyleColourList({
				{ UI::StyleColourType::WindowBg, UI::Colours::Background },
				{ UI::StyleColourType::FrameBg, UI::Colours::AlphaTint },
				{ UI::StyleColourType::FrameBgHovered, UI::Colours::LightTint },
				{ UI::StyleColourType::FrameBgActive, UI::Colours::LighterTint },
				{ UI::StyleColourType::CheckMark, UI::Colours::LightestTint },
			});

			UI::BeginWindow("Components", UI::WindowFlags::NoCollapse | UI::WindowFlags::NoDecoration | UI::WindowFlags::NoTitleBar | UI::WindowFlags::NoMove);

			auto size = ImGui::GetCurrentWindow()->Size;
			m_Width = (uint32_t)size.x;
			m_Height = (uint32_t)size.y;

			// Check if no entity is selected and end prematurely.
			if (m_SelectedUUID == 0)
			{
				UI::EndWindow();
				return;
			}

			auto registry = m_Project->GetSceneCollection().GetActive()->GetCollection()->GetMainRegistry();
			auto assetManager = m_Project->GetSceneCollection().GetActive()->GetAssetManager();

			// TagComponent
			if (registry->HasComponent<TagComponent>(m_SelectedUUID))
			{
				UI::Draw::Underline();
				UI::Dummy({ 0.0f, 2.0f });

				TagComponent& tag = registry->GetComponent<TagComponent>(m_SelectedUUID);

				static char buffer[256] = {};
				std::strncpy(buffer, tag.Tag.c_str(), sizeof(buffer) - 1);

				ImGuiStyle& style = ImGui::GetStyle();
				ImGui::PushItemWidth(m_Width - (2.0f * style.FramePadding.x) - 22.0f);
				UI::InputText("##Entity_Tag_LavenderUI", buffer, sizeof(buffer));
				ImGui::PopItemWidth();

				{
					UI::ScopedStyleList styles = UI::StyleColourList({
						{ UI::StyleColourType::Button, { 0.0f, 0.0f, 0.0f, 0.0f } },
						{ UI::StyleColourType::ButtonHovered, { 0.0f, 0.0f, 0.0f, 0.0f } },
						{ UI::StyleColourType::ButtonActive, { 0.0f, 0.0f, 0.0f, 0.0f } },
					});

					ImGui::SameLine();
					UI::ShiftCursorX(-10.0f);
					if (ImGui::ImageButton(m_PlusIcon->GetUIImage(), { 13.0f, 13.0f }))
					{
						ImGui::OpenPopup("Add Component");
					}
				}

				tag.Tag = std::string(buffer);

				UI::Dummy({ 0.0f, 2.0f });
				UI::Draw::Underline();
				UI::Dummy({ 0.0f, 2.0f });
			}

			// TransformComponent
			if (registry->HasComponent<TransformComponent>(m_SelectedUUID))
			{
				TransformComponent& transform = registry->GetComponent<TransformComponent>(m_SelectedUUID);

				ComponentUsage usage = BeginECSComponent<TransformComponent>();
				if (usage & ComponentUsage::Opened)
				{
					static bool uniformSize = false;

					// TODO: Look into ImGui tables, so the user doesn't have to move the column line every time.
					UI::BeginPropertyGrid(2);

					UI::ScopedStyleList colours = UI::StyleColourList({
						{ UI::StyleColourType::FrameBgHovered, UI::Colours::LightTint },
						{ UI::StyleColourType::FrameBgActive, UI::Colours::LighterTint },
						{ UI::StyleColourType::FrameBg, UI::Colours::NearBlack }
					});

					UI::Property("Position", transform.Position);
					UI::UniformProperty("Size", transform.Size, uniformSize, 0.1f, 0.0f, 0.0f, "%.2f", "Uniform Scaling");
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
							std::pair<std::string, UI::Combo::SelectionFunc> item = std::make_pair(name, [this, assetManager, asset]()
							{
								auto registry = m_Project->GetSceneCollection().GetActive()->GetCollection()->GetMainRegistry();
								MeshComponent& mesh = registry->GetComponent<MeshComponent>(m_SelectedUUID);
								mesh.MeshObject = RefHelper::RefAs<MeshAsset>(assetManager->GetAsset(asset.first));
							});

							meshCombo.Items.push_back(item);
							if (mesh.MeshObject && mesh.MeshObject->GetHandle() == asset.first)
							{
								meshCombo.Selected = item.first;
								meshCombo.Preview = item.first;
							}
							else if (meshCombo.Selected == "None")
							{
								meshCombo.Selected = "None";
								meshCombo.Preview = "Select";
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
							std::pair<std::string, UI::Combo::SelectionFunc> item = std::make_pair(name, [this, assetManager, asset]()
							{
								auto registry = m_Project->GetSceneCollection().GetActive()->GetCollection()->GetMainRegistry();
								MeshComponent& mesh = registry->GetComponent<MeshComponent>(m_SelectedUUID);
								mesh.Material = RefHelper::RefAs<MaterialAsset>(assetManager->GetAsset(asset.first));
							});

							materialCombo.Items.push_back(item);
							if (mesh.Material&& mesh.Material->GetHandle() == asset.first)
							{
								materialCombo.Selected = item.first;
								materialCombo.Preview = item.first;
							}
							else if (materialCombo.Selected == "None")
							{
								materialCombo.Selected = "None";
								materialCombo.Preview = "Select";
							}

						}
						UI::Property("Material", materialCombo);
					}

					UI::EndPropertyGrid();
				}
			}

			// TransformComponent
			if (registry->HasComponent<DirectionalLightComponent>(m_SelectedUUID))
			{
				DirectionalLightComponent& light = registry->GetComponent<DirectionalLightComponent>(m_SelectedUUID);

				ComponentUsage usage = BeginECSComponent<DirectionalLightComponent>("DirectionalLight");
				if (usage & ComponentUsage::Opened)
				{
					// TODO: Look into ImGui tables, so the user doesn't have to move the column line every time.
					UI::BeginPropertyGrid(2);

					UI::ScopedStyleList colours = UI::StyleColourList({
						{ UI::StyleColourType::FrameBgHovered, UI::Colours::LightTint },
						{ UI::StyleColourType::FrameBgActive, UI::Colours::LighterTint },
						{ UI::StyleColourType::FrameBg, UI::Colours::NearBlack }
					});

					UI::Property("Direction", light.Direction);
					UI::Property("Colour", light.Colour);
					UI::Property("Intensity", light.Intensity);

					UI::EndPropertyGrid();
				}
			}

			//Actual pop-up
			{
				UI::ScopedStyleList popupStyles = UI::StyleList({
					{ UI::StyleType::PopupRounding, 6.0f }
				});

				UI::ScopedStyleList popupColours = UI::StyleColourList({
					{ UI::StyleColourType::Header, UI::Colours::DarkTint },
					{ UI::StyleColourType::HeaderHovered, UI::Colours::LightTint },
					{ UI::StyleColourType::HeaderActive, UI::Colours::LighterTint }
				});

				if (ImGui::BeginPopupContextItem("Add Component"))
				{
					Entity entity = Entity(m_Project->GetSceneCollection().GetActive()->GetCollection(), m_SelectedUUID);
					AddComponentsUI(AllComponents(), entity);

					ImGui::EndMenu();
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