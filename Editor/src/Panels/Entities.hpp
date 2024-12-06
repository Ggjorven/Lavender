#pragma once

#include <Lavender/Core/Core.hpp>
#include <Lavender/Utils/Utils.hpp>
#include <Lavender/Utils/UUID.hpp>

#include <Lavender/UI/Style.hpp>

namespace Lavender::UI
{

	class Viewport;
	class Components;

	class Entities
	{
	public:
		Entities();
		virtual ~Entities();

		void RenderUI();

		static Ref<Entities> Create();

	private:
		void InitStyles();

	private:
		UI::StyleList m_Styles = {};
		UI::StyleList m_Colours = {};

		UUID m_SelectedEntity = UUID::Empty;

		friend class Viewport;
		friend class Components;
	};

}