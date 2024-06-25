#pragma once

#include <Lavender/Core/Core.hpp>
#include <Lavender/Utils/Utils.hpp>

#include "Panels/Entities.hpp"

namespace Lavender::UI
{

	enum class ComponentUsage
	{
		None = 0, Opened = BIT(0), Remove = BIT(1)
	};

	class Components
	{
	public:
		Components(Ref<Entities> entities);
		virtual ~Components();

		void RenderUI();

		static Ref<Components> Create(Ref<Entities> entities);

	private:
		void InitStyles();

	private:
		Ref<Entities> m_EntitiesRef = nullptr;
		
		UI::StyleList m_Styles = {};
		UI::StyleList m_Colours = {};
	};

}