#pragma once

#include "Lavender/Utils/Utils.hpp"

namespace Lavender
{

	class RenderCommandBuffer
	{
	public:
		RenderCommandBuffer() = default;
		virtual ~RenderCommandBuffer() = default;

		virtual void Begin() = 0;
		virtual void End() = 0;
		virtual void Submit() = 0;

		static Ref<RenderCommandBuffer> Create();
	};

}