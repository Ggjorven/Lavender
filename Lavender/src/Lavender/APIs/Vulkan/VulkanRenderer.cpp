#include "lvpch.h"
#include "VulkanRenderer.hpp"

#include "Lavender/Core/Logging.hpp"

#include "Lavender/Renderer/Renderer.hpp"

namespace Lavender
{
	
	struct VulkanRenderData
	{
	public:
		uint32_t DrawCalls = 0;
		// TODO: Add more...
	};
	
	VulkanRenderData* s_RenderData = nullptr;

	VulkanRenderer::VulkanRenderer(const RendererSpecification& specs)
	{
		s_RenderData = new VulkanRenderData();
	}

	VulkanRenderer::~VulkanRenderer()
	{
		// TODO: Wait till idle

		delete s_RenderData;
	}

	void VulkanRenderer::Display()
	{
	}

	void VulkanRenderer::OnResize(uint32_t widht, uint32_t height)
	{
	}

}