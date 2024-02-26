#pragma once

#include <memory>

#include "Lavender/Utils/Utils.hpp"

namespace Lavender
{

	class RenderingContext
	{
	public:
		RenderingContext() = default;
		virtual ~RenderingContext() = default;

		virtual void Init() = 0;
		virtual void Destroy() = 0;

		static Ref<RenderingContext> Create();
	};

}