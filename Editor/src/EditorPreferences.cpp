#include "EditorPreferences.hpp"

#include <Flow/Flow.hpp>

#include <imgui.h>
#include <imgui_internal.h>

using namespace Flow;

namespace Lavender::UI
{

	PreferencesSerializer::PreferencesSerializer(const std::filesystem::path& path)
		: m_Path(path)
	{
	}

	void PreferencesSerializer::Serialize()
	{
		Yaml::File file = Yaml::File(m_Path, FileMode::Write);
		file << Yaml::FileManip::BeginMap;

		file << Yaml::FileManip::Key << "Editor" << Yaml::FileManip::BeginMap;

		file << Yaml::FileManip::Key << "Preferences" << Yaml::FileManip::Value << AsString();

		file << Yaml::FileManip::EndMap;
		file << Yaml::FileManip::EndMap;
	}

	void PreferencesSerializer::Deserialize()
	{
		Yaml::File file = Yaml::File(m_Path, FileMode::Read);

		auto editor = file["Editor"];
		if (editor)
		{
			std::string preferences = editor["Preferences"].as<std::string>();
			Init(preferences);
		}
	}

	void PreferencesSerializer::Init(const std::string& iniData)
	{
		const char* settings = iniData.c_str();

		ImGuiIO& io = ImGui::GetIO();
		io.WantSaveIniSettings = true;

		ImGui::ClearIniSettings();
		ImGui::LoadIniSettingsFromMemory(settings, std::strlen(settings));

		settings = ImGui::SaveIniSettingsToMemory();
		io.WantSaveIniSettings = false;
	}

	std::string PreferencesSerializer::AsString()
	{
		size_t iniSize = 0;
		const char* str = ImGui::SaveIniSettingsToMemory(&iniSize);

		return std::string(str, iniSize);
	}

}