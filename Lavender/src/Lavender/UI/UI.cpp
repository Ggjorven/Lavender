#include "lvpch.h"
#include "UI.hpp"

#include "Lavender/Core/Logging.hpp"

namespace Lavender::UI
{

	void BeginWindow(const std::string& name, WindowFlags flags)
	{
		ImGui::Begin(name.c_str(), (bool*)0, (ImGuiWindowFlags)flags);
	}

	void EndWindow()
	{
		ImGui::End();
	}

	bool Button(const std::string& name, const glm::vec2& size)
	{
		return ImGui::Button(name.c_str(), ImVec2(size.x, size.y));
	}

	bool TreeNode(const std::string& name, TreeNodeFlags flags)
	{
		return ImGui::TreeNodeEx(name.c_str(), (ImGuiTreeNodeFlags)flags);
	}

	void TreeNodePop()
	{
		ImGui::TreePop();
	}

	void Selectable(const std::string& name, bool* selected, const glm::vec2& size, SelectableFlags flags)
	{
		ImGui::Selectable(name.c_str(), selected, (ImGuiSelectableFlags)flags, ImVec2(size.x, size.y));
	}

}