#pragma once

#include <memory>

namespace Lavender
{

	class RenderingContext
	{
	public:
		RenderingContext() = default;
		virtual ~RenderingContext() = default;

		virtual void Init() = 0;

		static std::shared_ptr<RenderingContext> Create();
	};

}