#include "lvpch.h"
#include "VulkanViewport.hpp"

#include "Lavender/Core/Application.hpp"
#include "Lavender/Core/Logging.hpp"
#include "Lavender/Utils/Profiler.hpp"

#include "Lavender/Renderer/Mesh.hpp"
#include "Lavender/Renderer/Shader.hpp"
#include "Lavender/Renderer/Renderer.hpp"
#include "Lavender/Renderer/UniformBuffer.hpp"
#include "Lavender/Renderer/FrameResources.hpp"

#include "Lavender/APIs/Vulkan/VulkanAllocator.hpp"
#include "Lavender/APIs/Vulkan/VulkanContext.hpp"
#include "Lavender/APIs/Vulkan/VulkanRenderPass.hpp"
#include "Lavender/APIs/Vulkan/VulkanImGuiLayer.hpp"

#include "Lavender/UI/UI.hpp"

#include <glm/glm.hpp>

#include <imgui_internal.h>
#include <backends/imgui_impl_vulkan.h>

namespace Lavender
{

	VulkanViewport::VulkanViewport(uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height)
	{
		ImageSpecification imageSpecs = {};
		imageSpecs.Usage = ImageSpecification::ImageUsage::Size;
		imageSpecs.Width = width;
		imageSpecs.Height = height;
		imageSpecs.Format = ImageSpecification::ImageFormat::BGRA; // Same as swapchain
		imageSpecs.Flags = ImageSpecification::ImageUsageFlags::Colour | ImageSpecification::ImageUsageFlags::Sampled | ImageSpecification::ImageUsageFlags::NoMipMaps;
		m_Image = RefHelper::RefAs<VulkanImage2D>(Image2D::Create(imageSpecs));

		RenderPassSpecification renderPassSpecs = {};
		renderPassSpecs.UsedAttachments = RenderPassSpecification::Attachments::Depth;
		renderPassSpecs.ColourLoadOp = RenderPassSpecification::ColourLoadOperation::Clear;
		renderPassSpecs.PreviousImageLayout = RenderPassSpecification::ImageLayout::Undefined;
		renderPassSpecs.FinalImageLayout = RenderPassSpecification::ImageLayout::ShaderRead;
		renderPassSpecs.ColourAttachment = m_Image;
		m_Renderpass = RenderPass::Create(renderPassSpecs);

		m_WindowStyle = UI::StyleList({
			{ UI::StyleType::WindowPadding, { 0.0f, 0.0f} }
		});

		m_WindowColours = UI::StyleColourList({
			{ UI::StyleColourType::SeparatorActive, UI::Colours::LighterTint },
			{ UI::StyleColourType::SeparatorHovered, UI::Colours::LighterTint }
		});

		auto image = RefHelper::RefAs<VulkanImage2D>(m_Image);
		m_ImGuiImage = (ImTextureID)ImGui_ImplVulkan_AddTexture(image->GetSampler(), image->GetImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}

	VulkanViewport::~VulkanViewport()
	{
		auto device = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetLogicalDevice()->GetVulkanDevice();
		vkDeviceWaitIdle(device);
		
		auto pool = ((VulkanImGuiLayer*)Application::Get().GetImGuiLayer())->GetVulkanDescriptorPool();
		vkFreeDescriptorSets(device, pool, 1, (VkDescriptorSet*)&m_ImGuiImage);
	}

	void VulkanViewport::BeginFrame()
	{
		LV_PROFILE_SCOPE("VulkanViewport::BeginFrame");
		m_Renderpass->Begin();

		FrameResources::GetPipeline()->Use(m_Renderpass->GetCommandBuffer());
	}

	void VulkanViewport::EndFrame()
	{
		LV_PROFILE_SCOPE("VulkanViewport::EndFrame");
		m_Renderpass->End();
		m_Renderpass->Submit();

		Renderer::WaitFor(m_Renderpass->GetCommandBuffer());
	}

	void VulkanViewport::BeginRender()
	{
		LV_PROFILE_SCOPE("VulkanViewport::BeginRender");
		m_WindowStyle.Push();
		m_WindowColours.Push();

		// To remove the tab bar.
		ImGuiWindowClass window = {};
		window.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
		ImGui::SetNextWindowClass(&window);

		UI::BeginWindow("Viewport", UI::WindowFlags::NoCollapse | UI::WindowFlags::NoDecoration | UI::WindowFlags::NoBackground | UI::WindowFlags::NoTitleBar | UI::WindowFlags::NoMove);

		auto imWindow = ImGui::GetCurrentWindow();
		auto position = imWindow->Pos;
		auto size = imWindow->Size;
		if ((uint32_t)size.x != m_Width || (uint32_t)size.y != m_Height)
		{
			Resize((uint32_t)size.x, (uint32_t)size.y);
		}
		m_Width = (uint32_t)size.x;
		m_Height = (uint32_t)size.y;
		//LV_LOG_TRACE("Width: {0}, Height: {1}", m_Width, m_Height);

		auto& mainWindow = Application::Get().GetWindow();
		m_XPos = (uint32_t)position.x - mainWindow.GetPositionX();
		m_YPos = (uint32_t)position.y - mainWindow.GetPositionY();

		auto region = ImGui::GetContentRegionAvail();
		ImGui::Image(GetImGuiTexture(), ImVec2(region.x, region.y), ImVec2(1.0f, 0.0f), ImVec2(0.0f, 1.0f)); // TODO: Replace with UI Image
	}

	void VulkanViewport::EndRender()
	{
		LV_PROFILE_SCOPE("VulkanViewport::EndRender");
		UI::EndWindow();

		m_WindowStyle.Pop();
		m_WindowColours.Pop();
	}

	bool VulkanViewport::InView(const glm::vec2& mainWindowPosition) const
	{
		if (mainWindowPosition.x >= m_XPos && mainWindowPosition.x <= m_XPos + m_Width &&
			mainWindowPosition.y >= m_YPos && mainWindowPosition.y <= m_YPos + m_Height)
		{
			return true;
		}

		return false;
	}

	void VulkanViewport::Resize()
	{
		Resize(m_Width, m_Height);
	}

	void VulkanViewport::Resize(uint32_t width, uint32_t height)
	{
		if (width != 0 && height != 0)
		{
			m_Image->Resize(width, height);

			auto swapchain = RefHelper::RefAs<VulkanContext>(Renderer::GetContext())->GetSwapChain();
			auto depth = swapchain->GetDepthImage();
			if (depth.Width != width || depth.Height != height)
				swapchain->ResizeDepth(width, height);

			m_Renderpass->Resize(width, height);
		}
	}

	glm::vec2 VulkanViewport::ConvertMousePosition(const glm::vec2& mainWindowPosition) const
	{
		return glm::vec2(mainWindowPosition.x - m_XPos, mainWindowPosition.y - m_YPos);
	}

	ImTextureID VulkanViewport::GetImGuiTexture()
	{
		return (ImTextureID)m_Image->GetUIImage();
	}

}