#pragma once

#include <Lavender/Utils/Utils.hpp>

#include <Lavender/Workspace/Project.hpp>

namespace Lavender
{

	class SettingsWindow
	{
	public:
		SettingsWindow(Ref<Project> project);
		virtual ~SettingsWindow();

		void RenderUI();

		inline void SetEnabled(bool state) { m_Enabled = state; }
		inline bool& GetEnabled() { return m_Enabled; }

		static Ref<SettingsWindow> Create(Ref<Project> project);

	private:
		bool CustomTree(const std::string& label);

	private:
		Ref<Project> m_Project = nullptr;

		bool m_Enabled = false;
		uint32_t m_Width = 0, m_Height = 0;
	};
}