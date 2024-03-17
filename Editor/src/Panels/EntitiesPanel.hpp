#pragma once

#include <Lavender/Utils/Utils.hpp>
#include <Lavender/Utils/UUID.hpp>

#include <Lavender/Workspace/Project.hpp>

#include <Lavender/UI/Style.hpp>

namespace Lavender
{

	class EntitiesPanel
	{
	public:
		EntitiesPanel(Ref<Project> project);
		virtual ~EntitiesPanel();

		void RenderUI();

		static Ref<EntitiesPanel> Create(Ref<Project> project);

	private:
		Ref<Project> m_Project = nullptr;

		UUID m_SelectedUUID = {};
	};

}