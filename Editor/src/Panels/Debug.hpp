#pragma once

#include <Lavender/Core/Core.hpp>
#include <Lavender/Utils/Utils.hpp>

#include <Lavender/UI/Style.hpp>

#include <glm/glm.hpp>

namespace Lavender::UI
{

	class Debug
	{
	public:
		Debug();
		virtual ~Debug();

		void RenderUI();

		static Ref<Debug> Create();

	private:
		void InitStyles();

	private:
		UI::StyleList m_Styles = {};
		UI::StyleList m_Colours = {};

		// Window Data
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		glm::uvec2 m_Position = { 0u, 0u };
	};

}