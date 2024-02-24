#pragma once

#include "Lavender/Renderer/RenderInstance.hpp"

namespace Lavender
{

	class VulkanRenderer : public RenderInstance
	{
	public:
		VulkanRenderer(const RendererSpecification& specs);
		virtual ~VulkanRenderer();

		void BeginFrame() override;
		void EndFrame() override;

		void OnResize(uint32_t width, uint32_t height) override;
	};

}