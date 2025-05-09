#include "SettingsWindow.hpp"

#include <Lavender/Core/Logging.hpp>
#include <Lavender/Utils/Profiler.hpp>

#include <Lavender/UI/UI.hpp>
#include <Lavender/UI/Style.hpp>
#include <Lavender/UI/Colours.hpp>

namespace Lavender
{

	SettingsWindow::SettingsWindow(Ref<Project> project)
		: m_Project(project)
	{
	}

	SettingsWindow::~SettingsWindow()
	{
	}
	
	void SettingsWindow::RenderUI()
	{
		if (!m_Enabled)
			return;

		UI::ScopedStyleList styles = { {
			{ UI::StyleType::WindowRounding, 4.0f },
			{ UI::StyleType::FrameRounding, 4.0f },
			{ UI::StyleType::FramePadding, { 15.0f, 6.0f } }
		} };

		UI::ScopedStyleList colours = { {
			{ UI::StyleColourType::TitleBg, UI::Colours::AlphaTintNonAlpha },
			{ UI::StyleColourType::TitleBgActive, UI::Colours::AlphaTintNonAlpha },
			{ UI::StyleColourType::TitleBgCollapsed, UI::Colours::AlphaTintNonAlpha },
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
		} };

		UI::BeginWindow("Settings", m_Enabled, UI::WindowFlags::NoDocking);

		auto size = ImGui::GetCurrentWindow()->Size;
		m_Width = (uint32_t)size.x;
		m_Height = (uint32_t)size.y;

		if (CustomTree("Script"))
		{
			UI::Text("This is not to messed with if you want to have default configurations.");
			UI::ShiftCursorY(2.0f);
			UI::Text("Full path: '{0}'", m_Project->GetScript() ? m_Project->GetScript()->GetPath().string() : "");

			UI::ShiftCursorY(3.0f);
			bool valid = m_Project->GetScript() ? std::filesystem::exists(m_Project->GetScript()->GetPath()) : false;
			UI::Text("Is Valid: {0}", valid);
			UI::ShiftCursorY(3.0f);

			UI::BeginPropertyGrid(2);

			UI::Combo configs = {};
			configs.Preview = ScriptConfigToString(m_Project->GetScriptConfig());
			configs.Selected = ScriptConfigToString(m_Project->GetScriptConfig());

			auto project = m_Project;

			// Debug
			configs.Items.push_back(std::make_pair("Debug", [project]()
			{
				ProjectDirectories& dirs = project->GetDirectories();

				std::filesystem::path a = Utils::ToolKit::ReplaceSlashes(dirs.ProjectDir / dirs.Scripts);
				std::filesystem::path b = a / "bin\\Debug";
				std::filesystem::path scriptPath = b / ScriptLoader::DefaultPath;

				Ref<ScriptLoader> script = ScriptLoader::Create(scriptPath);
				project->SetScript(script);
				project->GetScriptConfig() = ScriptConfig::Debug;
			}));

			// Release
			configs.Items.push_back(std::make_pair("Release", [project]()
			{
				ProjectDirectories& dirs = project->GetDirectories();

				std::filesystem::path a = Utils::ToolKit::ReplaceSlashes(dirs.ProjectDir / dirs.Scripts);
				std::filesystem::path b = a / "bin\\Release";
				std::filesystem::path scriptPath = b / ScriptLoader::DefaultPath;

				Ref<ScriptLoader> script = ScriptLoader::Create(scriptPath);
				project->SetScript(script);
				project->GetScriptConfig() = ScriptConfig::Release;
			}));

			// Dist
			configs.Items.push_back(std::make_pair("Dist", [project]()
			{
				ProjectDirectories& dirs = project->GetDirectories();

				std::filesystem::path a = Utils::ToolKit::ReplaceSlashes(dirs.ProjectDir / dirs.Scripts);
				std::filesystem::path b = a / "bin\\Dist";
				std::filesystem::path scriptPath = b / ScriptLoader::DefaultPath;

				Ref<ScriptLoader> script = ScriptLoader::Create(scriptPath);
				project->SetScript(script);
				project->GetScriptConfig() = ScriptConfig::Dist;
			}));

			UI::Property("Config", configs);

			UI::EndPropertyGrid();
		}

		UI::EndWindow();
	}

	Ref<SettingsWindow> SettingsWindow::Create(Ref<Project> project)
	{
		return RefHelper::Create<SettingsWindow>(project);
	}

	bool SettingsWindow::CustomTree(const std::string& label)
	{
		bool open = false;

		UI::ScopedStyleList styles = UI::StyleList({
			{ UI::StyleType::FrameBorderSize, 1.0f }
		});

		UI::ScopedStyleList colours = UI::StyleColourList({
			{ UI::StyleColourType::Border, UI::Colours::BackgroundDark },
			{ UI::StyleColourType::Header, UI::Colours::BackgroundPopup },
			{ UI::StyleColourType::HeaderHovered, UI::Colours::LightTint },
			{ UI::StyleColourType::HeaderActive, UI::Colours::LighterTint },
		});

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