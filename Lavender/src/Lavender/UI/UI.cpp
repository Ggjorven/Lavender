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

}