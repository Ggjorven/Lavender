#include "lvpch.h"
#include "VulkanRenderer.hpp"

#include "Lavender/Core/Logging.hpp"
#include "Lavender/Utils/Utils.hpp"

#include "Lavender/Renderer/Renderer.hpp"

#include "Lavender/APIs/Vulkan/VulkanContext.hpp"

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
		VkDevice& device = Utils::As<VulkanContext>(Renderer::GetContext())->GetLogicalDevice()->GetVulkanDevice();
		vkDeviceWaitIdle(device);

		delete s_RenderData;
	}

	void VulkanRenderer::Display()
	{
	}

	void VulkanRenderer::OnResize(uint32_t widht, uint32_t height)
	{
	}

}