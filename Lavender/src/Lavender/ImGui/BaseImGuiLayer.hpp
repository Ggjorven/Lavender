#pragma once

#include "Lavender/Core/Layer.hpp"

namespace Lavender
{

	class BaseImGuiLayer : public Layer
	{
	public:
		BaseImGuiLayer() = default;
		virtual ~BaseImGuiLayer() = default;

		virtual void Begin() = 0;
		virtual void End() = 0;

		static BaseImGuiLayer* Create();
	};

}