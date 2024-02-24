#pragma once

#include <memory>

#include "Lavender/Renderer/RenderConfig.hpp"
#include "Lavender/Utils/Utils.hpp"

namespace Lavender
{

	class RenderInstance
	{
	public:
		RenderInstance() = default;
		virtual ~RenderInstance() = default;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		virtual void OnResize(uint32_t width, uint32_t height) = 0;

		static RenderInstance* Create(const RendererSpecification& specs);

	protected:
		Utils::Queue<RenderFunction> m_RenderQueue = { };
	};

}