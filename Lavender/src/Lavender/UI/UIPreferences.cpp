#include "lvpch.h"
#include "UIPreferences.hpp"

#include "Lavender/Core/Logging.hpp"

#include <imgui.h>
#include <imgui_internal.h>

namespace Lavender
{

	UIPreferences::UIPreferences()
	{
	}

	UIPreferences::~UIPreferences()
	{
	}

	void UIPreferences::Initialize()
	{
		const char* settings = m_ImGuiIni.c_str();

		ImGuiIO& io = ImGui::GetIO();
		io.WantSaveIniSettings = true;

		ImGui::ClearIniSettings();
		ImGui::LoadIniSettingsFromMemory(settings, std::strlen(settings));

		settings = ImGui::SaveIniSettingsToMemory();
		io.WantSaveIniSettings = false;
	}

	void UIPreferences::SaveToString()
	{
		size_t iniSize = 0;
		const char* str = ImGui::SaveIniSettingsToMemory(&iniSize);

		m_ImGuiIni.clear();
		m_ImGuiIni.assign(str, iniSize);
	}

}