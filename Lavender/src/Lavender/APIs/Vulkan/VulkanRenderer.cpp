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
		VkDevice& device = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetLogicalDevice()->GetVulkanDevice();
		vkDeviceWaitIdle(device);

		delete s_RenderData;
	}

	void VulkanRenderer::BeginFrame()
	{
		// TODO: Execute resourceFree queue

		auto context = RefHelper::RefAs<VulkanContext>(Renderer::GetContext());

		std::vector<VkFence> fences = { };
		for (auto& cmd : m_WaitForCommandBuffers) // These are the WaitCommandBuffers from the last frame
			fences.push_back(cmd->GetInFlightFence((context->GetSwapChain()->GetCurrentFrame() + Renderer::GetSpecification().FramesInFlight - 1) % Renderer::GetSpecification().FramesInFlight));
		
		// Wait for all previous fences
		if (fences.size() > 0)
			vkWaitForFences(context->GetLogicalDevice()->GetVulkanDevice(), (uint32_t)fences.size(), fences.data(), VK_TRUE, UINT64_MAX);

		m_WaitForCommandBuffers.clear();
		VulkanRenderCommandBuffer::ResetSemaphore();

		auto swapchain = context->GetSwapChain();
		swapchain->BeginFrame();
	}

	void VulkanRenderer::EndFrame() // A.k.a Display/Present
	{
		auto swapchain = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetSwapChain();
		swapchain->EndFrame();
	}

	void VulkanRenderer::Submit(RenderFunction function)
	{
		m_RenderQueue.Add(function);
	}

	void VulkanRenderer::WaitFor(Ref<RenderCommandBuffer> commandBuffer)
	{
		m_WaitForCommandBuffers.push_back(RefHelper::RefAs<VulkanRenderCommandBuffer>(commandBuffer));
	}

	void VulkanRenderer::OnResize(uint32_t width, uint32_t height)
	{
		auto swapchain = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetSwapChain();
		swapchain->OnResize(width, height, Application::Get().GetWindow().IsVSync());
	}

}