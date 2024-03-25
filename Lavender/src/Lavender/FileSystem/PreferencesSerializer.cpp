#include "lvpch.h"
#include "PreferencesSerializer.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/FileSystem/YAMLUtils.hpp"

#include <fstream>

#include <glm/glm.hpp>

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
		YAML::Emitter data = {};

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
			LV_LOG_ERROR("Failed to open file {0}", file.string());
			return;
		}

		fileStream << data.c_str();
		fileStream.close();
	}

	void PreferencesSerializer::Deserialize(const std::filesystem::path& file)
	{
		YAML::Node data = {};
		try
		{
			data = YAML::LoadFile(file.string());
		}
		catch (YAML::BadFile e)
		{
			LV_LOG_WARN("Failed to load {0} (Code: {1})\n\tLoading empty preferences.", file.string(), e.what());

			std::string defaultPreferences = "[Window][Debug##Default]\nPos=245,51\nSize=246,253\nCollapsed=0\n\n[Window][Abc]\nPos=15,25\nSize=285,130\nCollapsed=0\n\n[Window][DockSpaceViewport_11111111]\nPos=0,19\nSize=1280,701\nCollapsed=0\n\n[Window][Viewport]\nPos=217,19\nSize=798,701\nCollapsed=0\nDockId=0x00000004,0\n\n[Window][A]\nPos=272,120\nSize=234,158\nCollapsed=0\n\n[Window][Entities]\nPos=0,19\nSize=215,301\nCollapsed=0\nDockId=0x00000005\n\n[Window][Components]\nPos=0,322\nSize=215,398\nCollapsed=0\nDockId=0x00000006\n\n[Window][FullScreenOverlay]\nViewportPos=242,277\nViewportId=0x75F6D232\nSize=1280,720\nCollapsed=0\n\n[Window][Unnamed Window]\nPos=60,60\nSize=59,54\nCollapsed=0\n\n[Window][Popup]\nPos=356,214\nSize=553,298\nCollapsed=0\n\n[Window][Dear ImGui Demo]\nPos=1069,127\nSize=532,626\nCollapsed=0\n\n[Window][Dear ImGui Style Editor]\nPos=881,84\nSize=330,530\nCollapsed=0\n\n[Window][Debug]\nPos=1017,19\nSize=263,701\nCollapsed=0\nDockId=0x00000002\n\n[Docking][Data]\nDockSpace         ID=0x8B93E3BD Window=0xA787BDB4 Pos=86,128 Size=1280,701 Split=X Selected=0x13926F0B\n  DockNode        ID=0x00000001 Parent=0x8B93E3BD SizeRef=1015,701 Split=Y\n    DockNode      ID=0x00000007 Parent=0x00000001 SizeRef=1280,357 Selected=0xCBC84B5C\n    DockNode      ID=0x00000008 Parent=0x00000001 SizeRef=1280,361 Split=X\n      DockNode    ID=0x00000003 Parent=0x00000008 SizeRef=215,720 Split=Y Selected=0xA99A06B3\n        DockNode  ID=0x00000005 Parent=0x00000003 SizeRef=228,301 Selected=0xA99A06B3\n        DockNode  ID=0x00000006 Parent=0x00000003 SizeRef=228,398 Selected=0xA115F62D\n      DockNode    ID=0x00000004 Parent=0x00000008 SizeRef=798,720 CentralNode=1 Selected=0x995B0CF8\n  DockNode        ID=0x00000002 Parent=0x8B93E3BD SizeRef=263,701 Selected=0xAD6468A3\n\n";
			m_Preferences->m_ImGuiIni = defaultPreferences;
			m_Preferences->Initialize();
			return;
		}
		
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
