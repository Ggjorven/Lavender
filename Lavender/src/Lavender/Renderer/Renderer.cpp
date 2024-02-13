#include "lvpch.h"
#include "Renderer.hpp"

#include "Lavender/Core/Logging.hpp"

#include <vulkan/vulkan.h>

#include "Lavender/Renderer/IndexBuffer.hpp"

#include "Lavender/APIs/Vulkan/VulkanManager.hpp"
#include "Lavender/APIs/Vulkan/Setup/VulkanResources.hpp"
#include "Lavender/Renderer/GraphicsContext.hpp"

#include "Lavender/Utils/Profiler.hpp"

namespace Lavender
{

	Renderer* Renderer::s_Instance = nullptr;
	APISpecifications Renderer::s_APISpecs = {};

	void Renderer::Init(const APISpecifications& specs)
	{
		s_APISpecs = specs;
		s_Instance = new Renderer();
	}

	void Renderer::Destroy()
	{
		delete s_Instance;
	}

	void Renderer::Wait()
	{
		VulkanDeviceInfo& info = VulkanManager::GetDeviceInfo();

		vkDeviceWaitIdle(info.Device);
	}

	void Renderer::SetClearColour(const glm::vec4& colour)
	{
		Renderer::GetAPISpecs().ClearColour = colour;
	}

	void Renderer::DrawIndexed(std::shared_ptr<IndexBuffer>& indexBuffer)
	{
		LV_PROFILE_SCOPE("DrawIndexed");

		auto& resourceInfo = VulkanManager::GetResourceInfo();

		vkCmdDrawIndexed(resourceInfo.CommandBuffers[Renderer::GetCurrentFrame()], indexBuffer->GetCount(), 1, 0, 0, 0);
	}

	void Renderer::OnResize(uint32_t width, uint32_t height)
	{
		VulkanManager::RecreateSwapChain();
	}

	void Renderer::AddToQueue(RenderFunction function)
	{
		LV_PROFILE_SCOPE("AddToRenderQueue");
		s_Instance->m_RenderQueue.push(function);
	}

	void Renderer::AddToUIQueue(UIFunction function)
	{
		LV_PROFILE_SCOPE("AddToUIQueue");
		s_Instance->m_UIQueue.push(function);
	}

	void Renderer::Display()
	{
		LV_PROFILE_SCOPE("Display");
		s_Instance->QueuePresent();
	}

	void Renderer::QueuePresent()
	{
		auto& deviceInfo = VulkanManager::GetDeviceInfo();
		auto& swapchainInfo = VulkanManager::GetSwapChainInfo();
		auto& resourceInfo = VulkanManager::GetResourceInfo();

		{
			LV_PROFILE_SCOPE("WaitForFences");
			vkWaitForFences(deviceInfo.Device, 1, &resourceInfo.InFlightFences[s_Instance->m_CurrentFrame], VK_TRUE, UINT64_MAX);
		}

		uint32_t imageIndex;

		VkResult result;
		{
			LV_PROFILE_SCOPE("AquireImage");
			result = vkAcquireNextImageKHR(deviceInfo.Device, swapchainInfo.SwapChain, UINT64_MAX, resourceInfo.ImageAvailableSemaphores[s_Instance->m_CurrentFrame], VK_NULL_HANDLE, &imageIndex);
		}
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			VulkanManager::RecreateSwapChain();
			return;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
			LV_LOG_ERROR("Failed to acquire swap chain image!");

		// Only reset the fence if we actually submit the work
		vkResetFences(deviceInfo.Device, 1, &resourceInfo.InFlightFences[s_Instance->m_CurrentFrame]);

		vkResetCommandBuffer(resourceInfo.CommandBuffers[s_Instance->m_CurrentFrame], 0);

		// Note(Jorben): Record the command buffer with all items in the queue
		RecordCommandBuffer(resourceInfo.CommandBuffers[s_Instance->m_CurrentFrame], imageIndex);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { resourceInfo.ImageAvailableSemaphores[s_Instance->m_CurrentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &resourceInfo.CommandBuffers[s_Instance->m_CurrentFrame];

		VkSemaphore signalSemaphores[] = { resourceInfo.RenderFinishedSemaphores[s_Instance->m_CurrentFrame] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		{
			LV_PROFILE_SCOPE("SubmitCommandBuffer");
			if (vkQueueSubmit(deviceInfo.GraphicsQueue, 1, &submitInfo, resourceInfo.InFlightFences[s_Instance->m_CurrentFrame]) != VK_SUCCESS)
				LV_LOG_ERROR("Failed to submit draw command buffer!");
		}

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { swapchainInfo.SwapChain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr; // Optional

		// Check for the result on present again
		{
			LV_PROFILE_SCOPE("QueuePresent");
			result = vkQueuePresentKHR(deviceInfo.PresentQueue, &presentInfo);
		}

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
			VulkanManager::RecreateSwapChain();
		else if (result != VK_SUCCESS)
			LV_LOG_ERROR("Failed to present swap chain image!");

		// Note(Jorben): We use the & operator since MAX_FRAMES_IN_FLIGHT is a power of 2 and this is a lot cheaper, if it's not use the % operator
		m_CurrentFrame = (m_CurrentFrame + 1) % LV_MAX_FRAMES_IN_FLIGHT;
	}

	void Renderer::RecordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex)
	{
		LV_PROFILE_SCOPE("RecordCommandBuffer");

		VulkanSwapChainInfo& swapchainInfo = VulkanManager::GetSwapChainInfo();

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		{
			LV_PROFILE_SCOPE("BeginCmdBuf");
			if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
				LV_LOG_ERROR("Failed to begin recording command buffer!");
		}

		//
		// TODO(Jorben): Create a custom way to start a render pass
		//
		VulkanRenderPass* renderPass = GraphicsContext::Get()->GetResources()->GetRenderPasses()[0]; // Get the first one for now

		std::vector<VkClearValue> clearValues = {};
		if (renderPass->HasDepth()) clearValues.resize(2);
		else clearValues.resize(1);

		APISpecifications specs = Renderer::GetAPISpecs();
		clearValues[0].color = { {specs.ClearColour.r, specs.ClearColour.g, specs.ClearColour.b, specs.ClearColour.a} };

		if (renderPass->HasDepth())
		{
			clearValues[1].depthStencil = { 1.0f, 0 };
		}

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass->m_RenderPass;
		renderPassInfo.framebuffer = renderPass->m_SwapChainFramebuffers[imageIndex];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapchainInfo.SwapChainExtent;

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		{
			LV_PROFILE_SCOPE("BeginRenderPass");
			vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		}

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)swapchainInfo.SwapChainExtent.width;
		viewport.height = (float)swapchainInfo.SwapChainExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = swapchainInfo.SwapChainExtent;
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		// Execute all commands
		while (!s_Instance->m_RenderQueue.empty())
		{
			auto& func = s_Instance->m_RenderQueue.front();
			func();
			s_Instance->m_RenderQueue.pop();
		}
		while (!s_Instance->m_UIQueue.empty())
		{
			auto& func = s_Instance->m_UIQueue.front();
			func();
			s_Instance->m_UIQueue.pop();
		}

		{
			LV_PROFILE_SCOPE("EndRenderPass");
			vkCmdEndRenderPass(commandBuffer);
		}

		{
			LV_PROFILE_SCOPE("EndCmdBuf");
			if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
				LV_LOG_ERROR("Failed to record command buffer!");
		}
	}

}