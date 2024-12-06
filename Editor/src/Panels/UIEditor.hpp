#pragma once

#include <imgui.h>
#include <imgui_internal.h>

namespace Lavender::UI
{

	class Editor
	{
	public:
		Editor() = default;
		virtual ~Editor() = default;

		void RenderWindow();
	};

}