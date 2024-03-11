#include "lvpch.h"
#include "PreferencesSerializer.hpp"

#include "Lavender/Core/Logging.hpp"

#include <fstream>
#include <string>

#include <glm/glm.hpp>
#include "YAMLUtils.hpp"

namespace Lavender
{

	PreferencesSerializer::PreferencesSerializer(Ref<UIPreferences>& preferences)
		: m_Preferences(preferences)
	{
	}

	PreferencesSerializer::~PreferencesSerializer()
	{
	}

	void PreferencesSerializer::Serialize(const std::filesystem::path& file)
	{
		YAML::Emitter data;

		data << YAML::BeginMap;
		data << YAML::Key << "Preferences";
		data << YAML::Value << "Editor";

		// Before we need can save the string we need to actually fill the string
		m_Preferences->SaveToString();

		data << YAML::Key << "ImGui";
		data << YAML::Value << m_Preferences->GetImGuiIni();

		data << YAML::EndMap;

		std::ofstream fileStream(file);

		if (!fileStream.good() || !fileStream.is_open())
		{
			LV_LOG_ERROR("Failed to open file {0}", file.string().c_str());
			return;
		}

		fileStream << data.c_str();
		fileStream.close();
	}

	void PreferencesSerializer::Deserialize(const std::filesystem::path& file)
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(file.string());
		}
		catch (YAML::BadFile e)
		{
			LV_LOG_WARN("Failed to load {0} (Code: {1})\n\tLoading empty preferences.", file.string(), e.what());

			std::string defaultPreferences = "[Window][Debug##Default]\nPos=60,60\nSize=400,400\nCollapsed=0\n\n[Window][Abc]\nPos=15,25\nSize=285,130\nCollapsed=0\n\n[Window][DockSpaceViewport_11111111]\nPos=0,0\nSize=1920,1017\nCollapsed=0\n\n[Window][Viewport]\nPos=0,0\nSize=1920,1017\nCollapsed=0\nDockId=0x8B93E3BD,0\n\n[Docking][Data]\nDockSpace ID=0x8B93E3BD Window=0xA787BDB4 Pos=0,23 Size=1920,1017 CentralNode=1 Selected=0x13926F0B\n\n";
			m_Preferences->m_ImGuiIni = defaultPreferences;
			m_Preferences->Initialize();
			return;
		}

		// TODO: Remove?
		//auto preferences = data["Preferences"];
		//if (!preferences)
		//{
		//	LV_LOG_ERROR("Failed to find preferences tab.");
		//	return;
		//}
		
		auto imguiData = data["ImGui"];
		if (!imguiData)
		{
			LV_LOG_ERROR("Failed to find ImGui data tab.");
			return;
		}

		m_Preferences->m_ImGuiIni = imguiData.as<std::string>();
		m_Preferences->Initialize();
	}

}
