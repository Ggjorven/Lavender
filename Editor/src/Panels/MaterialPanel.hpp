#pragma once

#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Workspace/Project.hpp"

namespace Lavender
{

	class MaterialPanel
	{
	public:
		MaterialPanel(Ref<Project> project);
		virtual ~MaterialPanel();

		void RenderUI();

		inline void SetEnabled(bool state) { m_Enabled = state; }

	private:
		Ref<Project> m_Project = nullptr;

		bool m_Enabled = false;
	};

}