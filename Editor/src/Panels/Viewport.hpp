#pragma once

#include "Lavender/Core/Core.hpp"
#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Renderer/RenderPass.hpp"

namespace Lavender::UI
{

	class Viewport
	{
	public:
		Viewport();
		virtual ~Viewport();

		void RenderUI();

		void Resize(uint32_t width, uint32_t height);
	};
}