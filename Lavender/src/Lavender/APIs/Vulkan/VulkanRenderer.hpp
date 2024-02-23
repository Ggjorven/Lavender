#pragma once

#include "Lavender/Renderer/RenderInstance.hpp"

namespace Lavender
{

	class VulkanRenderer : public RenderInstance
	{
	public:
		VulkanRenderer(const RendererSpecification& specs);
		virtual ~VulkanRenderer();

		void Display() override;

		void OnResize(uint32_t widht, uint32_t height) override;
	};

}