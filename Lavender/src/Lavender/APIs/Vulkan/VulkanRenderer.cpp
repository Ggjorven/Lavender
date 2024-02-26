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

		auto swapchain = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetSwapChain();
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

	std::vector<VkSemaphore> VulkanRenderer::GetSemaphores()
	{
		uint32_t currentFrame = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetSwapChain()->GetCurrentFrame();

		std::vector<VkSemaphore> semaphores = { };
		semaphores.clear();
		//semaphores.resize(m_WaitForCommandBuffers.size());

		for (auto& cmd : m_WaitForCommandBuffers)
		{
			semaphores.push_back(cmd->GetRenderFinishedSemaphore(currentFrame));
		}

		return semaphores;
	}

}