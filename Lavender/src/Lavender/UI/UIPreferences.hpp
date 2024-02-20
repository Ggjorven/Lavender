#pragma once

#include <filesystem>

namespace Lavender
{

	class PreferencesSerializer;

	class UIPreferences
	{
	public:
		UIPreferences();
		virtual ~UIPreferences();

		void Initialize();
		void SaveToString();

		inline std::string& GetImGuiIni() { return m_ImGuiIni; }

	private:
		std::string m_ImGuiIni = "Not set";

		friend class PreferencesSerializer;
	};

}