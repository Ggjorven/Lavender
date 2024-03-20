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

			std::string defaultPreferences = "[Window][Debug##Default]\nPos=60,60\nSize=400,400\nCollapsed=0\n\n[Window][Abc]\nPos=15,25\nSize=285,130\nCollapsed=0\n\n[Window][DockSpaceViewport_11111111]\nPos=0,19\nSize=1280,701\nCollapsed=0\n\n[Window][Viewport]\nPos=229,19\nSize=1051,701\nCollapsed=0\nDockId=0x00000004,0\n\n[Window][A]\nPos=272,120\nSize=234,158\nCollapsed=0\n\n[Window][Entities]\nPos=0,19\nSize=227,340\nCollapsed=0\nDockId=0x00000005\n\n[Window][Components]\nPos=0,361\nSize=227,359\nCollapsed=0\nDockId=0x00000006\n\n[Window][FullScreenOverlay]\nViewportPos=242,277\nViewportId=0x75F6D232\nSize=1280,720\nCollapsed=0\n\n[Window][Unnamed Window]\nPos=60,60\nSize=59,54\nCollapsed=0\n\n[Window][Popup]\nPos=356,214\nSize=553,298\nCollapsed=0\n\n[Docking][Data]\nDockSpace       ID=0x8B93E3BD Window=0xA787BDB4 Pos=216,258 Size=1280,701 Split=Y Selected=0x13926F0B\n  DockNode      ID=0x00000007 Parent=0x8B93E3BD SizeRef=1280,357 Selected=0xCBC84B5C\n  DockNode      ID=0x00000008 Parent=0x8B93E3BD SizeRef=1280,361 Split=X\n    DockNode    ID=0x00000003 Parent=0x00000008 SizeRef=227,720 Split=Y Selected=0xA99A06B3\n      DockNode  ID=0x00000005 Parent=0x00000003 SizeRef=228,340 Selected=0xA99A06B3\n      DockNode  ID=0x00000006 Parent=0x00000003 SizeRef=228,359 Selected=0xA115F62D\n    DockNode    ID=0x00000004 Parent=0x00000008 SizeRef=1051,720 CentralNode=1 Selected=0x995B0CF8\n\n";
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
