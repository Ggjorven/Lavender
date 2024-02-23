#include "lvpch.h"
#include "VulkanRenderer.hpp"

#include "Lavender/Core/Logging.hpp"

namespace Lavender
{
	
	struct VulkanRenderData
	{
	public:
		uint32_t DrawCalls = 0;
		// TODO: Add more...
	};
	
	VulkanRenderData* s_RenderData = nullptr;

	VulkanRenderer::VulkanRenderer()
	{
		s_RenderData = new VulkanRenderData();
	}

	VulkanRenderer::~VulkanRenderer()
	{
		delete s_RenderData;
	}

	void VulkanRenderer::OnResize(uint32_t widht, uint32_t height)
	{
	}

}