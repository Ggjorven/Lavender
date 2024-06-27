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
	void RenderComponents(ComponentGroup<TComponent...> group, Components* components, Entity& entity)
	{
		// Note(Jorben): Empty function for when there are no component types
	}

	template<typename FirstComponent, typename ... RestComponents>
	void RenderComponents(ComponentGroup<FirstComponent, RestComponents...> group, Components* components, Entity& entity)
	{
		RenderComponent<FirstComponent>(components, entity);
		RenderComponents<RestComponents...>(ComponentGroup<RestComponents...>(), components, entity);
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

		Entity& entity = Project::Get()->GetScenes().GetActive()->GetRegistry(Project::Get()->GetState()).GetEntity(m_EntitiesRef->m_SelectedEntity);
		if (!entity.HasComponent<TagComponent>())
			entity.AddComponent<TagComponent>();

		// Render all components
		RenderComponents(AllComponents(), this, entity);

		// TODO: Popup for adding components

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

	void Components::InitIcons()
	{
		ImageSpecification specs = {};
		specs.Usage = ImageUsage::File;
		specs.Layout = ImageLayout::ShaderRead;
		specs.Flags = ImageUsageFlags::Sampled | ImageUsageFlags::Colour;
		specs.Path = std::filesystem::path(Track::Lavender::Directory / "Editor/Resources/Images/grey-plus.png");
		specs.CreateUIImage = true;

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

		ComponentUsage usage = BeginComponent<TransformComponent>();
		if (usage & ComponentUsage::Opened)
		{
			static bool uniformSize = false;

			// TODO: Look into ImGui tables, so the user doesn't have to move the column line every time.
			UI::BeginPropertyGrid(2);

			UI::ScopedStyleList colours = UI::StyleList(/*{
				{ UI::StyleColourType::FrameBgHovered, UI::Colours::LightTint },
				{ UI::StyleColourType::FrameBgActive, UI::Colours::LighterTint },
				{ UI::StyleColourType::FrameBg, UI::Colours::NearBlack }
			}*/);

			UI::Property("Position", transform.Position);
			UI::UniformProperty("Size", transform.Size, uniformSize, 0.1f, 0.0f, 0.0f, "%.2f", "Uniform Scaling");
			UI::Property("Rotation", transform.Rotation);

			UI::EndPropertyGrid();
		}
	}

	template<>
	void Components::RenderComponent<MeshComponent>(Entity& entity)
	{
		MeshComponent& mesh = entity.GetComponent<MeshComponent>();

		ComponentUsage usage = BeginComponent<MeshComponent>();
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

			UI::BeginPropertyGrid(2);

			// Mesh Combo
			{
				UI::Combo meshes = {};

				for (auto& [handle, asset] : Project::Get()->GetAssets()->GetCollection().GetAssets())
				{
					if (asset->GetType() != AssetType::Mesh) continue;

					std::pair<std::string, UI::Combo::SelectionFunc> item = std::make_pair(asset->GetName(), [asset, selected = m_EntitiesRef->m_SelectedEntity]()
					{
						MeshComponent& mesh = Project::Get()->GetScenes().GetActive()->GetRegistry(Project::Get()->GetState()).GetEntity(selected).GetComponent<MeshComponent>();
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

				UI::Property("Mesh", meshes);
			}

			// Material Combo
			{
				UI::Combo materials = {};

				for (auto& [handle, asset] : Project::Get()->GetAssets()->GetCollection().GetAssets())
				{
					if (asset->GetType() != AssetType::Material) continue;

					std::pair<std::string, UI::Combo::SelectionFunc> item = std::make_pair(asset->GetName(), [asset, selected = m_EntitiesRef->m_SelectedEntity]()
					{
						MeshComponent& mesh = Project::Get()->GetScenes().GetActive()->GetRegistry(Project::Get()->GetState()).GetEntity(selected).GetComponent<MeshComponent>();
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

				UI::Property("Material", materials);
			}

			UI::EndPropertyGrid();
		}
	}

	template<>
	void Components::RenderComponent<PointLightComponent>(Entity& entity)
	{
		APP_LOG_TRACE("TODO");
	}

}