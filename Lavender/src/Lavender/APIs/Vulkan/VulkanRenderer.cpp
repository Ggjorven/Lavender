#include "lvpch.h"
#include "VulkanRenderer.hpp"

#include "Lavender/Core/Logging.hpp"
#include "Lavender/Core/Application.hpp"
#include "Lavender/Utils/Utils.hpp"
#include "Lavender/Utils/Profiler.hpp"

#include "Lavender/Renderer/Renderer.hpp"
#include "Lavender/Renderer/IndexBuffer.hpp"

#include "Lavender/APIs/Vulkan/VulkanContext.hpp"
#include "Lavender/APIs/Vulkan/VulkanRenderCommandBuffer.hpp"

namespace Lavender
{
	
	RenderData* s_RenderData = nullptr;

	VulkanRenderer::VulkanRenderer()
	{
		s_RenderData = new RenderData();
	}

	VulkanRenderer::~VulkanRenderer()
	{
		VkDevice& device = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetLogicalDevice()->GetVulkanDevice();
		vkDeviceWaitIdle(device);

		delete s_RenderData;
	}

	void VulkanRenderer::BeginFrame()
	{
		if (Application::Get().IsMinimized())
			return;

		s_RenderData->Reset();

		// TODO: Execute resourceFree queue

		auto context = RefHelper::RefAs<VulkanContext>(Renderer::GetContext());

		{
			LV_PROFILE_SCOPE("Wait for fences");
			std::vector<VkFence> fences = { };
			for (auto& cmd : m_WaitForCommandBuffers) // These are the WaitCommandBuffers from the last frame
				fences.push_back(cmd->GetInFlightFence(context->GetSwapChain()->GetCurrentFrame()));

			// Wait for all previous fences
			if (fences.size() > 0)
			{
				vkWaitForFences(context->GetLogicalDevice()->GetVulkanDevice(), (uint32_t)fences.size(), fences.data(), VK_TRUE, LV_MAX_UINT64);
				vkResetFences(context->GetLogicalDevice()->GetVulkanDevice(), (uint32_t)fences.size(), fences.data());
			}

			m_WaitForCommandBuffers.clear();
			VulkanRenderCommandBuffer::ResetSemaphore();
		}

		auto swapchain = context->GetSwapChain();
		swapchain->BeginFrame();
	}

	void VulkanRenderer::EndFrame() // A.k.a Display/Present
	{
		if (Application::Get().IsMinimized())
			return;

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

	void VulkanRenderer::Wait()
	{
		LV_PROFILE_SCOPE("VulkanRenderer::Wait");

		VkDevice& device = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetLogicalDevice()->GetVulkanDevice();
		vkDeviceWaitIdle(device);
	}

	void VulkanRenderer::DrawIndexed(Ref<RenderCommandBuffer> commandBuffer, Ref<IndexBuffer> indexBuffer)
	{
		LV_PROFILE_SCOPE("VulkanRenderer::DrawIndexed");
		s_RenderData->DrawCalls++;

		auto cmdBuf = RefHelper::RefAs<VulkanRenderCommandBuffer>(commandBuffer);
		vkCmdDrawIndexed(cmdBuf->GetVulkanCommandBuffer(), indexBuffer->GetCount(), 1, 0, 0, 0);
	}

	void VulkanRenderer::OnResize(uint32_t width, uint32_t height)
	{
		auto swapchain = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetSwapChain();
		swapchain->OnResize(width, height, Application::Get().GetWindow().IsVSync());
	}

	RenderData VulkanRenderer::GetRenderData()
	{
		return *s_RenderData;
	}

}