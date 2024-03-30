#include "MaterialPanel.hpp"

#include <Lavender/Core/Logging.hpp>

#include <Lavender/Workspace/Assets/Asset.hpp>

#include <Lavender/UI/UI.hpp>
#include <Lavender/UI/Style.hpp>
#include <Lavender/UI/Colours.hpp>

#include <imgui.h>

namespace Lavender
{

	MaterialPanel::MaterialPanel(Ref<Project> project)
		: m_Project(project)
	{
		m_CheckerBoard = Image2D::Create(std::filesystem::path(Utils::ToolKit::GetEnv("LAVENDER_DIR") + "\\Editor\\assets\\images\\Checkerboard.tga"));
	}

	MaterialPanel::~MaterialPanel()
	{
	}

	void MaterialPanel::RenderUI()
	{
		if (!m_Enabled)
			return;

		// To remove the tab bar.
		ImGuiWindowClass window = {};
		window.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
		ImGui::SetNextWindowClass(&window);

		UI::ScopedStyleList styles = {{
			{ UI::StyleType::FrameRounding, 4.0f },
			{ UI::StyleType::FramePadding, { 15.0f, 6.0f } }
		}};

		UI::ScopedStyleList colours = {{
			{ UI::StyleColourType::WindowBg, UI::Colours::Background },
			{ UI::StyleColourType::Header, UI::Colours::BackgroundPopup },
			{ UI::StyleColourType::HeaderHovered, UI::Colours::LightTint },
			{ UI::StyleColourType::HeaderActive, UI::Colours::LighterTint },
			{ UI::StyleColourType::FrameBg, UI::Colours::AlphaTint },
			{ UI::StyleColourType::FrameBgHovered, UI::Colours::LightTint },
			{ UI::StyleColourType::FrameBgActive, UI::Colours::LighterTint },
			{ UI::StyleColourType::Button, UI::Colours::DarkTint },
			{ UI::StyleColourType::ButtonHovered, UI::Colours::DarkTint },
			{ UI::StyleColourType::ButtonActive, UI::Colours::DarkTint },
		}};

		UI::BeginWindow("Material", UI::WindowFlags::NoCollapse | UI::WindowFlags::NoDecoration | UI::WindowFlags::NoTitleBar | UI::WindowFlags::NoMove);

		auto size = ImGui::GetCurrentWindow()->Size;
		m_Width = (uint32_t)size.x;
		m_Height = (uint32_t)size.y;

		if (CustomTree("Material Editor"))
		{
			UI::Dummy({ 0.0f, 2.0f });
			UI::Draw::Underline();
			UI::Dummy({ 0.0f, 2.0f });

			UI::Combo materials = {};
			for (auto& asset : m_Project->GetSceneCollection().GetActive()->GetAssetManager()->GetAssets())
			{
				if (asset.second->GetStaticType() != AssetType::MaterialAsset)
					continue;

				auto path = asset.second->GetAssetPath();
				std::string name = path.filename().replace_extension().string();
				std::pair<std::string, UI::Combo::SelectionFunc> item = std::make_pair(name, [this, asset]()
				{
					m_SelectedAsset = asset.first;
				});

				materials.Items.push_back(item);
				if (m_SelectedAsset == asset.first)
					materials.Selected = item.first;
			}
			materials.Preview = "Select Material";

			{
				std::pair<std::string, UI::Combo::SelectionFunc> item = std::make_pair("--Create New--", [this]()
				{
					auto asset = MaterialAsset::Create();
					m_Project->GetSceneCollection().GetActive()->GetAssetManager()->AddAsset(asset);

					asset->SetAssetPath(m_Project->GetDirectories().ProjectDir / m_Project->GetDirectories().Assets / "new.lvmaterial");
					m_SelectedAsset = asset->GetHandle();
				});

				materials.Items.push_back(item);
			}

			ImGuiStyle& style = ImGui::GetStyle();
			ImGui::PushItemWidth(m_Width - style.FramePadding.x);
			materials.Render("Materials", UI::ComboFlags::HeightRegular);
			ImGui::PopItemWidth();

			UI::Dummy({ 0.0f, 2.0f });
			UI::Draw::Underline();
			UI::Dummy({ 0.0f, 2.0f });

			if (m_SelectedAsset != AssetHandle::Empty)
			{
				Ref<MaterialAsset> asset = RefHelper::RefAs<MaterialAsset>(m_Project->GetSceneCollection().GetActive()->GetAssetManager()->GetAsset(m_SelectedAsset));
				{
					static char buffer[256] = {};
					std::filesystem::path path = asset->GetAssetPath();
					std::string filename = path.filename().replace_extension().string();
					std::strncpy(buffer, filename.c_str(), sizeof(buffer) - 1);

					ImGuiStyle& style = ImGui::GetStyle();
					ImGui::PushItemWidth(m_Width - style.FramePadding.x);
					UI::InputText("##Material_Name_LavenderUI", buffer, sizeof(buffer));
					ImGui::PopItemWidth();

					path = asset->GetAssetPath();
					path.replace_filename(std::filesystem::path(buffer)).replace_extension(".lvmaterial");
					asset->SetAssetPath(path);
				}

				UI::BeginPropertyGrid(2);
				// Albedo
				{
					UI::ClickAbleImage image = {};
					image.Image = asset->GetAlbedo() ? asset->GetAlbedo() : m_CheckerBoard;
					image.Size = { 32.0f, 32.0f };
					image.Action = [this, asset]()
					{
						std::filesystem::path beginDir = Utils::ToolKit::ReplaceSlashes(m_Project->GetDirectories().ProjectDir);
						std::filesystem::path filename = std::filesystem::path(Utils::ToolKit::OpenFile("", beginDir.string()));

						if (!filename.empty())
						{
							asset->SetAlbedo(Image2D::Create(filename), filename);
						}
					};

					UI::Property("Albedo", image);
				}

				UI::EndPropertyGrid();
			}
		}

		UI::EndWindow();
	}

	Ref<MaterialPanel> MaterialPanel::Create(Ref<Project> project)
	{
		return RefHelper::Create<MaterialPanel>(project);
	}

	bool MaterialPanel::CustomTree(const std::string& label)
	{
		bool open = false;

		UI::ScopedStyleList styles = { {
			{ UI::StyleType::FrameBorderSize, 1.0f }
		} };

		UI::ScopedStyleList colours = { {
			{ UI::StyleColourType::Border, UI::Colours::BackgroundDark },
			{ UI::StyleColourType::Header, UI::Colours::BackgroundPopup },
			{ UI::StyleColourType::HeaderHovered, UI::Colours::LightTint },
			{ UI::StyleColourType::HeaderActive, UI::Colours::LighterTint },
		} };

		UI::TreeNodeFlags treenodeFlags = UI::TreeNodeFlags::Framed | UI::TreeNodeFlags::SpanAvailWidth | UI::TreeNodeFlags::FramePadding | UI::TreeNodeFlags::DefaultOpen;
		if (UI::TreeNode(label.c_str(), treenodeFlags))
		{
			open = true;
			UI::TreeNodePop();
		}
		UI::ShiftCursorY(-1.0f);

		return open;
	}

}