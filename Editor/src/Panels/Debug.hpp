#pragma once

#include <Lavender/Core/Core.hpp>
#include <Lavender/Utils/Utils.hpp>

#include <Lavender/UI/Style.hpp>

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
	};

}