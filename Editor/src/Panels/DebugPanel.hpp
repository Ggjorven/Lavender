#pragma once

#include <Lavender/Utils/Utils.hpp>

#include <Lavender/Workspace/Project.hpp>

namespace Lavender
{

	class DebugPanel
	{
	public:
		DebugPanel(Ref<Project> project);
		virtual ~DebugPanel();

		void OnUpdate(float deltaTime);
		void RenderUI();

		static Ref<DebugPanel> Create(Ref<Project> project);

	private:
		bool CustomTree(const std::string& label);

	private:
		Ref<Project> m_Project = nullptr;

		uint32_t m_FPS = 0;
		float m_Frametime = 0.0;
	};

}