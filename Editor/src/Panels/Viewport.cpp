#include "Viewport.hpp"

#include "Lavender/Utils/Profiler.hpp"

#include "Lavender/Renderer/Renderer.hpp"
#include "Lavender/Renderer/CommandBuffer.hpp"

#include "Lavender/WorkSpace/EngineTracker.hpp"

#include <imgui.h>

namespace Lavender::UI
{

	Viewport::Viewport()
	{
		ImageSpecification imageSpecs = {};
		imageSpecs.Usage = ImageUsage::Size;
		imageSpecs.Width = Track::Viewport::Width;
		imageSpecs.Height = Track::Viewport::Height;
		imageSpecs.Format = Renderer::GetSwapChainImages()[0]->GetSpecification().Format; 
		imageSpecs.Layout = ImageLayout::ShaderRead;
		imageSpecs.Flags = ImageUsageFlags::Colour | ImageUsageFlags::Sampled | ImageUsageFlags::NoMipMaps;
		imageSpecs.CreateUIImage = true;

		m_Attachment = Image2D::Create(imageSpecs);

		RenderPassSpecification renderpassSpecs = {};
		renderpassSpecs.ColourAttachment = { m_Attachment };
		renderpassSpecs.ColourLoadOp = LoadOperation::Clear;
		renderpassSpecs.PreviousColourImageLayout = ImageLayout::Undefined;
		renderpassSpecs.FinalColourImageLayout = ImageLayout::ShaderRead;

		renderpassSpecs.DepthAttachment = Renderer::GetDepthImage();
		renderpassSpecs.DepthLoadOp = LoadOperation::Clear;

		m_RenderPass = RenderPass::Create(renderpassSpecs, CommandBuffer::Create());
	}

	Viewport::~Viewport()
	{
	}

	void Viewport::StartRecording()
	{
		APP_PROFILE_SCOPE("Viewport::Start");
		m_RenderPass->Begin();
	}

	void Viewport::EndRecording()
	{
		APP_PROFILE_SCOPE("Viewport::End");

		m_RenderPass->End();
		m_RenderPass->Submit();
	}

	void Viewport::RenderUI()
	{
		APP_PROFILE_SCOPE("Viewport::UI");

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });

		ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_None);

		ImGui::Image(m_Attachment->GetTextureID(), { (float)m_Attachment->GetWidth(), (float)m_Attachment->GetHeight() });
		ImGui::End();

		ImGui::PopStyleVar(1);
	}

	void Viewport::Resize(uint32_t width, uint32_t height)
	{
		// Set size
		Track::Viewport::Width = width;
		Track::Viewport::Height = height;

		// Resize
		m_Attachment->Resize(width, height);
		m_RenderPass->Resize(width, height);
	}

}