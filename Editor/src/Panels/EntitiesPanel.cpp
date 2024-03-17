#include "EntitiesPanel.hpp"

#include <Lavender/Core/Logging.hpp>

#include <Lavender/UI/UI.hpp>

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
		// To remove the tab bar.
		ImGuiWindowClass window = {};
		window.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
		ImGui::SetNextWindowClass(&window);

		UI::BeginWindow("Entities", UI::WindowFlags::NoCollapse | UI::WindowFlags::NoDecoration | UI::WindowFlags::NoBackground | UI::WindowFlags::NoTitleBar | UI::WindowFlags::NoMove);

		for (auto& entity : m_Project->GetSceneCollection().GetActive()->GetCollection()->GetMainRegistry()->GetEntityMap())
		{
			// Check for faulty UUID's
			if (entity.first == 0)
				break;

			bool selected = entity.first == m_SelectedUUID;
			UI::Selectable(std::to_string(entity.first.Get()), &selected);

			if (selected)
				m_SelectedUUID = entity.first;
		}

		UI::EndWindow();

		// To remove the tab bar.
		ImGui::SetNextWindowClass(&window);

		UI::BeginWindow("Components"/*, UI::WindowFlags::NoCollapse | UI::WindowFlags::NoDecoration | UI::WindowFlags::NoBackground | UI::WindowFlags::NoTitleBar | UI::WindowFlags::NoMove*/);

		// TagComponent
		auto registry = m_Project->GetSceneCollection().GetActive()->GetCollection()->GetMainRegistry();
		if (registry->HasComponent<TagComponent>(m_SelectedUUID))
		{
			TagComponent& tag = registry->GetComponent<TagComponent>(m_SelectedUUID);
			UI::Text("Tag: {}", tag.Tag);
		}

		UI::EndWindow();
	}

	Ref<EntitiesPanel> EntitiesPanel::Create(Ref<Project> project)
	{
		return RefHelper::Create<EntitiesPanel>(project);
	}

}