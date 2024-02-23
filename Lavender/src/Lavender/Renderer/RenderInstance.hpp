#pragma once

#include <memory>

namespace Lavender
{

	class RenderInstance
	{
	public:
		RenderInstance() = default;
		virtual ~RenderInstance() = default;

		virtual void OnResize(uint32_t width, uint32_t height) = 0;

		static RenderInstance* Create();
	};

}