#include "lvpch.h"
#include "VulkanViewport.hpp"

#include "Lavender/Core/Application.hpp"
#include "Lavender/Core/Logging.hpp"
#include "Lavender/Utils/Profiler.hpp"

#include "Lavender/Workspace/Project.hpp"

#include "Lavender/Renderer/Mesh.hpp"
#include "Lavender/Renderer/Shader.hpp"
#include "Lavender/Renderer/Renderer.hpp"
#include "Lavender/Renderer/UniformBuffer.hpp"
#include "Lavender/Renderer/FrameResources.hpp"
#include "Lavender/Renderer/Image.hpp"

#include "Lavender/APIs/Vulkan/VulkanContext.hpp"

#include "Lavender/UI/UI.hpp"

#include <glm/glm.hpp>

#include <imgui_internal.h>
#include <backends/imgui_impl_vulkan.h>

namespace Lavender
{

	static Ref<Image2D> s_PlayImage = nullptr;
	static Ref<Image2D> s_StopImage = nullptr;

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

		// Images
		ImageSpecification uiImageSpecs = {};
		uiImageSpecs.Usage = ImageSpecification::ImageUsage::File;
		uiImageSpecs.Format = ImageSpecification::ImageFormat::RGBA;
		uiImageSpecs.Flags = ImageSpecification::ImageUsageFlags::Colour | ImageSpecification::ImageUsageFlags::Sampled | ImageSpecification::ImageUsageFlags::NoMipMaps;

		uiImageSpecs.Path = Utils::ToolKit::GetEnv() + "/Editor/assets/images/Play.png";
		s_PlayImage = Image2D::Create(uiImageSpecs);

		uiImageSpecs.Path = Utils::ToolKit::GetEnv() + "/Editor/assets/images/Stop.png";
		s_StopImage = Image2D::Create(uiImageSpecs);
	}

	VulkanViewport::~VulkanViewport()
	{
		s_PlayImage.reset();
		s_StopImage.reset();
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

	static glm::vec2 s_TopLeftCursorPos = {};
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

		s_TopLeftCursorPos = glm::vec2(ImGui::GetCursorPos().x, ImGui::GetCursorPos().y);

		auto imWindow = ImGui::GetCurrentWindow();
		auto position = imWindow->Pos;
		auto size = imWindow->Size;
		if ((uint32_t)size.x != m_Width || (uint32_t)size.y != m_Height)
		{
			Resize((uint32_t)size.x, (uint32_t)size.y);
		}
		m_Width = (uint32_t)size.x;
		m_Height = (uint32_t)size.y;

		auto& mainWindow = Application::Get().GetWindow();
		m_XPos = (uint32_t)position.x - mainWindow.GetPositionX();
		m_YPos = (uint32_t)position.y - mainWindow.GetPositionY();

		auto region = ImGui::GetContentRegionAvail();
		ImGui::Image(GetImGuiTexture(), ImVec2(region.x, region.y), ImVec2(1.0f, 0.0f), ImVec2(0.0f, 1.0f));
	}

	void VulkanViewport::RenderUI(Project* project)
	{
		LV_PROFILE_SCOPE("VulkanViewport::RenderUI");

		UI::SetCursorPos(s_TopLeftCursorPos);

		glm::vec2 buttonSize = { 32.0f, 32.0f };
		UI::ShiftCursorX(((float)m_Width / 2.0f) - (buttonSize.x / 2.0f));
		{
			UI::ScopedStyleList colours = UI::StyleColourList({
				{ UI::StyleColourType::Button, { 0.0f, 0.0f, 0.0f, 0.0f } },
				{ UI::StyleColourType::ButtonHovered, { 0.0f, 0.0f, 0.0f, 0.0f } },
				{ UI::StyleColourType::ButtonActive, { 0.0f, 0.0f, 0.0f, 0.0f } }
			});

			ImTextureID id = nullptr;
			switch (project->GetState())
			{
			case ProjectState::Editor:
				id = s_PlayImage->GetUIImage();
				break;
			case ProjectState::Runtime:
				id = s_StopImage->GetUIImage();
				break;

			default:
				LV_LOG_FATAL("No proper state selected.");
				break;
			}

			if (ImGui::ImageButton(id, ImVec2(buttonSize.x, buttonSize.y)))
				project->SwitchState();
		}
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