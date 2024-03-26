#pragma once

#include <Lavender/Utils/Utils.hpp>

#include <Lavender/Workspace/Project.hpp>

namespace Lavender
{

	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel(Ref<Project> project);
		virtual ~ContentBrowserPanel();

		void OnUpdate(float deltaTime);
		void RenderUI();

		static Ref<ContentBrowserPanel> Create(Ref<Project> project);

	private:
		Ref<Project> m_Project = nullptr;
	};

}