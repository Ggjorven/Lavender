#include "Components.hpp"

#include <Lavender/Core/Application.hpp>

#include <Lavender/WorkSpace/Project.hpp>
#include <Lavender/WorkSpace/EngineTracker.hpp>

namespace Lavender::UI
{

	//////////////////////////////////////////////////////////////////////////
	// Utils
	//////////////////////////////////////////////////////////////////////////
	template<typename TComponent>
	void RenderComponent(Components* components, Entity& entity)
	{
		if (entity.HasComponent<TComponent>())
			components->RenderComponent<TComponent>(entity);
	}

	template<typename... TComponent>
	void RenderComponents(Utils::TypeGroup<TComponent...> group, Components* components, Entity& entity)
	{
		// Note(Jorben): Empty function for when there are no component types
	}

	template<typename FirstComponent, typename ... RestComponents>
	void RenderComponents(Utils::TypeGroup<FirstComponent, RestComponents...> group, Components* components, Entity& entity)
	{
		RenderComponent<FirstComponent>(components, entity);
		RenderComponents<RestComponents...>(Utils::TypeGroup<RestComponents...>(), components, entity);
	}
	//////////////////////////////////////////////////////////////////////////
	template<typename TComponent>
	void AddComponentUI(Entity& selected)
	{
		if (!selected.HasComponent<TComponent>() && ImGui::MenuItem(ComponentToString<TComponent>().c_str()))
			selected.AddComponent<TComponent>();
	}

	template<typename... TComponents>
	void AddComponentsUI(Utils::TypeGroup<TComponents...> group, Entity& selected)
	{
		// Note(Jorben): Empty function for when there are no components left
	}

	template<typename FirstComponent, typename ... RestComponents>
	void AddComponentsUI(Utils::TypeGroup<FirstComponent, RestComponents...> group, Entity& selected)
	{
		AddComponentUI<FirstComponent>(selected);
		AddComponentsUI<RestComponents...>(Utils::TypeGroup<RestComponents...>(), selected);
	}

	//////////////////////////////////////////////////////////////////////////
	// Core Functions
	//////////////////////////////////////////////////////////////////////////
	Components::Components(Ref<Entities> entities)
		: m_EntitiesRef(entities)
	{
		InitStyles();
		InitIcons();
	}

	Components::~Components()
	{
	}

	void Components::RenderUI()
	{
		m_Styles.Push();
		m_Colours.Push();

		UI::BeginWindow("Components", UI::WindowFlags::NoCollapse | UI::WindowFlags::NoDecoration | UI::WindowFlags::NoTitleBar | UI::WindowFlags::NoMove | UI::WindowFlags::NoTabBar);

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

		// Check if the entity handle is valid
		if (m_EntitiesRef->m_SelectedEntity == UUID::Empty)
		{
			UI::EndWindow();

			m_Styles.Pop();
			m_Colours.Pop();

			return;
		}

		Entity& entity = Scene::Get()->GetRegistry(Project::Get()->GetState()).GetEntity(m_EntitiesRef->m_SelectedEntity);
		if (!entity.HasComponent<TagComponent>())
			entity.AddComponent<TagComponent>();

		// Render all components
		RenderComponents(AllComponents(), this, entity);

		// Add Component pop-up
		{
			UI::ScopedStyleList popupStyles = UI::StyleList({
				{ UI::StyleType::PopupRounding, 6.0f }
			});

			UI::ScopedStyleList popupColours = UI::StyleList(/*{
				{ UI::StyleColourType::Header, UI::Colours::DarkTint },
				{ UI::StyleColourType::HeaderHovered, UI::Colours::LightTint },
				{ UI::StyleColourType::HeaderActive, UI::Colours::LighterTint }
			}*/);

			if (ImGui::BeginPopupContextItem("Add Component"))
			{
				AddComponentsUI(AllComponents(), entity);

				ImGui::EndMenu();
			}
		}

		UI::EndWindow();

		m_Styles.Pop();
		m_Colours.Pop();
	}

	Ref<Components> Components::Create(Ref<Entities> entities)
	{
		return RefHelper::Create<Components>(entities);
	}

	ComponentUsage Components::BeginComponent(const std::string& name, Ref<Image2D> icon)
	{
		static const ImVec2 iconSize = { 16.0f, 16.0f };

		ComponentUsage usage = ComponentUsage::None;

		UI::ScopedStyleList styles = UI::StyleList({
			{ UI::StyleType::FrameRounding, 1.0f },
			{ UI::StyleType::FramePadding, { 15.0f, 6.0f } },
			{ UI::StyleType::FrameBorderSize, 1.0f }
		});

		UI::ScopedStyleList colours = UI::StyleList({
			//{ UI::StyleColourType::Header, UI::Colours::BackgroundPopup },
			//{ UI::StyleColourType::HeaderHovered, UI::Colours::LightTint },
			//{ UI::StyleColourType::HeaderActive, UI::Colours::LighterTint },
			{ UI::StyleColourType::Button, { 0.0f, 0.0f, 0.0f, 0.0f } }
			//{ UI::StyleColourType::ButtonActive, { 0.0f, 0.0f, 0.0f, 0.0f } },
			//{ UI::StyleColourType::ButtonHovered, { 0.0f, 0.0f, 0.0f, 0.0f } }
		});

		// TODO: Icon
	
		// Positions
		glm::vec2 startCursorPos = { ImGui::GetCursorPosX(), ImGui::GetCursorPosY() };
		UI::SetCursorPos({ startCursorPos.x + ImGui::GetContentRegionAvail().x - iconSize.x - 2.0f, startCursorPos.y + (ImGui::GetStyle().FramePadding.y / 2.0f) + 1.0f });
		glm::vec2 componentOptionsPos = { ImGui::GetCursorPosX(), ImGui::GetCursorPosY() };
			
		// Button to trigger pop-up
		{
			UI::ScopedStyleList treeStyles = UI::StyleList({
				{ UI::StyleColourType::Border, { 0.0f, 0.0f, 0.0f, 0.0f } },
				{ UI::StyleColourType::BorderShadow, { 0.0f, 0.0f, 0.0f, 0.0f } }
			});

			if (UI::Button(fmt::format("##{0}_ComponentOptions", name), { iconSize.x, iconSize.y }))
				ImGui::OpenPopup(fmt::format("Component Options##{0}", name).c_str());
		}

		// Actual Tree
		UI::SetCursorPos(startCursorPos);
		{
			UI::ScopedStyleList treeStyles = UI::StyleList({
				{ UI::StyleColourType::Border, UI::Colours::BackgroundDark }
			});

			if (UI::Tree(name))
				usage = usage | ComponentUsage::Opened;
		}

		auto afterTreePos = ImGui::GetCursorPos();

		// Component Options image
		UI::SetCursorPos(componentOptionsPos);
		ImGui::Image((ImTextureID)m_DotsIcon->GetTextureID(), { iconSize.x, iconSize.y });
		
		// Actual pop-up
		if (ImGui::BeginPopup(fmt::format("Component Options##{0}", name).c_str()))
		{
			if (UI::MenuItem("Reset"))
				usage = usage | ComponentUsage::Reset;

			else if (UI::MenuItem("Remove"))
				usage = usage | ComponentUsage::Remove;

			ImGui::EndPopup();
		}

		UI::SetCursorPos({ afterTreePos.x, afterTreePos.y - 1.0f });

		return usage;
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

	void Components::InitIcons()
	{
		ImageSpecification specs = {};
		specs.Usage = ImageUsage::File;
		specs.Layout = ImageLayout::ShaderRead;
		specs.Flags = ImageUsageFlags::Sampled | ImageUsageFlags::Colour;
		specs.CreateUIImage = true;

		specs.Path = Track::Lavender::Directory / "Editor/Resources/Images/3-dots.png";
		m_DotsIcon = Image2D::Create(specs);

		specs.Path = Track::Lavender::Directory / "Editor/Resources/Images/grey-plus.png";
		m_PlusIcon = Image2D::Create(specs);
	}

	//////////////////////////////////////////////////////////////////////////
	// Component Functions
	//////////////////////////////////////////////////////////////////////////
	template<>
	void Components::RenderComponent<TagComponent>(Entity& entity)
	{
		UI::Draw::Underline();
		UI::ShiftCursorY(2.0f);

		TagComponent& tag = entity.GetComponent<TagComponent>();

		static char buffer[256] = {};
		std::strncpy(buffer, tag.Tag.c_str(), sizeof(buffer) - 1);

		ImGuiStyle& style = ImGui::GetStyle();
		ImGui::PushItemWidth(m_Width - (2.0f * style.FramePadding.x) - 22.0f);
		UI::InputText("##Entity_Tag_LavenderUI", buffer, sizeof(buffer));
		ImGui::PopItemWidth();

		{
			UI::ScopedStyleList styles = UI::StyleList({
				{ UI::StyleColourType::Button, { 0.0f, 0.0f, 0.0f, 0.0f } },
				{ UI::StyleColourType::ButtonHovered, { 0.0f, 0.0f, 0.0f, 0.0f } },
				{ UI::StyleColourType::ButtonActive, { 0.0f, 0.0f, 0.0f, 0.0f } },
			});

			ImGui::SameLine();
			UI::ShiftCursorX(-10.0f);
			if (ImGui::ImageButton(m_PlusIcon->GetTextureID(), { 13.0f, 13.0f }))
			{
				ImGui::OpenPopup("Add Component");
			}
		}

		tag.Tag = std::string(buffer);

		UI::ShiftCursorY(2.0f);
		UI::Draw::Underline();
		UI::ShiftCursorY(2.0f);
	}

	template<>
	void Components::RenderComponent<TransformComponent>(Entity& entity)
	{
		TransformComponent& transform = entity.GetComponent<TransformComponent>();

		ComponentUsage usage = BeginComponent("Transform");
		if (usage & ComponentUsage::Remove)
		{
			entity.RemoveComponent<TransformComponent>();
			return;
		}
		else if (usage & ComponentUsage::Reset)
			entity.AddOrReplaceComponent<TransformComponent>();

		if (usage & ComponentUsage::Opened)
		{
			// TODO: Look into ImGui tables, so the user doesn't have to move the column line every time.
			UI::BeginPropertyGrid("##TransformGrid", 2);

			UI::ScopedStyleList colours = UI::StyleList(/*{
				{ UI::StyleColourType::FrameBgHovered, UI::Colours::LightTint },
				{ UI::StyleColourType::FrameBgActive, UI::Colours::LighterTint },
				{ UI::StyleColourType::FrameBg, UI::Colours::NearBlack }
			}*/);

			UI::Property("Position", transform.Position);
			UI::Property("Size", transform.Size);
			UI::Property("Rotation", transform.Rotation);

			UI::EndPropertyGrid("##TransformGrid");
		}
	}

	template<>
	void Components::RenderComponent<MeshComponent>(Entity& entity)
	{
		MeshComponent& mesh = entity.GetComponent<MeshComponent>();

		ComponentUsage usage = BeginComponent("Mesh");
		if (usage & ComponentUsage::Remove)
		{
			entity.RemoveComponent<MeshComponent>();
			return;
		}
		else if (usage & ComponentUsage::Reset)
			entity.AddOrReplaceComponent<MeshComponent>();

		if (usage & ComponentUsage::Opened)
		{
			UI::ScopedStyleList colours = UI::StyleList(/*{
				{ UI::StyleColourType::Header, UI::Colours::BackgroundPopup },
				{ UI::StyleColourType::HeaderHovered, UI::Colours::LightTint },
				{ UI::StyleColourType::HeaderActive, UI::Colours::LighterTint },
				{ UI::StyleColourType::FrameBg, UI::Colours::AlphaTint },
				{ UI::StyleColourType::FrameBgHovered, UI::Colours::LightTint },
				{ UI::StyleColourType::FrameBgActive, UI::Colours::LighterTint },
				{ UI::StyleColourType::Button, UI::Colours::DarkTint },
				{ UI::StyleColourType::ButtonHovered, UI::Colours::DarkTint },
				{ UI::StyleColourType::ButtonActive, UI::Colours::DarkTint }
			}*/);

			UI::BeginPropertyGrid("##MeshGrid", 2);

			// Mesh Combo
			{
				UI::Combo meshes = {};

				// Loaded meshes
				for (auto& [handle, asset] : Project::Get()->GetAssets()->GetCollection().GetAssets())
				{
					if (asset->GetType() != AssetType::Mesh) continue;

					std::pair<std::string, UI::Combo::SelectionFunc> item = std::make_pair(asset->GetName(), [asset, selected = m_EntitiesRef->m_SelectedEntity]()
					{
						MeshComponent& mesh = Scene::Get()->GetRegistry(Project::Get()->GetState()).GetEntity(selected).GetComponent<MeshComponent>();
						mesh.Mesh = RefHelper::RefAs<MeshAsset>(asset);
					});
					meshes.Add(item);

					// Set the title if current mesh is selected
					if (mesh.Mesh && mesh.Mesh->GetHandle() == handle)
					{
						meshes.Selected = item.first;
						meshes.Preview = item.first;
					}
				}

				// Cached meshes
				for (auto& [handle, data] : Project::Get()->GetAssets()->GetCache().GetAssets())
				{
					if (data.Type != AssetType::Mesh) continue;

					std::pair<std::string, UI::Combo::SelectionFunc> item = std::make_pair(data.Name, [data, selected = m_EntitiesRef->m_SelectedEntity]()
					{
						MeshComponent& mesh = Scene::Get()->GetRegistry(Project::Get()->GetState()).GetEntity(selected).GetComponent<MeshComponent>();

						Ref<Asset> newMesh = MeshAsset::Create(data);
						newMesh->Deserialize();

						Project::Get()->GetAssets()->AddAsset(newMesh);

						mesh.Mesh = RefHelper::RefAs<MeshAsset>(newMesh);
					});
					meshes.Add(item);
				}

				UI::Property("Mesh", meshes);
			}

			// Material Combo
			{
				UI::Combo materials = {};

				// Loaded materials
				for (auto& [handle, asset] : Project::Get()->GetAssets()->GetCollection().GetAssets())
				{
					if (asset->GetType() != AssetType::Material) continue;

					std::pair<std::string, UI::Combo::SelectionFunc> item = std::make_pair(asset->GetName(), [asset, selected = m_EntitiesRef->m_SelectedEntity]()
					{
						MeshComponent& mesh = Scene::Get()->GetRegistry(Project::Get()->GetState()).GetEntity(selected).GetComponent<MeshComponent>();
						mesh.Material = RefHelper::RefAs<MaterialAsset>(asset);
					});
					materials.Add(item);

					// Set the title if current mesh is selected
					if (mesh.Material && mesh.Material->GetHandle() == handle)
					{
						materials.Selected = item.first;
						materials.Preview = item.first;
					}
				}

				// Cached materials
				for (auto& [handle, data] : Project::Get()->GetAssets()->GetCache().GetAssets())
				{
					if (data.Type != AssetType::Material) continue;

					std::pair<std::string, UI::Combo::SelectionFunc> item = std::make_pair(data.Name, [data, selected = m_EntitiesRef->m_SelectedEntity]()
					{
						MeshComponent& mesh = Scene::Get()->GetRegistry(Project::Get()->GetState()).GetEntity(selected).GetComponent<MeshComponent>();

						Ref<Asset> newMaterial = MaterialAsset::Create(data);
						newMaterial->Deserialize();

						Project::Get()->GetAssets()->AddAsset(newMaterial);

						mesh.Material = RefHelper::RefAs<MaterialAsset>(newMaterial);
					});
					materials.Add(item);
				}

				UI::Property("Material", materials);
			}

			UI::EndPropertyGrid("##MeshGrid");
		}
	}

	template<>
	void Components::RenderComponent<PointLightComponent>(Entity& entity)
	{
		PointLightComponent& pointLight = entity.GetComponent<PointLightComponent>();

		ComponentUsage usage = BeginComponent("PointLight");
		if (usage & ComponentUsage::Remove)
		{
			entity.RemoveComponent<PointLightComponent>();
			return;
		}
		else if (usage & ComponentUsage::Reset)
			entity.AddOrReplaceComponent<PointLightComponent>();

		if (usage & ComponentUsage::Opened)
		{
			UI::ScopedStyleList colours = UI::StyleList(/*{
				{ UI::StyleColourType::FrameBgHovered, UI::Colours::LightTint },
				{ UI::StyleColourType::FrameBgActive, UI::Colours::LighterTint },
				{ UI::StyleColourType::FrameBg, UI::Colours::NearBlack }
			}*/);

			UI::BeginPropertyGrid("##PointLightGrid", 2);

			{
				glm::vec4 tempColour = glm::vec4(pointLight.Colour, 1.0f);

				UI::ColourPicker colourPicker = UI::ColourPicker(tempColour);
				colourPicker.Label = "Colour";
				colourPicker.Size = 40.0f;

				UI::Property("Colour", colourPicker);

				pointLight.Colour = tempColour;
			}

			UI::Property("Radius", pointLight.Radius);
			UI::Property("Intensity", pointLight.Intensity);

			UI::EndPropertyGrid("##PointLightGrid");
		}
	}

	template<>
	void Components::RenderComponent<ScriptComponent>(Entity& entity)
	{
		ScriptComponent& script = entity.GetComponent<ScriptComponent>();

		ComponentUsage usage = BeginComponent("Script");
		if (usage & ComponentUsage::Remove)
		{
			entity.RemoveComponent<ScriptComponent>();
			return;
		}
		else if (usage & ComponentUsage::Reset)
			entity.AddOrReplaceComponent<ScriptComponent>();

		if (usage & ComponentUsage::Opened)
		{
			UI::ScopedStyleList colours = UI::StyleList(/*{
				{ UI::StyleColourType::FrameBgHovered, UI::Colours::LightTint },
				{ UI::StyleColourType::FrameBgActive, UI::Colours::LighterTint },
				{ UI::StyleColourType::FrameBg, UI::Colours::NearBlack }
			}*/);

			UI::BeginPropertyGrid("##ScriptGrid", 2);

			{
				UI::Combo classes = {};

				for (auto& cls : Project::Get()->GetScript()->GetClasses())
				{
					std::pair<std::string, UI::Combo::SelectionFunc> item = std::make_pair(cls, [cls, selected = m_EntitiesRef->m_SelectedEntity]()
					{
						ScriptComponent& script = Scene::Get()->GetRegistry(Project::Get()->GetState()).GetEntity(selected).GetComponent<ScriptComponent>();

						if (Project::Get()->GetScript())
							Project::Get()->GetScript()->AddInstance(cls, selected);

						script = cls;
					});
					classes.Add(item);

					// Set the title if current class is selected
					if (script.Class == cls)
					{
						classes.Selected = item.first;
						classes.Preview = item.first;
					}
				}

				UI::Property("Class", classes);
			}

			UI::EndPropertyGrid("##ScriptGrid");
		}
	}

	template<>
	void Components::RenderComponent<CameraComponent>(Entity& entity)
	{
		CameraComponent& camera = entity.GetComponent<CameraComponent>();

		ComponentUsage usage = BeginComponent("Camera");
		if (usage & ComponentUsage::Remove)
		{
			entity.RemoveComponent<CameraComponent>();
			return;
		}
		else if (usage & ComponentUsage::Reset)
			entity.AddOrReplaceComponent<CameraComponent>();

		if (usage & ComponentUsage::Opened)
		{
			UI::BeginPropertyGrid("##CameraGrid", 2);

			UI::Property("Yaw", camera.Yaw);
			UI::Property("Pitch", camera.Pitch);
			UI::Property("FOV", camera.FOV);

			// Active
			{
				UI::Combo active = {};
				active.Add(std::make_pair("True", [selected = m_EntitiesRef->m_SelectedEntity]() {
					CameraComponent& camera = Scene::Get()->GetRegistry(Project::Get()->GetState()).GetEntity(selected).GetComponent<CameraComponent>();
					camera.Active = true;
				}));
				active.Add(std::make_pair("False", [selected = m_EntitiesRef->m_SelectedEntity]() {
					CameraComponent& camera = Scene::Get()->GetRegistry(Project::Get()->GetState()).GetEntity(selected).GetComponent<CameraComponent>();
					camera.Active = false;
				}));

				if (camera.Active == true)
				{
					active.Selected = "True";
					active.Preview = "True";
				}
				else
				{
					active.Selected = "False";
					active.Preview = "False";
				}

				UI::Property("Active", active);
			}

			UI::Property("Near", camera.Near);
			UI::Property("Far", camera.Far);

			UI::EndPropertyGrid("##CameraGrid");
		}
	}

}